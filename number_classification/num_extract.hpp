#ifndef NUM_EXTRACT_HPP
#define NUM_EXTRACT_HPP
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/ml/ml.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "lib/svm/svm.h"
#include <fstream>
#include <vector>
#include "lib/svm/svm-scale.h"
#include <ros/package.h>

using namespace cv;
using namespace std;

class Num_Extract
{
private:


    int _classifier;    // use 1 SVM
    int _train_samples;
    int _classes;
    int sizex ;
    int sizey ;
    int ImageSize ;
    int HOG3_size;
    char _pathToImages[200];
    bool _temp_match;
    int _print_nos[4];

    int Nframes;  // continuos N frame counts for correct detection
    vector< vector<vector<int> > > Nmaxcount1;
    int cindex;
    string _ros_path;

protected:

    bool A_encloses_B(RotatedRect A, RotatedRect B);
    bool validate (Mat mask, Mat pre);
    vector<Mat> extract_Number(vector<Mat> mask,Mat pre);
    vector<Mat> rotate_bins(vector<Mat>,Mat);
    vector<Mat> extract(Mat mask, Mat pre);
    int mode(vector<int> list, int* maxcount);

    void LearnFromImages(CvMat* trainData, CvMat* trainClasses);
    void RunSelfTest(KNearest& knn2, CvSVM& SVM2);
    vector<int> Classification(KNearest knearest, CvSVM SVM, Mat _image);

    float maximum(float x, float y, float z);

    void HOG3(IplImage *Im,vector<float>& descriptors);
    vector<Mat> HOGMatching_Template();

    vector<int> HOGMatching_Compare(vector<Mat> hist, Mat test_img);
    svm_model* loadModel (const char* modelName);
    int PredictNumber(svm_model* model, Mat _image);
    float _maxProbability;
    int _finalNoCount;

public:

    struct InParams{
        int classifier;    // use 1 SVM
        int train_samples;
        int classes;
        char pathToImages[200];
        bool temp_match;
        int print_nos[4];
        string ros_path;
    };

    class SubTaskReturn{
    public:
        int _no_of_bins;
        vector<Point> _bin_centers;
        double _orientation;
        int _area_of_bins;
        //vector<int> _detected_nos;
        //vector<int> _mode_counts;
        bool _NumberDetectionComplete;
        float _confidence;
        int _number;
        SubTaskReturn() {
        _NumberDetectionComplete = false;
        _confidence = 0;
        _number = 0;
        }

        //~SubTaskReturn();
    };

    Num_Extract(Num_Extract::InParams params);
    Num_Extract();
    ~Num_Extract();
    double pi;
    bool is_valid;
    bool numbers_found;
    Num_Extract::SubTaskReturn run (Mat mask,Mat pre);
    int img_no;
    double _orientation;

};

#endif
