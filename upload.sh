#! /usr/bin/bash

n=1
last=100
while [ $n -lt $last ]
do
 cat > upload$n.txt <<<  "Newly created file name is : upload$n.txt"
 n=$(($n+1))
done
