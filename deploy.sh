#!/bin/bash

# 设置一些变量
MAIN_BRANCH="main"
GH_PAGES_BRANCH="gh-pages"
BUILD_DIR="public"
COMMIT_MESSAGE="Deploy to GitHub Pages"

# 检查当前分支是否为 main
CURRENT_BRANCH=$(git rev-parse --abbrev-ref HEAD)
if [ "$CURRENT_BRANCH" != "$MAIN_BRANCH" ]; then
  echo "当前分支不是 $MAIN_BRANCH，切换到 $MAIN_BRANCH 分支"
  git checkout $MAIN_BRANCH
fi

# 拉取最新代码
echo "拉取最新代码..."
git pull origin $MAIN_BRANCH

# 构建网站
echo "开始构建网站..."
npx quartz build -d $BUILD_DIR

# 切换到 gh-pages 分支
echo "切换到 $GH_PAGES_BRANCH 分支..."
git checkout $GH_PAGES_BRANCH

# 将 public 目录中的文件添加到 gh-pages 分支
echo "将 public/ 目录中的文件推送到 $GH_PAGES_BRANCH 分支..."
cp -r $BUILD_DIR/* .

# 提交并推送到 gh-pages 分支
git add .
git commit -m "$COMMIT_MESSAGE"
git push origin $GH_PAGES_BRANCH

# 切换回 main 分支
git checkout $MAIN_BRANCH

echo "部署完成！"
