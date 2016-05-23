#########################################################################
# File Name: check.sh
# Author: ma6174
# mail: ma6174@163.com
# Created Time: Wed 18 May 2016 03:09:13 PM CST
#########################################################################
#!/bin/bash

dir1=/home/hanbing1/files
dir2=/home/hanbing1/ksyun_ks3sdk_c/tests/press_tests

for i in `ls $dir1`
do
    echo $i===
    file1=$dir1/$i
    md5=`md5sum $file1 |awk '{print $1}'`
    file2=$dir2/${i}_save
    md5sum ${file2}* | grep -v $md5
    ls ${file2}* | wc -l
done
