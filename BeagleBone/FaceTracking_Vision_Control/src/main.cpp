// COMPILATION:
// g++ `pkg-config opencv --cflags --libs` -lrt main.cpp

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <time.h>
#include <iostream>
#include <stdio.h>
#include "unistd.h"
using namespace std;
using namespace cv;

/** Global variables */
String face_cascade_name = "haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);
VideoWriter outputVideo;
float prev_DTY=0;
float SumError=0;
float max_SumError=0;
float prev_error=0;
struct timespec prev_time;
bool controller_init=true;

#define PWM_DTY_NULL 0
#define Kp 1
#define Kd 0.2
#define Ki 0.2
#define PWM_TIME_PERIOD 500
#define display 1


float Controller(float error,float max_error)
{	
	// PID Controller
	float kp = Kp;
	float kd = Kd;
	float ki = Ki;
	
	float DError,max_DError,control_op,max_control_op,PWM_DTY;
	
	struct timespec now_time;
	clock_gettime(CLOCK_REALTIME, &now_time);
	double delta_time;
	if(controller_init)
	{
		SumError = error;
		DError = error;
		max_SumError = max_error;
		max_DError = max_error;
		controller_init = false;
	}
	else
	{
		delta_time = (now_time.tv_sec-prev_time.tv_sec)+(double)(now_time.tv_nsec-prev_time.tv_nsec)/1000000000.0d;
		SumError = SumError+error*delta_time;
	 	DError = (error-prev_error)/delta_time;
		max_SumError = max_SumError+max_error*delta_time;
		max_DError = (max_error-prev_error)/delta_time;	
	}
	
	control_op = kp*error+kd*DError+ki*SumError;	
	max_control_op = kp*max_error+kd*max_DError+ki*max_SumError;
	PWM_DTY = abs(control_op/max_control_op);
	prev_DTY = PWM_DTY;
	prev_error = error;
	prev_time = now_time;	
	return PWM_DTY;
}

void SetPWMDTY(float PWM_DTY)
{	
	float PWMTimePeriod = PWM_TIME_PERIOD;
	char command[1000];
   int PWMONTime = ceil(PWMTimePeriod*PWM_DTY);
	sprintf(command,"echo %f > /sys/devices/ocp.2/pwm_test_P9_14.9/period",PWMTimePeriod);
	sprintf(command,"echo %d > /sys/devices/ocp.2/pwm_test_P9_14.9/duty",PWMONTime);

	if(prev_DTY==0 && PWM_DTY==0)
	{
	}
	else
	{
		cout<<command<<"\n";	
		system(command);
	}
}

void SetMotorDirection(int DIRECTION)
{
	char command[1000];
	if(DIRECTION == 1)
	sprintf(command,"bash gpio_15_1_49_0.sh");
	else if (DIRECTION == -1)
	sprintf(command,"bash gpio_15_0_49_1.sh");

	system(command);
}

void StopMotor()
{
	char command[1000];
	sprintf(command,"bash StopMotor.sh");
	system(command);
}


Point detectAndDisplay( Mat frame )
{
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	struct timespec start,end;
	clock_gettime(CLOCK_REALTIME, &start);
	//-- Detect faces
	face_cascade.detectMultiScale( frame_gray, faces, 2, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
//	face_cascade.detectMultiScale( frame_gray, faces, 2.5, 2, 0, Size(30, 30),Size(150,150));

	clock_gettime(CLOCK_REALTIME, &end);
	double time_diff = (end.tv_sec-start.tv_sec)+(double)(end.tv_nsec-start.tv_nsec)/1000000000.0d;
	cout<<"FACE DETECTION TIME: "<<time_diff<<" seconds"<<endl;

	int max_size=0;
	int max_index=0;
	Point largest_face(-1,-1);
	if(faces.size()>0) cout<<"faces: ";
	for( size_t i = 0; i < faces.size(); i++ )
	{
		Point center;
	  if(display)
	  {
		center = Point( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
		rectangle(frame,faces[i],Scalar(255,0,0));
		ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
	  }
		cout<<center<<"\t";
/*
		Mat faceROI = frame_gray( faces[i] );
		std::vector<Rect> eyes;

		//-- In each face, detect eyes
		eyes_cascade.detectMultiScale( faceROI, eyes, 2, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );

		for( size_t j = 0; j < eyes.size(); j++ )
		{
			Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
			int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
			circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
		}
*/
		int face_size = faces[i].width*faces[i].height;
		if(face_size > max_size)
		{
			max_size = face_size;
			max_index = i;
			largest_face.x = center.x;
			largest_face.y = center.y;
		}
	}
	cout<<"\n";
	//-- Show what you got
	imshow( window_name, frame );
//	outputVideo.write(frame);

	// output the largest face
	return largest_face;
}


/** @function main */
int main( int argc, const char** argv )
{
	clock_t timer = clock();
	VideoCapture capture(-1);
	int width = 320;
	int height = 240;
	capture.set(CV_CAP_PROP_FRAME_WIDTH,width);	
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,height);	

	if(!capture.isOpened())
	{
		cout<<"Failed to connect to camera\n";
	}
	Mat frame;
	Point center(width/2,height/2);
	Mat rot_mat = getRotationMatrix2D(center,-90,1);

	//-- 1. Load the cascades
	if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
	if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };

	//-- 2. Read the video stream
	Size S = Size(640,480);
	const string NAME="1.avi";
	//	outputVideo.open(NAME,CV_FOURCC('M','J','P','G'),20, S, true); // 20 FPS 

	if( capture.isOpened() )
	{
		while( true )
		{
			Point target_center;

			struct timespec start,end;
			clock_gettime(CLOCK_REALTIME, &start);

			Mat temp;
			capture>>frame;			
			// since the camera is mounted vertically rotate the image by 90deg
			warpAffine(frame,frame,rot_mat,temp.size());
			
			//-- 3. Apply the classifier to the frame
			if( !frame.empty() )
			{ 
				target_center = detectAndDisplay(frame);		
			}
			else
			{ 	
				printf(" --(!) No captured frame -- Break!"); 
				break; 
			}

			clock_gettime(CLOCK_REALTIME, &end);
			double time_diff = (end.tv_sec-start.tv_sec)+(double)(end.tv_nsec-start.tv_nsec)/1000000000.0d;
			cout<<"TIME: "<<time_diff<<" seconds"<<endl;
			cout<<"FPS: "<<1/time_diff<<endl;
			int c = waitKey(1);
			if( (char)c == 'c' ) { break; }

			// Event Handler
			if(target_center.x == -1)
			{
				cout<<"No Face Found\n";
				// set PWM directly to NULL
				SetPWMDTY(PWM_DTY_NULL);
			}
			else 
			{
				cout<<"Face Detected!\n";
				// calculate error
				// since the camera is vertical, max_error=height/2 otherwise it shud be width/2
				float error =  height/2-target_center.x;
				float max_error = height/2;

				// Set Direction depending on sign of error	
				int DIRECTION;
				if(error > 0) DIRECTION = 1;
				else DIRECTION = -1;
				SetMotorDirection(DIRECTION);

				// Controller
				float PWM_DTY = Controller(error,max_error);

				cout<<"PWM_DTY "<<PWM_DTY<<"\t";
				cout<<"DIRECTION "<<DIRECTION<<"\n";
				// Set PWM
				SetPWMDTY(PWM_DTY);

				// coz a small PWM can force the motor to travel large angles due to inertia,
				// motor is forcefully stopped in some time after a PWM is given
				// usleep delay in msec
				unsigned long ms = 1;
				usleep(5*ms*1000);
				StopMotor();
//				SetPWMDTY(PWM_DTY_NULL);
				usleep(1*ms*1000);
			}
		}
	}


	return 0;
}


