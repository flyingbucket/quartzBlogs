# init 
```python
    def __init__(
        self,
        image_size,
        in_channels,
        model_channels,
        out_channels,
        num_res_blocks,
        attention_resolutions,
        dropout=0,
        channel_mult=(1, 2, 4, 8),
        conv_resample=True,
        dims=2,
        num_classes=None,
        use_checkpoint=False,
        use_fp16=False,
        num_heads=-1,
        num_head_channels=-1,
        num_heads_upsample=-1,
        use_scale_shift_norm=False,
        resblock_updown=False,
        use_new_attention_order=False,
        use_spatial_transformer=False,    # custom transformer support
        transformer_depth=1,              # custom transformer support
        context_dim=None,                 # custom transformer support
        n_embed=None,                     # custom support for prediction of discrete ids into codebook of first stage vq model
        legacy=True,
        disable_self_attentions=None,
        num_attention_blocks=None,
        disable_middle_self_attn=False,
        use_linear_in_transformer=False,
        semb_channels=None
    ):
        super().__init__()
        if use_spatial_transformer:
            assert context_dim is not None, 'Fool!! You forgot to include the dimension of your cross-attention conditioning...'

        if context_dim is not None:
            assert use_spatial_transformer, 'Fool!! You forgot to use the spatial transformer for your cross-attention conditioning...'
            from omegaconf.listconfig import ListConfig
            if type(context_dim) == ListConfig:
                context_dim = list(context_dim)

        if num_heads_upsample == -1:
            num_heads_upsample = num_heads

        if num_heads == -1:
            assert num_head_channels != -1, 'Either num_heads or num_head_channels has to be set'

        if num_head_channels == -1:
            assert num_heads != -1, 'Either num_heads or num_head_channels has to be set'

        self.image_size = image_size
        self.in_channels = in_channels
        self.model_channels = model_channels
        self.out_channels = out_channels
        if isinstance(num_res_blocks, int): # [[#residual block]]
            self.num_res_blocks = len(channel_mult) * [num_res_blocks]
        else:
            if len(num_res_blocks) != len(channel_mult):
                raise ValueError("provide num_res_blocks either as an int (globally constant) or "
                                 "as a list/tuple (per-level) with the same length as channel_mult")
            self.num_res_blocks = num_res_blocks
        if disable_self_attentions is not None: # [[#attention block]]
            # should be a list of booleans, indicating whether to disable self-attention in TransformerBlocks or not
            assert len(disable_self_attentions) == len(channel_mult)
        if num_attention_blocks is not None:
            assert len(num_attention_blocks) == len(self.num_res_blocks)
            assert all(map(lambda i: self.num_res_blocks[i] >= num_attention_blocks[i], range(len(num_attention_blocks))))
            print(f"Constructor of UNetModel received num_attention_blocks={num_attention_blocks}. "
                  f"This option has LESS priority than attention_resolutions {attention_resolutions}, "
                  f"i.e., in cases where num_attention_blocks[i] > 0 but 2**i not in attention_resolutions, "
                  f"attention will still not be set.")
		# [[#UNet 关键参数配置]] 
        self.attention_resolutions = attention_resolutions
        self.dropout = dropout
        self.channel_mult = channel_mult
        self.conv_resample = conv_resample
        self.num_classes = num_classes
        self.use_checkpoint = use_checkpoint
        self.dtype = th.float16 if use_fp16 else th.float32
        self.num_heads = num_heads
        self.num_head_channels = num_head_channels
        self.num_heads_upsample = num_heads_upsample
        self.predict_codebook_ids = n_embed is not None
		# [[#time_embed]]
        time_embed_dim = model_channels * 4
        self.time_embed = nn.Sequential(
            linear(model_channels, time_embed_dim),
            nn.SiLU(),
            linear(time_embed_dim, time_embed_dim),
        )
		# [[#类别条件（Class-Conditional）机制在 UNet 中的实现]]
        if self.num_classes is not None:
            if isinstance(self.num_classes, int):
                self.label_emb = nn.Embedding(num_classes, time_embed_dim)
            elif self.num_classes == "continuous":
                print("setting up linear c_adm embedding layer")
                self.label_emb = nn.Linear(1, time_embed_dim)
            else:
                raise ValueError()

        self.input_blocks = nn.ModuleList(
            [
                TimestepEmbedSequential(
                    conv_nd(dims, in_channels, model_channels, 3, padding=1)
                )
            ]
        )
        self._feature_size = model_channels
        input_block_chans = [model_channels]
        ch = model_channels
        ds = 1
        for level, mult in enumerate(channel_mult):
            for nr in range(self.num_res_blocks[level]):
                layers = [
                    ResBlockDual(
                        ch,
                        time_embed_dim,
                        dropout,
                        semb_channels=semb_channels,
                        out_channels=mult * model_channels,
                        dims=dims,
                        use_checkpoint=use_checkpoint,
                        use_scale_shift_norm=use_scale_shift_norm,
                    )
                ]
                ch = mult * model_channels
                if ds in attention_resolutions:
                    if num_head_channels == -1:
                        dim_head = ch // num_heads
                    else:
                        num_heads = ch // num_head_channels
                        dim_head = num_head_channels
                    if legacy:
                        #num_heads = 1
                        dim_head = ch // num_heads if use_spatial_transformer else num_head_channels
                    if exists(disable_self_attentions):
                        disabled_sa = disable_self_attentions[level]
                    else:
                        disabled_sa = False

                    if not exists(num_attention_blocks) or nr < num_attention_blocks[level]:
                        layers.append(
                            AttentionBlock(
                                ch,
                                use_checkpoint=use_checkpoint,
                                num_heads=num_heads,
                                num_head_channels=dim_head,
                                use_new_attention_order=use_new_attention_order,
                            ) if not use_spatial_transformer else SpatialTransformerV2(
                                ch, num_heads, dim_head, depth=transformer_depth, context_dim=context_dim,
                                disable_self_attn=disabled_sa, use_linear=use_linear_in_transformer,
                                use_checkpoint=use_checkpoint
                            )
                        )
                self.input_blocks.append(TimestepEmbedSequential(*layers))
                self._feature_size += ch
                input_block_chans.append(ch)
            if level != len(channel_mult) - 1:
                out_ch = ch
                self.input_blocks.append(
                    TimestepEmbedSequential(
                        ResBlockDual(
                            ch,
                            time_embed_dim,
                            dropout,
                            semb_channels=semb_channels,
                            out_channels=out_ch,
                            dims=dims,
                            use_checkpoint=use_checkpoint,
                            use_scale_shift_norm=use_scale_shift_norm,
                            down=True,
                        )
                        if resblock_updown
                        else Downsample(
                            ch, conv_resample, dims=dims, out_channels=out_ch
                        )
                    )
                )
                ch = out_ch
                input_block_chans.append(ch)
                ds *= 2
                self._feature_size += ch
		# [[#attention_head]] 
        if num_head_channels == -1:
            dim_head = ch // num_heads
        else:
            num_heads = ch // num_head_channels
            dim_head = num_head_channels
        if legacy:
            #num_heads = 1
            dim_head = ch // num_heads if use_spatial_transformer else num_head_channels
        self.middle_block = TimestepEmbedSequential(
            ResBlockDual(
                ch,
                time_embed_dim,
                dropout,
                semb_channels=semb_channels,
                dims=dims,
                use_checkpoint=use_checkpoint,
                use_scale_shift_norm=use_scale_shift_norm,
            ),
            AttentionBlock(
                ch,
                use_checkpoint=use_checkpoint,
                num_heads=num_heads,
                num_head_channels=dim_head,
                use_new_attention_order=use_new_attention_order,
            ) if not use_spatial_transformer else SpatialTransformerV2(  # always uses a self-attn
                            ch, num_heads, dim_head, depth=transformer_depth, context_dim=context_dim,
                            disable_self_attn=disable_middle_self_attn, use_linear=use_linear_in_transformer,
                            use_checkpoint=use_checkpoint
                        ),
            ResBlockDual(
                ch,
                time_embed_dim,
                dropout,
                semb_channels=semb_channels,
                dims=dims,
                use_checkpoint=use_checkpoint,
                use_scale_shift_norm=use_scale_shift_norm,
            ),
        )
        self._feature_size += ch

        self.output_blocks = nn.ModuleList([])
        for level, mult in list(enumerate(channel_mult))[::-1]:
            for i in range(self.num_res_blocks[level] + 1):
                ich = input_block_chans.pop()
                layers = [
                    ResBlockDual(
                        ch + ich,
                        time_embed_dim,
                        dropout,
                        semb_channels=semb_channels,
                        out_channels=model_channels * mult,
                        dims=dims,
                        use_checkpoint=use_checkpoint,
                        use_scale_shift_norm=use_scale_shift_norm,
                    )
                ]
                ch = model_channels * mult
                if ds in attention_resolutions:
                    if num_head_channels == -1:
                        dim_head = ch // num_heads
                    else:
                        num_heads = ch // num_head_channels
                        dim_head = num_head_channels
                    if legacy:
                        #num_heads = 1
                        dim_head = ch // num_heads if use_spatial_transformer else num_head_channels
                    if exists(disable_self_attentions):
                        disabled_sa = disable_self_attentions[level]
                    else:
                        disabled_sa = False

                    if not exists(num_attention_blocks) or i < num_attention_blocks[level]:
                        layers.append(
                            AttentionBlock(
                                ch,
                                use_checkpoint=use_checkpoint,
                                num_heads=num_heads_upsample,
                                num_head_channels=dim_head,
                                use_new_attention_order=use_new_attention_order,
                            ) if not use_spatial_transformer else SpatialTransformerV2(
                                ch, num_heads, dim_head, depth=transformer_depth, context_dim=context_dim,
                                disable_self_attn=disabled_sa, use_linear=use_linear_in_transformer,
                                use_checkpoint=use_checkpoint
                            )
                        )
                if level and i == self.num_res_blocks[level]:
                    out_ch = ch
                    layers.append(
                        ResBlockDual(
                            ch,
                            time_embed_dim,
                            dropout,
                            semb_channels=semb_channels,
                            out_channels=out_ch,
                            dims=dims,
                            use_checkpoint=use_checkpoint,
                            use_scale_shift_norm=use_scale_shift_norm,
                            up=True,
                        )
                        if resblock_updown
                        else Upsample(ch, conv_resample, dims=dims, out_channels=out_ch)
                    )
                    ds //= 2
                self.output_blocks.append(TimestepEmbedSequential(*layers))
                self._feature_size += ch

        self.out = nn.Sequential(
            normalization(ch),
            nn.SiLU(),
            zero_module(conv_nd(dims, model_channels, out_channels, 3, padding=1)),
        )
        if self.predict_codebook_ids:
            self.id_predictor = nn.Sequential(
            normalization(ch),
            conv_nd(dims, model_channels, n_embed, 1),
            #nn.LogSoftmax(dim=1)  # change to cross_entropy and produce non-normalized logits
        )
```

## attention_head

在使用多头注意力模块（如 AttentionBlock 或 SpatialTransformerV2）时，需要指定下列两个参数之一：

- `num_heads`：注意力头的数量（例如 8 表示使用 8 个并行注意力分支）
- `num_head_channels`：每个注意力头的通道宽度（例如 64 表示每个头维度为 64）

### 二者的关系

二者满足如下关系：
$$
\mathrm{num\_heads} \times \mathrm{num\_head\_channels} \leq \mathrm{total\_channels}
$$
你只需要显式指定一个，另一个可以自动推导。

### 参数检查逻辑

源代码中的校验逻辑如下：

```python
if num_heads == -1:
    assert num_head_channels != -1, 'Either num_heads or num_head_channels has to be set'

if num_head_channels == -1:
    assert num_heads != -1, 'Either num_heads or num_head_channels has to be set'
```

也就是说，**必须至少指定一个参数**，否则将抛出错误。

### ️ 注意事项

- 若 `total_channels` 无法整除指定参数，可能导致计算出错或维度不一致；
- 两者都指定时要确保一致性，即：`num_heads * num_head_channels == total_channels`；
- 推荐做法是：**设置你想控制的那一个，留另一个自动计算**。

###  示例

假设当前层通道数为 320：

- 若设置 `num_heads=8`，则 `num_head_channels=320 // 8 = 40`
- 若设置 `num_head_channels=64`，则 `num_heads=320 // 64 = 5`

```python
# 推荐示例
attention_block = AttentionBlock(
    channels=320,
    num_heads=8,
    num_head_channels=-1,  # 自动计算为 40
)

# 或者
attention_block = AttentionBlock(
    channels=320,
    num_heads=-1,
    num_head_channels=64,  # 自动计算为 5 heads
)
```

## residual block
这段代码用于配置UNet的各层中残差块（residual block）的数量。
```python
        if isinstance(num_res_blocks, int):
            self.num_res_blocks = len(channel_mult) * [num_res_blocks]
```
如果传入的`num_res_blocks`参数是单一整数，那么每一层将都使用这个数量的残差块。
```python
        else:
            if len(num_res_blocks) != len(channel_mult):
                raise ValueError("provide num_res_blocks either as an int (globally constant) or "
                                 "as a list/tuple (per-level) with the same length as channel_mult")
            self.num_res_blocks = num_res_blocks
```
如果传入的`num_res_blocks`参数不是整数，那么期望为列表或其他包含整数序列的有序容器，且该容器的长度应和`channel_mult`的长度（也即UNet的单测深度）相同。

## attention block
这段代码配置了attention block（自注意力机制）部分的启用情况。
**注意**：在StableSR中，自注意力和交叉注意力都是通过统一的transformer模块实现的，其自注意力机制并不是SR3那样传统的dot-product attention。StableSR中，该transformer可以选择是否开启自注意力部分，但默认必须开启交叉注意力以为UNet提供必要的生成条件。
```python
        if disable_self_attentions is not None:
            # should be a list of booleans, indicating whether to disable self-attention in TransformerBlocks or not
            assert len(disable_self_attentions) == len(channel_mult)
```

`disable_self_attentions`参数期望是None或者与`channel_mult`长度相同的布尔数组，用于控制UNet内各层是否开启自注意力模块。None表示全部开启，布尔数组则按照其真值控制。

```python
        if num_attention_blocks is not None:
            assert len(num_attention_blocks) == len(self.num_res_blocks)
            assert all(map(lambda i: self.num_res_blocks[i] >= num_attention_blocks[i], range(len(num_attention_blocks))))
            print(f"Constructor of UNetModel received num_attention_blocks={num_attention_blocks}. "
                  f"This option has LESS priority than attention_resolutions {attention_resolutions}, "
                  f"i.e., in cases where num_attention_blocks[i] > 0 but 2**i not in attention_resolutions, "
                  f"attention will still not be set.")
```
- 控制 **每一层中最多能插入多少个 attention 模块**；
- 不是层级数，而是实际 ResBlock 数量对 attention 的数量限制；
- 必须匹配结构深度，即 `num_attention_blocks[i] ≤ num_res_blocks[i]`。

- 打印语句更加清晰地表明了配置参数的优先级关系

	- **最终是否插入 attention block 的判定，首先看 `attention_resolutions`，然后才看 `num_attention_blocks`**。
	    
	
	也就是说：
	
	- 你可以设置 `num_attention_blocks[i] = 1` 表示“最多插入 1 个”；
	    
	- 但如果 `2**i` 不在 `attention_resolutions`（例如 16, 32）中，那 attention 就**不会插入**；
	    
	- 换句话说：你只表达了“允许插入”，**插不插要由 attention_resolutions 决定**。

## UNet 关键参数配置

| 参数                                | 说明                                      |
| --------------------------------- | --------------------------------------- |
| `attention_resolutions`           | 控制在哪些分辨率下插入 attention（如 16、32）          |
| `dropout`                         | dropout 概率，用于 regularization            |
| `channel_mult`                    | 每一层通道数是基通道数（`model_channels`）的几倍        |
| `conv_resample`                   | 是否使用卷积实现下/上采样（True 为可学习）                |
| `num_classes`                     | 是否使用 class-conditional 条件（如标签）          |
| `use_checkpoint`                  | 是否使用 gradient checkpointing 减少显存        |
| `dtype`                           | 网络中使用的精度（float16 或 float32）             |
| `num_heads` / `num_head_channels` | Attention 中的 head 设置                    |
| `num_heads_upsample`              | 上采样阶段的 head 数                           |
| `predict_codebook_ids`            | 是否输出 codebook token（即是否为 VQGAN decoder） |

## time_embed
**注意**：这是将时间信息引入UNet的核心途径。

```python
        time_embed_dim = model_channels * 4
        self.time_embed = nn.Sequential(
            linear(model_channels, time_embed_dim),
            nn.SiLU(),
            linear(time_embed_dim, time_embed_dim),
        )
```
这段代码构造了一个简单的 **MLP（多层感知机）**，将原始的 timestep embedding 映射成供 ResBlock 使用的时间条件向量。
- 输入维度：`model_channels`（例如 320）
- 输出维度：`time_embed_dim = 4 × model_channels`（例如 1280）
- 激活函数：`SiLU`（即 Swish 激活，效果比 ReLU 更平滑）

TODO:time_embed只是time aware encoder的一个模块，在进入他之前会有一个原始的time embeding（可能是正余弦位置编码），将编码结果输入这个time_embed的MLP来进一步添加可学习性，其输出用于调节残差块的工作模式。以上内容应该在后续代码和forward函数中有更多的体现。

## 类别条件（Class-Conditional）机制在 UNet 中的实现

### 一、什么是类别条件？

在扩散模型（如 DDPM、StableSR、LDM）中，类别条件是一种用于**引导图像生成方向**的辅助信息。通过在每一步扩散中注入类别标签，模型能够学习：

> 如何在第 t 步生成属于类别 y 的图像特征。

该机制适用于分类引导图像生成，例如：

- 生成一张“猫”而不是“狗”的图像
- 合成特定类型的建筑、车辆或自然场景图像
- 条件控制任务，如 super-resolution with category hints

---

### 二、实现方式

在 `UNetModelDualcondV2` 中，类别条件通过以下结构实现：

```python
if isinstance(self.num_classes, int):
    self.label_emb = nn.Embedding(num_classes, time_embed_dim)
elif self.num_classes == "continuous":
    self.label_emb = nn.Linear(1, time_embed_dim)
```

#### 支持两种类别条件格式：

| 类型       | 含义                      | 模块           | 输入形式                    |
|------------|---------------------------|----------------|-----------------------------|
| 离散类别   | 明确类别标签，如 0~9      | `nn.Embedding` | `y ∈ {0, ..., num_classes}` |
| 连续变量   | 属性值、实数条件           | `nn.Linear`    | `y ∈ ℝ`（形如 `[B, 1]`）    |

嵌入后将与时间嵌入相加，用于调节每个 ResBlock：

```python
emb = time_embed(t) + label_emb(y)
```

---

### 三、类别条件的来源

类别条件 **不是自动生成的**，而是 **由用户显式提供**：

- 对于分类任务：标签 `y` 来自数据集；
- 对于连续条件任务：如模糊程度、温度等数值，由用户设定；
- 不使用类别条件时，设置 `num_classes = None` 即可关闭。

---

### 四、关闭类别条件的方法

只需在初始化模型时设定：

```python
num_classes = None
```

即可完全关闭类别条件路径：

- 不构建 `label_emb`
- 时间嵌入 `emb = time_embed(t)` 不再包含类别信息
- 模型仅依赖时间步与其他条件（如结构条件）

---

### 五、技术意义

类别条件扩展了扩散模型的能力，使其支持：

- 分类控制生成（class-conditional generation）
- 多模态控制结构（如 time + class + structure）
- 通用控制器设计（支持标签、模态、风格等注入）

这一机制也为后续加入 cross-attention 等结构提供了条件输入的通道。


