#!/bin/bash
read -p "Please input(Y/N)" value

if [ "$value" == "Y" ] || [ "$value" == "y" ]; then
    echo "you input yes!"

elif [ "$value" == "N" ] || [ "$value" == "n" ]; then
    echo "you input no!"

fi


read -p "Please input(L/K)" value1
if [ "$value1" == "L" ] || [ "$value1" == "l" ]; then
    echo "you input QU TA DA YE!"
else
    echo "you input FUCK!"
    exit 0
fi
