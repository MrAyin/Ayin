#!/bin/bash

# 输入一个正整数 n
read -p "请输入一个正整数 n：" n

echo "1 到 $n 之间去掉包含 3 和 3 的倍数的数字："

for (( i=1; i<=n; i++ )); do
  # 检查是否包含数字 3 或是 3 的倍数
  if [[ "$i" =~ 3 ]] || (( i % 3 == 0 )); then
    continue
  fi
  echo "$i"
done
