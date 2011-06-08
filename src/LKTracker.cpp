#include <LKTracker.h>
using namespace cv;

void filterPts(vector<cv::Point2f>& points,vector<uchar>& status,vector<float>& fb_err,vector<float>& sim_err, float fbmed, float simed){
  size_t i, k;
  for( i=k = 0; i<points.size(); ++i ){
      if( !status[i] )
        continue;
      if(fb_err[i] <= fbmed && sim_err[i]<= simed)
        points[k++] = points[i];
  }
  points.resize(k);
}

void calcFBError(const  vector<cv::Point2f> &pts1, const vector<cv::Point2f> &pts2, vector<float> &fberror){
  for( int i= 0; i<pts1.size(); ++i ){
      fberror[i] = norm(pts1[i]-pts2[i]);
  }
}

LKTracker::LKTracker(){
  term_criteria = TermCriteria( TermCriteria::COUNT+TermCriteria::EPS, 20, 0.03);
  window_size = Size(5,5);
  level = 5;
  lambda = 0.5;
}

void LKTracker::trackf2f(const Mat& img1, const Mat& img2, vector<Point2f> &points1, vector<cv::Point2f> &points2){
  //Forward-Backward tracking
  calcOpticalFlowPyrLK( img1,img2, points1, points2, status,sim_error, window_size, level, term_criteria, lambda, 0);
  calcOpticalFlowPyrLK( img2,img1, points2, pointsFB, FB_status,FB_error, window_size, level, term_criteria, lambda, 0);
  //Compute the real FB-error
  calcFBError(points1,pointsFB,FB_error);
  //Get Error Medians
  sim_med = median(sim_error);
  FB_med = median(FB_error);
  printf("Forward-Backward error median = %f\n Similarity error median = %f\n",FB_med,sim_med);
  //Filter out points with FB_error[i] > median(FB_error) && points with sim_error[i] > median(sim_error)
  filterPts(points2,status,FB_error,sim_error,FB_med,sim_med);
  //Get Reliable Points
  //points = pointsFB;
}

/*
 * old OpenCV style
void LKTracker::init(Mat img0, vector<Point2f> &points){
  //Preallocate
  //pyr1 = cvCreateImage(Size(img1.width+8,img1.height/3),IPL_DEPTH_32F,1);
  //pyr2 = cvCreateImage(Size(img1.width+8,img1.height/3),IPL_DEPTH_32F,1);
  //const int NUM_PTS = points.size();
  //status = new char[NUM_PTS];
  //track_error = new float[NUM_PTS];
  //FB_error = new float[NUM_PTS];
}

void LKTracker::trackf2f(..){
  cvCalcOpticalFlowPyrLK( &img1, &img2, pyr1, pyr1, points1, points2, points1.size(), window_size, level, status, track_error, term_criteria, CV_LKFLOW_INITIAL_GUESSES);
  cvCalcOpticalFlowPyrLK( &img2, &img1, pyr2, pyr1, points2, pointsFB, points2.size(),window_size, level, 0, 0, term_criteria, CV_LKFLOW_INITIAL_GUESSES | CV_LKFLOW_PYR_A_READY | CV_LKFLOW_PYR_B_READY );
}
*/

