#!/bin/bash

# 输入一个正整数 n
read -p "请输入一个正整数 n：" n

# 初始化回文数个数统计
count=0

echo "1 到 $n 之间的回文数有："

# 检查 1 到 n 之间的数
for (( i=1; i<=n; i++ )); do
  # 将数转换为字符串
  str="$i"
  # 反转字符串
  rev_str=$(echo "$str" | rev)
  
  # 如果原数和反转后的数相同，则是回文数
  if [[ "$str" == "$rev_str" ]]; then
    echo "$i"
    ((count++))
  fi
done

echo "总共找到 $count 个回文数。"
