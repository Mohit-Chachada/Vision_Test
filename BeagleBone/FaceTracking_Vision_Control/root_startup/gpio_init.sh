#!/bin/bash
# gpio_init()
cd /sys/class/gpio
chmod a+w /sys/class/gpio/export
echo 49 > export
cd /sys/class/gpio/gpio49
chmod a+w /sys/class/gpio/gpio49/direction
chmod a+w /sys/class/gpio/gpio49/value
echo out > direction
echo 1 > value
cd /sys/class/gpio
echo 15 > export
cd /sys/class/gpio/gpio15
chmod a+w /sys/class/gpio/gpio15/direction
chmod a+w /sys/class/gpio/gpio15/value
echo out > direction
echo 0 > value
echo "GPIO 15 & 49 SETUP DONE!"

