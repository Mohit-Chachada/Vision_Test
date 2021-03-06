#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <string.h>

using namespace cv;
using namespace std;

int POS_SAMPLES_NO = 2800;//2900;
int NEG_SAMPLES_NO = 3200;
const string POS_FOLDER_NAME = "pos";
const string NEG_FOLDER_NAME = "neg";
int SAMPLE_RECT_WIDTH = 20;
int SAMPLE_RECT_HEIGHT = 25;
int SAMPLE_RECT_X = 0;
int SAMPLE_RECT_Y = 0;
int OBJECTS_IN_SAMPLE = 1;

ofstream pos_info;
ofstream bg_txt;

void storeDescriptionFiles()
{
    // POSITIVE
    pos_info.open("./pos.info");
    
    for (int pos_no = 0; pos_no < POS_SAMPLES_NO; pos_no++)
    {
            //pos_info<<POS_FOLDER_NAME<<"/"<<pos_no<<".png"<<" "<<OBJECTS_IN_SAMPLE<<" "<<SAMPLE_RECT_X<<" "<SAMPLE_RECT_Y<<" "<<SAMPLE_RECT_WIDTH<<" "<<SAMPLE_RECT_HEIGHT<<"\n";
            pos_info<<"pos/"<<pos_no<<".png"<<" 1 0 0 20 25\n";
    }
    pos_info.close();
    
    // NEGATIVE
    bg_txt.open("bg.txt");
    for (int neg_no = 0; neg_no < NEG_SAMPLES_NO; neg_no++)
    {
            bg_txt<<NEG_FOLDER_NAME<<"/"<<neg_no<<".jpg"<<"\n";
    }
    bg_txt.close();
}





int main (int argc, char** argv) {

    storeDescriptionFiles();

    char command[1000];
    sprintf(command,"opencv_createsamples -vec buoy.vec -info pos.info -w 20 -h 25 -num 2800");  // -show
    system(command);
    cout<<"vec file created\n";
    sprintf(command,"opencv_traincascade -data data -vec buoy.vec -bg bg.txt -numPos 2700 -numNeg 3000 -numStages 10 -featureType LBP -w 20 -h 25 -bt GAB");
    system(command);
    cout<<"Training complete\n";
return 0;
 }

