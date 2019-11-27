#!/bin/bash

for name in lik lik1 lik2 lilk3
do
    echo "Your name is : $name"
done


while [ "$value" != "close" ]
do
    read -p "Please input str : " value
done

echo "Stop~"

for((i = 0; i < 10; i++)) 
do
    echo "i : $i"
done
