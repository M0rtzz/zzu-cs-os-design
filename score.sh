#!/bin/zsh

# ------------------------------------------------------------------
# @file: score.sh
# @brief: 合并分数文件，并输出年级排名前十、各分数区间人数、平均分
# @author: M0rtzz
# @date: 2024-04-30
# ------------------------------------------------------------------

rm -f ./score/sorted_scores.txt

# 按分数逆序排序
{
    for file in ./score/*.txt; do
        awk '{print $1, $4, $5}' "${file}"
        # 文件之间添加换行符
        printf "\n"
    done
} | sort -k3 -rn >./score/sorted_scores.txt

# 输出年级排名前十
echo "年级排名前十："
head -n 10 ./score/sorted_scores.txt

# 统计各分数区间人数
echo "60以下人数："
awk '$3 < 60 {count++} END {print count}' ./score/sorted_scores.txt

echo "60-70人数："
awk '$3 >= 60 && $3 < 70 {count++} END {print count}' ./score/sorted_scores.txt

echo "70-80人数："
awk '$3 >= 70 && $3 < 80 {count++} END {print count}' ./score/sorted_scores.txt

echo "80-90人数："
awk '$3 >= 80 && $3 < 90 {count++} END {print count}' ./score/sorted_scores.txt

echo "90-100人数："
awk '$3 >= 90 && $3 <= 100 {count++} END {print count}' ./score/sorted_scores.txt

# 平均分
echo -n "平均分："
awk '{sum+=$3; count++} END {print sum/count}' ./score/sorted_scores.txt

# 删除后三行的换行符
sed -i '$d' ./score/sorted_scores.txt
sed -i '$d' ./score/sorted_scores.txt
sed -i '$d' ./score/sorted_scores.txt
