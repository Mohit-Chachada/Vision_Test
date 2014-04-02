#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml/ml.hpp"
#include <iostream>
#include <stdio.h>
#include <fstream>

using namespace cv;
using namespace std;

const int nr_classes = 2;  // object & non-object
const int train_samples[nr_classes] = {450,450};	//no of samples for each class
char pathToImages[] = "./images/buoy";
int class_labels[nr_classes]	= {0,1};
const int feature_size = 75;

vector<float> ColorHistFeature(Mat window_img)
{	
  Mat img = window_img.clone();
  /// Separate the image in 3 channels
  vector<Mat> bgr_planes;
  cvtColor(img,img,CV_BGR2HSV);
  // HSV frames
  split( img, bgr_planes );

  /// Establish the number of bins for each channel
  int histSize = 25;

  /// Set the ranges for each channel
  float range[] = { 0, 256 } ;
  const float* histRange = { range };

  bool uniform = true; bool accumulate = false;

  Mat b_hist, g_hist, r_hist;

  /// Compute the histograms:
  calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

  int hist_w = 512; int hist_h = 400;
  int bin_w = cvRound( (double) hist_w/histSize );
  Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

  /// Normalize the result to [ 0, histImage.rows ]
  normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
  normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
  normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
	
	//combine each channels hists in a single feature vector
	int nrcombine=3;
	vector<float> feature; 	//feature vector
	feature.resize(histSize*nrcombine);
	for (int i=0; i<feature.size(); i++){
		if(i<histSize)  feature[i] = b_hist.at<float>(i,0);
		else if(i>=histSize && i<histSize*2)  feature[i] = g_hist.at<float>(i-histSize,0);
		else if(i>=histSize*2 && i<histSize*3)  feature[i] = r_hist.at<float>(i-2*histSize,0);                    
	}

/*
  // Draw the histograms

  for( int i = 1; i < histSize; i++ )
  {
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                       Scalar( 255, 0, 0), 2, 8, 0  );
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                       Scalar( 0, 255, 0), 2, 8, 0  );
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                       Scalar( 0, 0, 255), 2, 8, 0  );
  }

  /// Display
  imshow("test img histogram", histImage );
  waitKey(1);
*/  
	return feature;
}


int main( int argc, char** argv )
{
	Mat img;
   char file[255];
	
	//total no of training samples
	int total_train_samples = 0;
	for(int cl=0; cl<nr_classes; cl++)
	{
		total_train_samples = total_train_samples + train_samples[cl];
	}
	
	// Training Data
	Mat training_data = Mat(total_train_samples,feature_size,CV_32FC1);
	Mat training_label = Mat(total_train_samples,1,CV_32FC1);
	// training data .csv file
	ofstream trainingDataCSV;
	trainingDataCSV.open("./training_data.csv");	
		
	int index = 0;	
	for(int cl=0; cl<nr_classes; cl++)
	{
      for(int ll=0; ll<train_samples[cl]; ll++)
      {
      	//assign sample label
			training_label.at<float>(index+ll,0) = class_labels[cl]; 	
			//image feature extraction
 			sprintf(file, "%s/%d/%d.png", pathToImages, class_labels[cl], ll);
         img = imread(file, 1);
         if (!img.data)
         {
             cout << "File " << file << " not found\n";
             exit(1);
         }
         imshow("sample",img);
         waitKey(1);
         //calculate feature vector
			vector<float> feature = ColorHistFeature(img);
			for(int ft=0; ft<feature.size(); ft++)
			{
				training_data.at<float>(index+ll,ft) = feature[ft];
				trainingDataCSV<<feature[ft]<<",";
			}
			trainingDataCSV<<class_labels[cl]<<"\n";
		}
		index = index + train_samples[cl];
	}	
	
	trainingDataCSV.close();

	/// Decision Tree
	// Training
	float *priors = NULL;
	CvDTreeParams DTParams = CvDTreeParams(25, // max depth
		                                    5, // min sample count
		                                    0, // regression accuracy: N/A here
		                                    false, // compute surrogate split, no missing data
		                                    15, // max number of categories (use sub-optimal algorithm for larger numbers)
		                                    15, // the number of cross-validation folds
		                                    false, // use 1SE rule => smaller tree
		                                    false, // throw away the pruned tree branches
		                                    priors // the array of priors
		                                   );
	CvDTree DTree;
	DTree.train(training_data,CV_ROW_SAMPLE,training_label,Mat(),Mat(),Mat(),Mat(),DTParams);
			
	// save model
	DTree.save("training.model");		
	
	// load model
	CvDTree DT;
	DT.load("training.model");	
	
	// test on sample image
	string filename = string(pathToImages)+"/test.png";
	Mat test_img = imread(filename.c_str());
	vector<float> test_feature = ColorHistFeature(test_img);
	CvDTreeNode* result_node = DT.predict(Mat(test_feature),Mat(),false);
	double predictedClass = result_node->value;
	cout<<"predictedClass "<<predictedClass<<"\n";

/*	
	//CvMLData for calculating error
	CvMLData* MLData;
	MLData = new CvMLData();
	MLData->read_csv("training_data.csv");
	MLData->set_response_idx(feature_size);
//	MLData->change_var_type(feature_size,CV_VAR_CATEGORICAL);
	
	// calculate training error
	float error = DT.calc_error(MLData,CV_TRAIN_ERROR,0);
	cout<<"training error "<<error<<"\n";
*/
	return 0;
}
