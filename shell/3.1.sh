#!/bin/bash

# 定义出勤记录文件夹路径
attendance_dir="/home/buyhorse1/records/01-cnet"

# 统计出勤次数
declare -A attendance_count

# 遍历记录文件夹中的文件
for file in "$attendance_dir"/*; do
  # 提取文件名的日期部分（假设格式如202409271611），只保留前8位作为日期
  date_part=$(basename "$file" | cut -c1-8)

  # 使用数组记录每天的出勤，确保每天只计数一次
  declare -A student_attendance
  
  # 获取文件中的唯一学生账号，并过滤非学生账号的数据
  students=$(awk '{print $1}' "$file" | grep -E '^cnet[0-9]+$' | sort -u)
  
  for student in $students; do
    # 使用“学号-日期”作为键，确保每个学生每天只计一次
    if [[ -z ${student_attendance["$student-$date_part"]} ]]; then
      student_attendance["$student-$date_part"]=1
      ((attendance_count["$student"]++))
    fi
  done
done

# 按照学生账号排序并输出
for student in $(echo "${!attendance_count[@]}" | tr ' ' '\n' | sort); do
  echo "$student: ${attendance_count[$student]}"
done

# 输出自己的出勤次数
my_id="cnet3952" # 用户的学号
echo "$my_id: ${attendance_count[$my_id]} (自己)"
