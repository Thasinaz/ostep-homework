#!/usr/bin/env bash

echo '# x y'

for ((i = 1; i <= 8192; i *= 2)); do
    echo $i $(./tlb $i 8192)
done
