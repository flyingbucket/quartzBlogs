| 参数                          | 作用与说明                                                                             |
| --------------------------- | --------------------------------------------------------------------------------- |
| `image_size`                | 输入图像的大小，用于构建 U-Net 结构的层级，一般用于输入前的尺寸校验或推断网络深度。                                     |
| `in_channels`               | 输入图像的通道数，例如 RGB 为 3，灰度图为 1。                                                       |
| `model_channels`            | 模型的基础通道数，决定了第一层特征图的宽度。U-Net 的通道数通常是 `model_channels * mult` 形式增长。                 |
| `out_channels`              | 输出图像的通道数，通常与 `in_channels` 相同，例如 3 通道图像。也可以是 codebook 大小（用于 VQ）。                  |
| `num_res_blocks`            | 每个 downsample/upsample 层使用的 ResBlock 个数。可为 int（每层相同），也可为 list（不同层不同数量）。           |
| `attention_resolutions`     | 指定在哪些 resolution 下插入 attention 模块。例如包含 4 表示在 1/4 尺度特征图上加入 self-attn 或 cross-attn。 |
| `dropout`                   | Dropout 概率，控制网络的随机失活程度。用于提升泛化能力。                                                  |
| `channel_mult`              | U-Net 每个层级的通道扩展倍率。例如 `(1, 2, 4, 8)` 表示第4层为基通道的8倍。                                 |
| `conv_resample`             | 是否使用卷积方式进行上下采样（如 PixelShuffle、ConvTranspose），否则使用简单插值。                            |
| `dims`                      | 输入数据维度，`2` 表示 2D 图像，1 或 3 则用于序列或体数据。                                              |
| `num_classes`               | 分类条件数量。如果设置为整数，则模型是 class-conditional；支持 `int` 或 `"continuous"`。                  |
| `use_checkpoint`            | 是否开启 gradient checkpointing（反向传播时节省内存）。会牺牲速度换空间。                                  |
| `use_fp16`                  | 是否使用 float16 精度推理，主要用于减少显存。                                                       |
| `num_heads`                 | 注意力机制中 head 的数量，如果为 -1，则根据 `num_head_channels` 自动计算。                              |
| `num_head_channels`         | 每个 attention head 的维度，优先级高于 `num_heads`。二者需至少设置一个。                                |
| `num_heads_upsample`        | 用于上采样阶段的 attention head 数，默认与 `num_heads` 一致。                                     |
| `use_scale_shift_norm`      | 是否在 ResBlock 中使用 FiLM 风格的 scale-shift 归一化（用于条件建模）。                                |
| `resblock_updown`           | 是否使用残差块进行上/下采样，否则使用 Downsample/Upsample 模块。                                       |
| `use_new_attention_order`   | 控制 attention 顺序的实验性设置。默认为 False。                                                  |
| `use_spatial_transformer`   | 是否使用 Transformer 替代原生 AttentionBlock。开启后需设置 `context_dim`。                        |
| `transformer_depth`         | Transformer 模块的堆叠深度（层数）。                                                          |
| `context_dim`               | Cross-Attention 中 context（如文本、图像编码）的维度，必须与 transformer 配套。                        |
| `n_embed`                   | 如果不为 None，表示该模型用于预测 codebook 的离散 token（如 VQ-VAE / VQGAN 场景）。                      |
| `legacy`                    | 控制是否使用 legacy 的 attention 维度设置逻辑。True 表示保持旧实现兼容性。                                 |
| `disable_self_attentions`   | 是否禁用某些层中的 self-attn。为一个布尔列表，长度等于 `channel_mult`。                                  |
| `num_attention_blocks`      | 控制每个层的 attention block 数量。优先级低于 `attention_resolutions`。                          |
| `disable_middle_self_attn`  | 是否禁用 U-Net 最底部（中间层）的 self-attn。                                                   |
| `use_linear_in_transformer` | 是否在 transformer 中使用 Linear 形式的投影层。                                                |
| `semb_channels`             | **结构条件通道数**（例如结构图、小波子带等），会传入 `ResBlockDual` 进行时间/结构融合。                            |
## 与 Time-Aware Encoder 的关系

作为 Time-Aware Encoder，这些参数中以下几项起关键作用：

|参数|Time-Aware Encoder 中的意义|
|---|---|
|`t_emb`（隐含于 `time_embed`）|为每个时刻 `t` 生成时间嵌入，与结构一起参与生成（类似时间条件扩散）|
|`semb_channels`|支持结构条件的通道输入（如小波子带、深度图、边缘图等），通过 `ResBlockDual` 融合进入主干网络|
|`context_dim` + `use_spatial_transformer`|实现 Cross-Attention，用于将文本/图像上下文信息融合进特征流|
|`use_checkpoint` + `use_fp16`|控制训练时的计算效率，适合大规模时间序列训练|