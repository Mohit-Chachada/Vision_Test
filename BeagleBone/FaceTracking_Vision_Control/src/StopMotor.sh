#!/bin/bash
# making both motor direction pins same to stop motor instantly
cd /sys/class/gpio/gpio49
echo 1 > value
cd /sys/class/gpio/gpio15
echo 1 > value

