#include <iostream>
#include <cmath>
#include <time.h>
#include <stdio.h>
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

static void help()
{
    printf("Usage:\n ./a.out <image1> <image2>\n");
}

float df_dDx(float Dx, float Dy, float phi, float Z, float **A, float **B, int N)
{
    float sum=0;
    for(int i=0;i<N;i++)
    {sum=sum+2*(Dx-Z*(A[i][0]*cos(phi)-A[i][1]*sin(phi)-B[i][0]));
    }
    return sum;
}

float df_dDy(float Dx,float Dy, float phi, float Z, float **A, float **B, int N)
{
    float sum=0;
    for(int i=0;i<N;i++)
    {sum=sum+2*(Dy-Z*(A[i][0]*sin(phi)+A[i][1]*cos(phi)-B[i][1]));
    }
    return sum;
}

float df_dphi(float Dx,float Dy, float phi, float Z, float **A, float **B, int N)
{
    float sum=0;
    for(int i=0;i<N;i++)
    {sum=sum + 2*(Dx-Z*(A[i][0]*cos(phi)-A[i][1]*sin(phi)-B[i][0])) * ((-Z)*(-A[i][0]*sin(phi)-A[i][1]*cos(phi))) + 2*(Dy-Z*(A[i][0]*sin(phi)+A[i][1]*cos(phi)-B[i][1])) * ((-Z)*(A[i][0]*cos(phi)-A[i][1]*sin(phi)));
    }
    return sum;
}

float df_dZ(float Dx,float Dy, float phi, float Z, float **A, float **B, int N)
{
    float sum=0;
    for(int i=0;i<N;i++)
    {sum=sum + 2*(Dx-Z*(A[i][0]*cos(phi)-A[i][1]*sin(phi)-B[i][0])) * ((-1)*(A[i][0]*cos(phi)-A[i][1]*sin(phi)-B[i][0])) + 2*(Dy-Z*(A[i][0]*sin(phi)+A[i][1]*cos(phi)-B[i][1])) * ((-1)*(A[i][0]*sin(phi)+A[i][1]*cos(phi)-B[i][1]));
    }
    return sum;
}

void ransacTest(const std::vector<cv::DMatch> matches,const std::vector<cv::KeyPoint>&keypoints1,const std::vector<cv::KeyPoint>& keypoints2,std::vector<cv::DMatch>& goodMatches,double distance,double confidence)
{
    goodMatches.clear();
    // Convert keypoints into Point2f
    std::vector<cv::Point2f> points1, points2;
    for (std::vector<cv::DMatch>::const_iterator it= matches.begin();it!= matches.end(); ++it)
    {
        // Get the position of left keypoints
        float x= keypoints1[it->queryIdx].pt.x;
        float y= keypoints1[it->queryIdx].pt.y;
        points1.push_back(cv::Point2f(x,y));
        // Get the position of right keypoints
        x= keypoints2[it->trainIdx].pt.x;
        y= keypoints2[it->trainIdx].pt.y;
        points2.push_back(cv::Point2f(x,y));
    }
    
    // Compute F matrix using RANSAC
    std::vector<uchar> inliers(points1.size(),0);
    cv::Mat fundemental= cv::findFundamentalMat(cv::Mat(points1),cv::Mat(points2),inliers,FM_RANSAC,distance,confidence); // confidence probability
    // extract the surviving (inliers) matches
    std::vector<uchar>::const_iterator
            itIn= inliers.begin();
    std::vector<cv::DMatch>::const_iterator
            itM= matches.begin();
    // for all matches
    for ( ;itIn!= inliers.end(); ++itIn, ++itM)
    {
        if (*itIn)
        { // it is a valid match
            goodMatches.push_back(*itM);
        }
    }
}


int main(int argc, char** argv)
{
    clock_t time;
    time=clock();
    int N,count,feature,extract,match,outlier,solver;
    float *u_old,*v_old,*u_new,*v_new;
    float **A,**B;
    float uo,vo,fx,fy,Z,Dx,Dy,phi,e,Dx_o,Dy_o,phi_o,Z_o,gm;
    bool opticalFlow; // set(=1) for optical flow method
    opticalFlow=true;
    vector<DMatch> matches;
    vector<uchar> status;

    if(argc < 3)
    { help();
        return -1;
    }

    //Default option values
    feature=1;
    extract=1;
    match=1;
    outlier=1;
    solver=1;
    //Argument input for option selection
    if (argc>=8){
        feature=atoi(argv[3]);
        extract=atoi(argv[4]);
        match=atoi(argv[5]);
        outlier=atoi(argv[6]);
        solver=atoi(argv[7]);
    }
    // Intrinsic Calibration parameters for img size 320x240
    uo=157.73985;
    vo=134.19819;
    fx=391.54809;
    fy=395.45221;

    Mat img1 = imread(argv[1]);
    Mat mask1 = imread(argv[2]);
    cvtColor(mask1,mask1,CV_BGR2GRAY);
    Mat img2 = imread(argv[3]);
    if(img1.empty() || img2.empty() || mask1.empty())
    {
        printf("Can't read one of the images\n");
        return -1;
    }
    /*
    if(opticalFlow) {
        resize(img1,img1,Size(60,50));
        resize(img2,img2,Size(60,50));
    }
*/
    // detecting keypoints
    vector<KeyPoint> keypoints1, keypoints2;
    vector<Point2f> keypoints1_2f,keypoints2_2f;

    if(!opticalFlow){
        switch(feature)
        {
        case 1: //FAST
        {int threshold=130;
            FastFeatureDetector detector(threshold);
            detector.detect(img1, keypoints1);
            detector.detect(img2, keypoints2);
            break;
        }
        case 2: //SURF
        {SurfFeatureDetector detector(2000);
            detector.detect(img1, keypoints1);
            detector.detect(img2, keypoints2);
            break;
        }
        case 3: //GFTT
        {int maxCorners=150;
            GoodFeaturesToTrackDetector detector(maxCorners);
            detector.detect(img1, keypoints1);
            detector.detect(img2, keypoints2);
            break;
        }
        case 4: //ORB
        {int maxCorners=150;
            OrbFeatureDetector detector(maxCorners);
            detector.detect(img1, keypoints1);
            detector.detect(img2, keypoints2);
            break;
        }
        case 5: //Harris  (change threshold, presently some default threshold)
        {
            Ptr<FeatureDetector> detector= FeatureDetector::create("HARRIS");
            detector->detect(img1, keypoints1);
            detector->detect(img2, keypoints2);
            break;
        }
        }

        // computing descriptors
        Mat descriptors1, descriptors2;
        switch(extract)
        {
        case 1: //SURF
        {
            SurfDescriptorExtractor extractor;
            extractor.compute(img1, keypoints1, descriptors1);
            extractor.compute(img2, keypoints2, descriptors2);
            break;
        }
        case 2: //SIFT
        {
            SiftDescriptorExtractor extractor;
            extractor.compute(img1, keypoints1, descriptors1);
            extractor.compute(img2, keypoints2, descriptors2);
            break;
        }
        case 3: //ORB
        {
            OrbDescriptorExtractor extractor;
            extractor.compute(img1, keypoints1, descriptors1);
            extractor.compute(img2, keypoints2, descriptors2);
            break;
        }
        case 6: // for Optical_Flow (all cases 6)
            break;
        }

        // matching descriptors
        //vector<DMatch> matches; (made global)
        switch (match)
        {
        case 1: //BruteForce
        {
            BFMatcher matcher(NORM_L2);
            matcher.match(descriptors1, descriptors2, matches);
            break;
        }
        case 2: //Flann
        {
            FlannBasedMatcher matcher;
            matcher.match(descriptors1, descriptors2, matches);
            break;
        }
        case 6: // for Optical_Flow (all cases 6)
            break;
        }


        // finding good matches
        vector< DMatch > good_matches;
        switch (outlier)
        {
        case 1:
        {
            double distance=50.; //quite adjustable/variable
            double confidence=0.99; //doesnt affect much when changed
            ransacTest(matches,keypoints1,keypoints2,good_matches,distance,confidence);
            break;
        }
        case 2:
        {
            //look whether the match is inside a defined area of the image
            //only 25% of maximum of possible distance
            double tresholdDist = 0.25*sqrt(double(img1.size().height*img1.size().height + img1.size().width*img1.size().width));
            good_matches.reserve(matches.size());
            for (size_t i = 0; i < matches.size(); ++i)
            {
                Point2f from = keypoints1[matches[i].queryIdx].pt;
                Point2f to = keypoints2[matches[i].trainIdx].pt;
                //calculate local distance for each possible match
                double dist = sqrt((from.x - to.x) * (from.x - to.x) + (from.y - to.y) * (from.y - to.y));
                //save as best match if local distance is in specified area and on same height
                if (dist < tresholdDist)
                {
                    good_matches.push_back(matches[i]);
                }
            }
            break;
        }
        case 3: //dist<2*min_dist
        {
            double max_dist = 0; double min_dist = 100;

            //-- Quick calculation of max and min distances between keypoints
            for( int i = 0; i < descriptors1.rows; i++ )
            { double dist = matches[i].distance;
                if( dist < min_dist ) min_dist = dist;
                if( dist > max_dist ) max_dist = dist;
            }

            printf("-- Max dist : %f \n", max_dist );
            printf("-- Min dist : %f \n", min_dist );

            //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
            //-- PS.- radiusMatch can also be used here.

            for( int i = 0; i < descriptors1.rows; i++ )
            { if( matches[i].distance < 2*min_dist )
                { good_matches.push_back( matches[i]); }
            }
            break;
        }
        case 6: // for Optical_Flow (all cases 6)
            break;
        }

        matches=good_matches; // update matches by good_matches
        N=matches.size();  // no of matched feature points
    } //if(!opticalFlow)
    else {
        int maxCorners=150;
        std::vector<cv::KeyPoint> _keypoints1;	// all keypoints detected
        GoodFeaturesToTrackDetector detector(maxCorners);

        detector.detect(img1, _keypoints1, mask1);

        // convert KeyPoint to Point2f
        for (int i=0;i<_keypoints1.size(); i++)
        {
            float x= _keypoints1[i].pt.x;
            float y= _keypoints1[i].pt.y;
            keypoints1_2f.push_back(cv::Point2f(x,y));
        }

        // subpixel corner refinement for keypoints1_2f
        Size SPwinSize = Size(3,3);		//search window size=(2*n+1,2*n+1)
        Size zeroZone = Size(1,1);	// dead_zone size in centre=(2*n+1,2*n+1)
        TermCriteria SPcriteria=TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01);
        //    cornerSubPix(img1, keypoints1_2f, SPwinSize, zeroZone, SPcriteria);

        // LK Sparse Optical Flow
        vector<uchar> status;
        vector<float> err;
        Size winSize=Size(11,11);
        int maxLevel=3;
        TermCriteria criteria=TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01);
        int flags=0;
        double minEigThreshold=1e-4;
        cv::calcOpticalFlowPyrLK(img1, img2, keypoints1_2f, keypoints2_2f, status, err, winSize, maxLevel, criteria, flags, minEigThreshold);

        // subpixel corner refinement for keypoints2_2f
        //    cornerSubPix(img2, keypoints2_2f, SPwinSize, zeroZone, SPcriteria);
        
        float dist;
        vector<Point2f> matchedKeypoints2_2f;
        // convert Point2fs to KeyPoints
        for (int i=0;i<keypoints2_2f.size(); i++)
        {
            float x1= keypoints1_2f[i].x;
            float y1= keypoints1_2f[i].y;
            float x2= keypoints2_2f[i].x;
            float y2= keypoints2_2f[i].y;

            dist = sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) ); 	// dist betwn obtained matches
            if(status[i]==1 && dist<=20){			// min dist threshold
                KeyPoint kp1(x1,y1,1.0,-1.0,0.0,0,-1);
                keypoints1.push_back(kp1);

                KeyPoint kp2(x2,y2,1.0,-1.0,0.0,0,-1);
                matchedKeypoints2_2f.push_back(Point2f(x2,y2));
                keypoints2.push_back(kp2);
                //          fmatches.push_back(i);
            }
        }

        N=keypoints2.size();  // no of matched feature points
        RotatedRect bbox = minAreaRect(Mat(matchedKeypoints2_2f));
        Point2f pts[4];
        bbox.points(pts);
        Mat img = Mat::zeros(img1.size(),CV_8UC1);
        vector<vector<Point> > polyPts;
        polyPts.resize(1);
        for(int j = 0 ; j<4 ; j++){
            polyPts[0].push_back(pts[j]);
        }
        Scalar color = Scalar(255,255,255);
        cv::fillPoly(img,polyPts,color,8,0,Point());
        imshow("bounding Box",img);
        waitKey(0);
    }

    // Old and new consecutive frames pixel coordinate
    u_old=new float [N];
    v_old=new float [N];
    u_new=new float [N];
    v_new=new float [N];

    A=new float* [N]; //old [X/Z Y/Z 1]
    B=new float* [N]; //new [Xn/Z Yn/Z 1]

    for(int i=0; i<N; i++)
    {
        A[i] = new float [3];
        B[i] = new float [3];
    }

    // Obtaining pixel coordinates of feature points
    for(size_t i = 0; i < N; i++)
    {
        Point2f point1,point2;
        if(!opticalFlow){
            point1 = keypoints1[matches[i].queryIdx].pt;
            point2 = keypoints2[matches[i].trainIdx].pt;
        }
        else {
            point1 = keypoints1[i].pt;
            point2 = keypoints2[i].pt;
        }
        u_old[i]=point1.x;
        v_old[i]=point1.y;
        u_new[i]=point2.x;
        v_new[i]=point2.y;

        A[i][0] = -(u_old[i]-uo)/fx;
        A[i][1] = (v_old[i]-vo)/fy;
        A[i][2] = 1;

        B[i][0] = -(u_new[i]-uo)/fx;
        B[i][1] = (v_new[i]-vo)/fy;
        B[i][2] = 1;

    }

    // Finding least square error using Gradient Descent Method
    // x_vect={Dx,Dy,phi,Z} and x(n+1)=x(n)-grad(f(x(n)))
    // f(x)=sum{i=1 to N}[(Dx-Z(A[i][0]*cos(phi)-A[i][1]*sin(phi)-B[i][0]))^2] + sum{i=1 to N}[(Dy-Z(A[i][0]*sin(phi)+A[i][1]*cos(phi)-B[i][1]))^2]
    // grad(f(x))={df/dDx,df/dDy,df/dphi,df/dZ}

    //initial guess
    Dx=0;Dy=0;phi=0;Z=1;

    // Initial error
    e=0;
    for(size_t i = 0; i < N; i++){
        e =e+(Dx-Z*(A[i][0]*cos(phi)-A[i][1]*sin(phi)-B[i][0]))*(Dx-Z*(A[i][0]*cos(phi)-A[i][1]*sin(phi)-B[i][0]))+(Dy-Z*(A[i][0]*sin(phi)+A[i][1]*cos(phi)-B[i][1]))*(Dy-Z*(A[i][0]*sin(phi)+A[i][1]*cos(phi)-B[i][1]));
    }

    // Iterate x_vect={Dx,Dy,phi,Z} using gradient functions until error<0.01
    count=0;
    //gm=0.005;
    while(e>=0.01){
        count++;
        //Old x_vect={Dx,Dy,phi,Z}
        Dx_o=Dx;Dy_o=Dy;phi_o=phi;Z_o=Z;
        switch (solver)
        {
        case 1: gm=0.005; // Gradient Descent
            break;
        case 2: gm=1/e; // Newton-Raphson
            break;
        case 6: gm=0.005; //optical flow
        }

        //New x_vect={Dx,Dy,phi,Z}
        Dx=Dx_o-gm*df_dDx(Dx_o,Dy_o,phi_o,Z_o,A,B,N);
        Dy=Dy_o-gm*df_dDy(Dx_o,Dy_o,phi_o,Z_o,A,B,N);
        phi=phi_o-gm*df_dphi(Dx_o,Dy_o,phi_o,Z_o,A,B,N);
        Z=Z_o-gm*df_dZ(Dx_o,Dy_o,phi_o,Z_o,A,B,N);

        // Find error
        e=0;
        for(size_t i = 0; i < N; i++){
            e =e+(Dx-Z*(A[i][0]*cos(phi)-A[i][1]*sin(phi)-B[i][0]))*(Dx-Z*(A[i][0]*cos(phi)-A[i][1]*sin(phi)-B[i][0]))+(Dy-Z*(A[i][0]*sin(phi)+A[i][1]*cos(phi)-B[i][1]))*(Dy-Z*(A[i][0]*sin(phi)+A[i][1]*cos(phi)-B[i][1]));
        }
        //cout<<e<<"\t";
    }

    time=clock()-time;
    cout<<"N "<<N<<"\n"<<Dx<<"\n"<<Dy<<"\n"<<phi<<"\n"<<Z<<"\n";
    cout<<e<<"\n"<<count<<"\n";
    cout<<((float)time)/CLOCKS_PER_SEC<<"\n";

    // drawing the keypoints in two imgs
    const Scalar& color=Scalar(255,255,0); //BGR
    int flags=DrawMatchesFlags::DEFAULT;
    namedWindow("keypoints1", 1);
    Mat img_key1;
    drawKeypoints(img1, keypoints1,img_key1,color,flags);
    imshow("keypoints1", img_key1);
    waitKey(0);
    
    namedWindow("keypoints2", 1);
    Mat img_key2;
    drawKeypoints(img2, keypoints2,img_key2,color,flags);
    imshow("keypoints2", img_key2);
    waitKey(0);

    /*
    // drawing the rmatches
    namedWindow("matches", 1);
    Mat img_matches;
    drawMatches(img1, keypoints1, img2, keypoints2, matches, img_matches);
    imshow("matches", img_matches);
    waitKey(0);
*/
    return 0;
}