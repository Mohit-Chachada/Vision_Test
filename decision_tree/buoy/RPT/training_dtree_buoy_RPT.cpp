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
char pathToImages[] = "../images";
int class_labels[nr_classes]	= {0,1};
const int feature_size = 20;

vector<float> RPTFeature(Mat img)
{
	//Params
	int N = 20;											// size of feature vector
	
	float PI = 3.14;	
	Mat I_gray = Mat(Size(img.cols,img.rows),CV_8UC1);
	cvtColor(img,I_gray,CV_BGR2GRAY);
	int width = img.cols;
	int height = img.rows;
	Point center(width/2,height/2);
	float max_r = min(width/2,height/2);		//max radius
	vector<float> radius;							// all possible radii
	radius.resize(N);
	vector<int> NrPoints;							//no of points on each circle
	NrPoints.resize(N);
	vector<vector<Point> > pt;						//point locations on each circle
	pt.resize(N);
	vector<float>I_sum,I_avg,I_norm;
	I_sum.resize(N);
	I_avg.resize(N);
	I_norm.resize(N);			

	//since avging over pixels on the circle at radius r, the no of points are taken=floor(2*pi*r) => del_theta=1/r
	// x=xc+r*cos(theta); theta=k/r; k=0,1,2,3...NrPoints-1

	for(int i=0;i<N;i++)
	{
		radius[i] = (i+1)*max_r/N;
		NrPoints[i] = floor(2*PI*radius[i]);
		pt[i].resize(NrPoints[i]);
		I_sum[i] = 0;
		I_avg[i] = 0;
		int Imax = 0;
		for(int j=0; j<NrPoints[i];j++)
		{
			pt[i][j].x = floor( center.x + radius[i]*cos(j/radius[i]) );
			pt[i][j].y = floor( center.y + radius[i]*sin(j/radius[i]) );			
//			cout<< pt[i][j]<<"\t"<<img.size()<<"\n";
			I_sum[i] += I_gray.at<uchar>(pt[i][j].x,pt[i][j].y);
			int temp = I_gray.at<uchar>(pt[i][j].x,pt[i][j].y);			
			if(temp>Imax) Imax=temp;
		}
		I_avg[i] = I_sum[i]/NrPoints[i];
		I_norm[i] = I_avg[i]/Imax;
	}
	
	// forming the feature vector
	vector<float> feature;
	int feature_size = N;
	feature.resize(feature_size);
	for(int i=0; i<feature_size; i++)	
	{
//		feature[i] = I_avg[i];
		feature[i] = I_norm[i];
		//To avoid variations at vary small and very large radii, dont consider those feature elements
//		if(radius[i]<0.30*max_r || radius[i]>0.90*max_r)
		if(i<0.2*N || i>0.75*N)
		{
			feature[i] = 0;
		}		
	}
	
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
         cout<<class_labels[cl]<<"\t"<<ll<<"\n";
         //calculate feature vector
			vector<float> feature = RPTFeature(img);
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
	waitKey(0);
	// test on sample image
	string filename = string(pathToImages)+"/test.png";
	Mat test_img = imread(filename.c_str());
	vector<float> test_feature = RPTFeature(test_img);
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
