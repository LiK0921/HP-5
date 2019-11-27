#!/bin/bash
echo "Please input a filename"
read -p "file name : " filename
test -e $filename && echo "$filename is exist" || echo "$filename is not exist"

read -p "first string : " str1
read -p "second string : " str2
test  $str1 == $str2 && echo "$str1 == $str2" || echo "$str1 != $str2"
