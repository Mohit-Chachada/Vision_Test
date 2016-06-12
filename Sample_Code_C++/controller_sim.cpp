/***************************************************************************************************
File: 		   controller_sim.cpp
Author: 	   Mohit Chachada <chachadamohit@gmail.com>
Description:   Matsya AUV 6-DOF Simulator for testing Guidance, Navigation and Control Algorithms
Last modified: 7th July 2015
***************************************************************************************************/

#include "controller_sim.h"

ControllerSim::ControllerSim()
{
    //Set SimulationMode for Controller
    Ctrl.setSimulationMode(true);

    //SensorData Subscribers
    _subPressureSensor = _nh.subscribe("/hw_comm/depth",1,&ControllerSim::_callBackPressureSensor,this);
    _subDvlPosition = _nh.subscribe("/dvl/position",1,&ControllerSim::_callBackDvlPosition,this);
    _subDvlVelocity = _nh.subscribe("/dvl/velocity",1,&ControllerSim::_callBackDvlVelocity,this);
    _subImuRpy = _nh.subscribe("/imu/data_rpy",1,&ControllerSim::_callBackImuRpy,this);
    _subImuRpyRates = _nh.subscribe("/imu/data",1,&ControllerSim::_callBackImuRpyRates,this);

    //Dvl & Imu Publisher for simulation
    _pubDvlPosition = _nh.advertise<auv_msgs::DvlPosition>("/dvl/position",1);
    _pubDvlVelocity = _nh.advertise<auv_msgs::DvlVelocity>("/dvl/velocity",1);
    _pubImuVelocity = _nh.advertise<sensor_msgs::Imu>("/imu/data",1);
    _pubImuRpy = _nh.advertise<auv_msgs::Orientation>("/imu/data_rpy",1);

    //SetPoint Subscriber
    _subSetPoint = _nh.subscribe("/hw_comm/beagle/set_point",1,&ControllerSim::_callBackSetPoint,this);

    //Creating and advertizing service to start simulation
    _srvResetControl = _nh.advertiseService("/controller_sim/reset_control", &ControllerSim::_callBackResetControl,this);

    //Update Control Params service call
    _srvUpdateControlParam = _nh.advertiseService("/hw_comm/beagle/set_control_param", &ControllerSim::_callBackUpdateControlParam, this);

    /*
    //Update Control Params service call
    srvControlValidity = _nh.advertiseService("/hw_comm/beagle/control_validity",
                                            &SetControlParam::validityCallback,
                                            this);
    */
    controlParam.controlValidity = VALID_YAW_CONTROL | VALID_PITCH_CONTROL | VALID_DEPTH_CONTROL | VALID_SURGE_CONTROL | VALID_SWAY_CONTROL;

    //initialize _endSimulation flag
    _endSimulation = false;

    //Initialize W-frame Prev Position at t=0
    WPos_prev << 0,0,0,0,0,0;
}

void ControllerSim::updateROSParams()
{
    ROS_INFO("SETTING THE CONTROL PARAM \n");
    _nh.getParam("kp_yaw",controlParam.kpYaw);
    _nh.getParam("kd_yaw",controlParam.kdYaw);
    _nh.getParam("ki_yaw",controlParam.kiYaw);
    _nh.getParam("kp_pitch",controlParam.kpPitch);
    _nh.getParam("kd_pitch",controlParam.kdPitch);
    _nh.getParam("ki_pitch",controlParam.kiPitch);
    _nh.getParam("kp_depth",controlParam.kpDepth);
    _nh.getParam("kd_depth",controlParam.kdDepth);
    _nh.getParam("ki_depth",controlParam.kiDepth);
    _nh.getParam("c_surge",controlParam.cSurge);
    _nh.getParam("kp_surge",controlParam.kpSurge);
    _nh.getParam("ki_surge",controlParam.kiSurge);
    _nh.getParam("kd_surge",controlParam.kdSurge);
    _nh.getParam("c_sway",controlParam.cSway);
    _nh.getParam("kp_sway",controlParam.kpSway);
    _nh.getParam("ki_sway",controlParam.kiSway);
    _nh.getParam("kd_sway",controlParam.kdSway);
    _nh.getParam("pConstant",controlParam.pConstant);
    _nh.getParam("pFactor",controlParam.pFactor);
    _nh.getParam("kp_x",controlParam.kpX);
    _nh.getParam("kp_y",controlParam.kpY);
    _nh.getParam("kp_z",controlParam.kpZ);
    _nh.getParam("kp_phi",controlParam.kpPhi);
    _nh.getParam("kp_theta",controlParam.kpTheta);
    _nh.getParam("kp_psi",controlParam.kpPsi);
    _nh.getParam("direct_mode",controlParam.directMode);
}

bool ControllerSim::_callBackUpdateControlParam(std_srvs::Empty::Request &request, std_srvs::Empty::Response &response)
{
    updateROSParams();
    return true;
}

void ControllerSim::_callBackPressureSensor(std_msgs::UInt16 Msg)
{
    pressureData.height = Msg.data;
}

void ControllerSim::_callBackDvlPosition(auv_msgs::DvlPosition Msg)
{
    sensorData.dvlData.positionX = Msg.x;
    sensorData.dvlData.positionY = Msg.y;
    sensorData.dvlData.positionZ = Msg.z;
}

void ControllerSim::_callBackDvlVelocity(auv_msgs::DvlVelocity Msg)
{
    sensorData.dvlData.velocityX = Msg.vx;
    sensorData.dvlData.velocityY = Msg.vy;
    sensorData.dvlData.velocityZ = Msg.vz;
    sensorData.dvlData.velocitySurge = Msg.velSurge;
    sensorData.dvlData.velocitySway = Msg.velSway;
    sensorData.dvlData.velocityHeave = Msg.velHeave;
}

void ControllerSim::_callBackImuRpy(auv_msgs::Orientation Msg)
{
    sensorData.imuData.roll = Msg.roll;
    sensorData.imuData.pitch = Msg.pitch;
    sensorData.imuData.yaw  = Msg.yaw;
}

void ControllerSim::_callBackImuRpyRates(sensor_msgs::Imu Msg)
{
    sensorData.imuData.rollRate = Msg.angular_velocity.x;
    sensorData.imuData.pitchRate = Msg.angular_velocity.y;
    sensorData.imuData.yawRate  = Msg.angular_velocity.z;
}

void ControllerSim::_callBackSetPoint(auv_msgs::Pose data)
{
    setPoint.posX = data.position.x;
    setPoint.posY = data.position.y;
    setPoint.posZ = data.position.z;
    setPoint.velX = data.velocity.x;
    setPoint.velY = data.velocity.y;
    setPoint.velZ = data.velocity.z;
    setPoint.roll = data.orientation.roll;
    setPoint.pitch = data.orientation.pitch;
    setPoint.yaw = data.orientation.yaw;
    setPoint.rollRate = data.angular_velocity.roll;
    setPoint.pitchRate = data.angular_velocity.pitch;
    setPoint.yawRate = data.angular_velocity.yaw;
    setPoint.surgeRate = data.velocity_Bframe.x;
    setPoint.swayRate = data.velocity_Bframe.y;
    setPoint.heaveRate = data.velocity_Bframe.z;
    setPoint.surge = data.relative_position_Bframe.x;
    setPoint.sway = data.relative_position_Bframe.y;
    setPoint.heave = data.relative_position_Bframe.z;
}

void ControllerSim::endSimulation(bool flag)
{
    _endSimulation = flag;
}

void ControllerSim::start()
{
    printf("Start Function Call..\n");
    system("rosparam load ~/catkin_ws/src/robosub/auv_hw_comm/beagle_param.yaml");
    updateROSParams();
}

bool ControllerSim::_callBackResetControl(std_srvs::Empty::Request &req, std_srvs::Empty::Response &res)
{
    Ctrl.controllerErrorReset();
    return true;
}

void ControllerSim::loop()
{
	ROS_INFO("Inside Simulation Loop..");
    ros::spinOnce();
    	
	printf("SP SUv %f,SWv %f,Yv %f\n",setPoint.surgeRate,setPoint.swayRate,setPoint.yawRate);
	printf("SD SUv %f,SWv %f,Yv %f\n",sensorData.dvlData.velocitySurge,sensorData.dvlData.velocitySway,sensorData.imuData.yawRate);
	
	//Update SensorData & SetPoint
	Ctrl.updatePoint(sensorData,pressureData,setPoint);

	if(controlParam.kpPsi == 0 || controlParam.kpSurge == 0) ROS_WARN("Control Params NOT Loaded Successfully");
	//Update Controller Parameters
	Ctrl.updateParam(controlParam);
	
	//Call main Controller
	Ctrl.motionController(1);		
			
	//Get PWM Values from controller
	pwmData = Ctrl.getPWMData();
	
	//Get Force-Torque Vector
	vector<double> vecFT = Ctrl.getForceTorqueVector();
	
	//Simulate on System Model
	//Obtain B-frame response unsing systems difference equation
	vector<double> bVel_sim = Matsya.response(vecFT);
	Vector6d BVel;
	for(int i=0; i<NrOP; i++)
	{
		BVel[i] = bVel_sim[i];
	}	
	printf("BVel_sim: SU %.3f,SW %.3f,H %.3f,R %.3f,P %.3f,Y %.3f\n", BVel[AUVModel::B_DOF::SU], BVel[AUVModel::B_DOF::SW], BVel[AUVModel::B_DOF::H], BVel[AUVModel::B_DOF::R], BVel[AUVModel::B_DOF::P], BVel[AUVModel::B_DOF::Y]);

	//Calculate 3D-TF Matrices for Forward & Inverse Kinematics (B-frame Velocity <-> W-frame Velocity)
	double phi = WPos_prev[AUVModel::W_DOF::PH];
	double theta = WPos_prev[AUVModel::W_DOF::TH];
	double psi = WPos_prev[AUVModel::W_DOF::PS];
	Matrix6d JK = Matsya.BodyToWorldFrameTransformMatrix(phi, theta, psi);
	
	//Convert B-frame Velocity to W-frame Velocity
	Vector6d WVel = JK*BVel;
	
    //Obtain W-frame Position outputs by integrating the W-frame Velocity outputs
    Vector6d WPos = WPos_prev + WVel*T_SAMP;
	WPos[AUVModel::W_DOF::PH] = Matsya.normalizeAngle(WPos[AUVModel::W_DOF::PH]);
	WPos[AUVModel::W_DOF::TH] = Matsya.normalizeAngle(WPos[AUVModel::W_DOF::TH]);
	WPos[AUVModel::W_DOF::PS] = Matsya.normalizeAngle(WPos[AUVModel::W_DOF::PS]);		    
	WPos_prev = WPos;
	printf("WPos_sim: X %.3f,Y %.3f,Z %.3f,PH %.3f, TH %.3f, PS %.3f\n", WPos[AUVModel::W_DOF::X], WPos[AUVModel::W_DOF::Y], WPos[AUVModel::W_DOF::Z], WPos[AUVModel::W_DOF::PH], WPos[AUVModel::W_DOF::TH], WPos[AUVModel::W_DOF::PS]);
	

	//Publish bVel on dvlData.velocity Surge, Sway and Heave
	//(multiplied by 100 for conversion from m/s to cm/s)
	auv_msgs::DvlVelocity msgDvlVel;
	msgDvlVel.velSurge = BVel[AUVModel::B_DOF::SU]*100;
	msgDvlVel.velSway = BVel[AUVModel::B_DOF::SW]*100;
	msgDvlVel.velHeave = BVel[AUVModel::B_DOF::H]*100;		
	//Publish wVel on dvlData.velocity X, Y and Z
	//(multiplied by 100 for conversion from m/s to cm/s)
	msgDvlVel.vx = WVel[AUVModel::W_DOF::X]*100;
	msgDvlVel.vy = WVel[AUVModel::W_DOF::Y]*100;
	msgDvlVel.vz = WVel[AUVModel::W_DOF::Z]*100;	
	
	//Publish wPos on dvlData.position X, Y & Z
	//(multiplied by 100 for conversion from m to cm)
	auv_msgs::DvlPosition msgDvlPos;
	msgDvlPos.x = WPos[AUVModel::W_DOF::X]*100;
	msgDvlPos.y = WPos[AUVModel::W_DOF::Y]*100;
	msgDvlPos.z = WPos[AUVModel::W_DOF::Z]*100;	
		
	//Publish wVel on imuData.velocity Roll, Pitch and Yaw
	//(multiplied by 180/M_PI for conversion from rad/s to deg/s)
	//((-1) for sign convention adjustment NOT required since roll and pitch vel are NOT multiplied by (-1) in IMU)
	sensor_msgs::Imu msgImuVel;
	msgImuVel.angular_velocity.x = (1)*WVel[AUVModel::W_DOF::PH]*(180/M_PI);
	msgImuVel.angular_velocity.y = WVel[AUVModel::W_DOF::TH]*(180/M_PI);
	msgImuVel.angular_velocity.z = (1)*WVel[AUVModel::W_DOF::PS]*(180/M_PI);		
	
	//Publish wPos on imuData.position Roll, Pitch and Yaw
	//(multiplied by 180/M_PI for conversion from rad to deg)
	//(multiplied by (-1) for sign convention adjustment)
	auv_msgs::Orientation msgImuPos;
	msgImuPos.roll = (-1)*WPos[AUVModel::W_DOF::PH]*(180/M_PI);
	msgImuPos.pitch = WPos[AUVModel::W_DOF::TH]*(180/M_PI);
	msgImuPos.yaw = (-1)*WPos[AUVModel::W_DOF::PS]*(180/M_PI);		

	if(PUBLISH_DVL == true)	_pubDvlPosition.publish(msgDvlPos);	
	if(PUBLISH_DVL == true)	_pubDvlVelocity.publish(msgDvlVel);
	if(PUBLISH_IMU == true)	_pubImuRpy.publish(msgImuPos);	
	if(PUBLISH_IMU == true)	_pubImuVelocity.publish(msgImuVel);	
	printf("\n");
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "controller_sim");
    ControllerSim v;
    ROS_INFO("\n[CONTROLLER_SIMULATION] Started");
    // some function call required at start
    v.start();

    // loop function at rate
    ros::Rate loop_rate(CONTROLLER_FREQ);
    while(ros::ok())
    {
        v.loop();
        loop_rate.sleep();
    }
    return 0;
}