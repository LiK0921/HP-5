#!/bin/bash
read -p "Please input your age and sex : " age sex
echo "Your age = $age, sex = $sex"

echo "Please input two number"
read -p "Please input first number : " a 
read -p "Please input second number : " b 
sum=$(($a+$b))
echo "$a + $b = $sum"
