#!/bin/bash
#startup
# Set IP
ifconfig eth0 192.168.7.2 netmask 255.255.255.0
route add default gw 192.168.7.1
# camera permission
chmod a+rw /dev/video0
# gpio_init PIN 15 & 49
bash gpio_init.sh
# PWM init
bash pwm_init.sh
