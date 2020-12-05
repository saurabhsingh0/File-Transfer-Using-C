#! /usr/bin/bash

n=1
last=100
while [ $n -lt $last ]
do
 cat > down$n.txt <<<  "Newly created file name is : down$n.txt"
 n=$(($n+1))
done
