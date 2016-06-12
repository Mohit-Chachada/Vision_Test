/***************************************************************************************************
File: 		   controller_sim.h
Author: 	   Mohit Chachada <chachadamohit@gmail.com>
Description:   Matsya AUV 6-DOF Simulator for testing Guidance, Navigation and Control Algorithms
Last modified: 7th July 2015
***************************************************************************************************/

#ifndef AUV_CONTROL_SIMULATION_H
#define AUV_CONTROL_SIMULATION_H

#include <ros/ros.h>
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/SetCameraInfo.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include "std_srvs/Empty.h"
#include <string.h>
#include "std_msgs/UInt16.h"
#include "auv_msgs/DvlPosition.h"
#include "auv_msgs/DvlVelocity.h"
#include "auv_msgs/Orientation.h"
#include "sensor_msgs/Imu.h"
#include "auv_msgs/Pose.h"
#include "firmware/beaglebone/controller/controller.h"
#include "firmware/beaglebone/definations.h"

#define VEHICLE_TEST false
#if(VEHICLE_TEST==true)
	#define PUBLISH_DVL false
	#define PUBLISH_IMU false
#else
	#define PUBLISH_DVL true
	#define PUBLISH_IMU true
#endif

using namespace std;

class ControllerSim 
{
  private:

	ros::Subscriber _subPressureSensor;
	ros::Subscriber _subDvlPosition;
	ros::Subscriber _subDvlVelocity;	
	ros::Subscriber _subImuRpy;
	ros::Subscriber _subImuRpyRates;	
	ros::Subscriber _subSetPoint;

    ros::NodeHandle _nh;
    
	ros::ServiceServer _srvResetControl;    
	ros::ServiceServer _srvUpdateControlParam;
	
	ros::Publisher _pubDvlPosition;
	ros::Publisher _pubDvlVelocity;
	ros::Publisher _pubImuVelocity;
	ros::Publisher _pubImuRpy;
    
	//Structs for SensorData, Setpoint, PWM, Control Parameters
	struct SensorData sensorData;
	struct PressureData pressureData;
	struct SetPoint setPoint;
	struct PwmData pwmData; 
	struct ControlParam controlParam;    
	
	//callBack subscriptions for sensor data
	void _callBackPressureSensor(std_msgs::UInt16 Msg);
	void _callBackDvlPosition(auv_msgs::DvlPosition Msg);
	void _callBackDvlVelocity(auv_msgs::DvlVelocity Msg);	
	void _callBackImuRpy(auv_msgs::Orientation Msg);	
	void _callBackImuRpyRates(sensor_msgs::Imu Msg);
	
	//callBack subscription for SetPoint
	void _callBackSetPoint(auv_msgs::Pose Msg);		
	
	//service to start controller simulation
	bool _callBackResetControl(std_srvs::Empty::Request &req, std_srvs::Empty::Response &res);
	
	//service to update service controller simulation
	bool _callBackUpdateControlParam(std_srvs::Empty::Request &req, std_srvs::Empty::Response &res);
	
	//End Simulation Flag
	bool _endSimulation;
	
	//update ros params
	void updateROSParams();
	
	//Controller
	Controller Ctrl;
	
	//AUV Model
	AUVModel Matsya;

	//Wframe Previous Position
	Vector6d WPos_prev;
	
public:
    ControllerSim();
    void loop();
    
    //End Simulation
    void endSimulation(bool );
    
    //Start Simulation
    void start();
};



#endif //AUV_CONTROL_SIMULATION