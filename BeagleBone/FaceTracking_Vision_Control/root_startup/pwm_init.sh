#!/bin/bash
# PWM init & permission set
modprobe pwm_test
echo am33xx_pwm > /sys/devices/bone_capemgr.7/slots
echo bone_pwm_P9_14 > /sys/devices/bone_capemgr.7/slots 
chmod a+rw /sys/devices/ocp.2/pwm_test_P9_14.9/duty
chmod a+rw /sys/devices/ocp.2/pwm_test_P9_14.9/period
chmod a+rw /sys/devices/ocp.2/pwm_test_P9_14.9/polarity
echo 0 > /sys/devices/ocp.2/pwm_test_P9_14.9/polarity
echo 500 > /sys/devices/ocp.2/pwm_test_P9_14.9/period
echo 0 > /sys/devices/ocp.2/pwm_test_P9_14.9/duty 
echo "PWM P9_14 pin SETUP DONE!"
