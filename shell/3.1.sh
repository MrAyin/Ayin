#!/bin/bash

# ������ڼ�¼�ļ���·��
attendance_dir="/home/buyhorse1/records/01-cnet"

# ͳ�Ƴ��ڴ���
declare -A attendance_count

# ������¼�ļ����е��ļ�
for file in "$attendance_dir"/*; do
  # ��ȡ�ļ��������ڲ��֣������ʽ��202409271611����ֻ����ǰ8λ��Ϊ����
  date_part=$(basename "$file" | cut -c1-8)

  # ʹ�������¼ÿ��ĳ��ڣ�ȷ��ÿ��ֻ����һ��
  declare -A student_attendance
  
  # ��ȡ�ļ��е�Ψһѧ���˺ţ������˷�ѧ���˺ŵ�����
  students=$(awk '{print $1}' "$file" | grep -E '^cnet[0-9]+$' | sort -u)
  
  for student in $students; do
    # ʹ�á�ѧ��-���ڡ���Ϊ����ȷ��ÿ��ѧ��ÿ��ֻ��һ��
    if [[ -z ${student_attendance["$student-$date_part"]} ]]; then
      student_attendance["$student-$date_part"]=1
      ((attendance_count["$student"]++))
    fi
  done
done

# ����ѧ���˺��������
for student in $(echo "${!attendance_count[@]}" | tr ' ' '\n' | sort); do
  echo "$student: ${attendance_count[$student]}"
done

# ����Լ��ĳ��ڴ���
my_id="cnet3952" # �û���ѧ��
echo "$my_id: ${attendance_count[$my_id]} (�Լ�)"
