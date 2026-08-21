#!/bin/bash
# sync_cjson.sh - 从 cJSON 仓库同步 4 个核心文件到 struct2json/cJSON/
#
# 用法:
#   ./sync_cjson.sh [cJSON仓库路径]     # 默认 ../cJSON
#
# 说明:
#   本仓库通过 git subtree 方式管理内嵌 cJSON（历史见 cJSON-subtree-base 分支）。
#   为保持 cJSON/ 目录最小，仅同步 cJSON.c / cJSON.h / cJSON_Utils.c / cJSON_Utils.h
#   四个核心文件。同步后走编译+测试+valgrind 验证，通过后手动提交。
set -e

CJSON_REPO="${1:-../cJSON}"
FILES="cJSON.c cJSON.h cJSON_Utils.c cJSON_Utils.h"

cd "$(dirname "$0")"

if [ ! -d "$CJSON_REPO/.git" ]; then
    echo "错误: $CJSON_REPO 不是 git 仓库，请指定 cJSON 仓库路径"
    exit 1
fi

echo "== cJSON 仓库当前 HEAD: $(git -C "$CJSON_REPO" log --oneline -1) =="
echo "== 同步前 struct2json 内嵌版本最后提交 =="
git log --oneline -1 -- cJSON/ || true

for f in $FILES; do
    if ! cmp -s "$CJSON_REPO/$f" "cJSON/$f"; then
        echo "差异: $f"
    fi
done

# 复制 4 个核心文件
for f in $FILES; do
    cp "$CJSON_REPO/$f" "cJSON/$f"
done

echo "== 已复制 4 个核心文件，接下来验证 =="
# 编译并运行完整验证链
( cd demo && \
  gcc ../cJSON/cJSON.c ../struct2json/src/*.c ./*.c \
      -I ../cJSON/ -I ../struct2json/inc/ -lm -DDEBUGS2J -g -o /tmp/s2j_sync_test && \
  /tmp/s2j_sync_test > /dev/null && \
  valgrind --leak-check=full --error-exitcode=1 /tmp/s2j_sync_test > /dev/null 2>&1 ) \
  && echo "== 验证通过（编译+运行+valgrind）==" \
  || { echo "== 验证失败，回退文件 =="; git checkout -- cJSON/; exit 1; }

echo "== 完成。请检查 git diff 确认后手动提交，建议提交信息注明同步自 cJSON 仓库的提交号 =="
