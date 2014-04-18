#!/bin/bash
# Set PWM DUTY CYCLE
node -e "require('bonescript').analogWrite('P9_14',0.5,500,console.log)"
