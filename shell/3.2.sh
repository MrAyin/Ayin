#!/bin/bash

# ����һ�������� n
read -p "������һ�������� n��" n

# ��ʼ������������ͳ��
count=0

echo "1 �� $n ֮��Ļ������У�"

# ��� 1 �� n ֮�����
for (( i=1; i<=n; i++ )); do
  # ����ת��Ϊ�ַ���
  str="$i"
  # ��ת�ַ���
  rev_str=$(echo "$str" | rev)
  
  # ���ԭ���ͷ�ת�������ͬ�����ǻ�����
  if [[ "$str" == "$rev_str" ]]; then
    echo "$i"
    ((count++))
  fi
done

echo "�ܹ��ҵ� $count ����������"
