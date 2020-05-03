#!/bin/bash
echo "paste stack locations"
while true;
do
    read -p "" stack
    arm-none-eabi-addr2line --demangle --inlines -faps -e bin/monolith.elf $stack
done

