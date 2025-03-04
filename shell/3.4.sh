#!/bin/bash

# 成绩单文件名
score_file="score.txt"

# 创建成绩单文件（如果不存在）
touch "$score_file"

# 定义菜单函数
menu() {
  echo "请选择操作："
  echo "1. 添加成绩"
  echo "2. 删除成绩"
  echo "3. 查询成绩"
  echo "4. 退出"
}

# 添加成绩
add_score() {
  read -p "输入学号：" id
  read -p "输入姓名：" name
  read -p "输入成绩：" score
  echo "$id $name $score" >> "$score_file"
  echo "成绩已添加。"
}

# 删除成绩
delete_score() {
  read -p "输入要删除的学号：" id
  sed -i "/^$id /d" "$score_file"
  echo "成绩已删除。"
}

# 查询成绩
query_score() {
  read -p "输入要查询的学号：" id
  result=$(grep "^$id " "$score_file")
  if [[ -n "$result" ]]; then
    echo "查询结果：$result"
  else
    echo "未找到该学号的成绩。"
  fi
}

# 主程序
while true; do
  menu
  read -p "请选择操作（1-4）：" choice
  case "$choice" in
    1) add_score ;;
    2) delete_score ;;
    3) query_score ;;
    4) echo "退出程序。"; break ;;
    *) echo "无效的选择，请重新输入。" ;;
  esac
done 
