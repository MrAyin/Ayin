#!/bin/bash

# �ɼ����ļ���
score_file="score.txt"

# �����ɼ����ļ�����������ڣ�
touch "$score_file"

# ����˵�����
menu() {
  echo "��ѡ�������"
  echo "1. ��ӳɼ�"
  echo "2. ɾ���ɼ�"
  echo "3. ��ѯ�ɼ�"
  echo "4. �˳�"
}

# ��ӳɼ�
add_score() {
  read -p "����ѧ�ţ�" id
  read -p "����������" name
  read -p "����ɼ���" score
  echo "$id $name $score" >> "$score_file"
  echo "�ɼ�����ӡ�"
}

# ɾ���ɼ�
delete_score() {
  read -p "����Ҫɾ����ѧ�ţ�" id
  sed -i "/^$id /d" "$score_file"
  echo "�ɼ���ɾ����"
}

# ��ѯ�ɼ�
query_score() {
  read -p "����Ҫ��ѯ��ѧ�ţ�" id
  result=$(grep "^$id " "$score_file")
  if [[ -n "$result" ]]; then
    echo "��ѯ�����$result"
  else
    echo "δ�ҵ���ѧ�ŵĳɼ���"
  fi
}

# ������
while true; do
  menu
  read -p "��ѡ�������1-4����" choice
  case "$choice" in
    1) add_score ;;
    2) delete_score ;;
    3) query_score ;;
    4) echo "�˳�����"; break ;;
    *) echo "��Ч��ѡ�����������롣" ;;
  esac
done 
