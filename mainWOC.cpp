/*****************************************************************************************************
* FILENAME :        mainWOC.cpp
*
* DESCRIPTION :
*       This file contains a main function which calls following image processing methods from
*		template_image.cpp and map_reduce_histogram.cpp to run on grey and color images
*		- Subsampling
*		- Median Filter (5x5) and (11x11)
*		- Histogram
******************************************************************************************************/

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <omp.h>
#include "template_image.cpp"
#include "map_reduce_histogram.cpp"

using namespace cv;
using namespace std;

RNG rng(12345);


int main(){
	
	vector<Mat> test_imgs_grey;
    vector<Mat> test_imgs_bgr;

    Size small=Size(2048,2048);
    Size large=Size(4096,4096);

    //Initializing gray images
    Mat grey_small_uint=Mat(small,CV_8UC1);
    Mat grey_large_uint=Mat(large,CV_8UC1);
    Mat grey_small_float=Mat(small,CV_32FC1);
    Mat grey_large_float=Mat(large,CV_32FC1);

    rng.fill(grey_small_uint,RNG::UNIFORM,Scalar(0),Scalar(255),true);
    rng.fill(grey_large_uint,RNG::UNIFORM,Scalar(0),Scalar(255),true);
    rng.fill(grey_small_float,RNG::UNIFORM,Scalar(0),Scalar(255),true);
    rng.fill(grey_large_float,RNG::UNIFORM,Scalar(0),Scalar(255),true);

    test_imgs_grey.push_back(grey_small_uint);
    test_imgs_grey.push_back(grey_small_float);
    test_imgs_grey.push_back(grey_large_uint);
    test_imgs_grey.push_back(grey_large_float);

    //Initializing bgr images
    Mat bgr_small_uint=Mat(small,CV_8UC3);
    Mat bgr_small_float=Mat(small,CV_32FC3);
    Mat bgr_large_uint=Mat(large,CV_8UC3);
    Mat bgr_large_float=Mat(large,CV_32FC3);

	rng.fill(bgr_small_uint,RNG::UNIFORM,Scalar::all(0), Scalar::all(255),true);
	rng.fill(bgr_small_float,RNG::UNIFORM,Scalar::all(0), Scalar::all(255),true);
	rng.fill(bgr_large_uint,RNG::UNIFORM,Scalar::all(0), Scalar::all(255),true);
	rng.fill(bgr_large_float,RNG::UNIFORM,Scalar::all(0), Scalar::all(255),true);

	
	test_imgs_bgr.push_back(bgr_small_uint);
    test_imgs_bgr.push_back(bgr_small_float);
    test_imgs_bgr.push_back(bgr_large_uint);
    test_imgs_bgr.push_back(bgr_large_float);
	
	
	Mat img_src, img_dst;
    int cnt=1, i;
    clock_t start;
    double duration;
    int num_iteration=100;

	/////////////////////////////////// GRAY IMAGES /////////////////////////////////////////

    cout<<"Processing Gray Images..."<<endl;
    cout<<"========================="<<endl;
    
    //Iterate over gray images
    for(vector<Mat>::iterator it=test_imgs_grey.begin();it<test_imgs_grey.end();++it){

       img_src=*it;
       Size s=img_src.size();
       string shape=(s==small)?" small shape ":" large shape ";
       string depth=(img_src.depth()==0)?"uint8":"float";
       
       cout<<endl<<"Testcase "<<cnt<<": grey,"<<shape<<", pixeltype: "<<depth<<endl;
       
       //Iterate over four operations - Subsampling, Median filtering (5x5),
       //Median Filtering (11x11) and Histogram
       for(int op=0;op<4;op++){
       
		   if(img_src.depth()==0){

		   		//Img_Type: uint8 
		   		
				vector<vector<uchar> > v_img_src,v_img_dst;
				template_image <uchar> obj;
				uchar dummy1;
				//Convert source Mat object to vector<vector<Img_Type> > 
				//using template_image object
				v_img_src=obj.convertMat2Vec(img_src,dummy1);
			
				start=clock();
				switch(op){
				case 0:
					//Subsampling
					for(i=0;i<num_iteration;i++)
						v_img_dst=obj.subSample(v_img_src);
					cout<<"\tOperation: Subsampling ....";
					break;
				case 1:
					//Median Filtering (5x5)
					for(i=0;i<num_iteration;i++)
						v_img_dst=obj.medianFilter(v_img_src,5);
					cout<<"\tOperation: Median Filter (5x5) ....";
					break;
				case 2:
					//Median Filtering (11x11)
					for(i=0;i<num_iteration;i++)
						v_img_dst=obj.medianFilter(v_img_src,11);
					cout<<"\tOperation: Median Filter (11x11) ....";
					break;	
				case 3:
					//Histogram
					for(i=0;i<num_iteration;i++){
						//std::pair<std::vector<float>, std::vector<int> > histogram_edges_vals = map_reduce_histogram(v_img_src, 255);
						unsigned char range_l=0, range_u=255;
						std::map<unsigned char, int>  histogram_map = map_reduce_histogram(v_img_src, range_l, range_u, 255);
					}
					cout<<"\tOperation: Histogram ....";
					break;				
				default:
					cout<<"No Operation"<<endl;
				}
		   }
		   else if(img_src.depth()==5){
		   		
		   		//Img_Type: float 
		   		
				vector<vector<float> > v_img_src,v_img_dst;
				template_image <float> obj;
				float dummy2;
				
				//Convert source Mat object to vector<vector<Img_Type> > 
				//using template_image object
				v_img_src=obj.convertMat2Vec(img_src,dummy2);
			
				start=clock();
				
				switch(op){
				case 0:
					//Subsampling
					for(i=0;i<num_iteration;i++)
						v_img_dst=obj.subSample(v_img_src);
					cout<<"\tOperation: Subsampling ....";
					break;
				case 1:
					//Median Filter (5x5)
					for(i=0;i<num_iteration;i++)
						v_img_dst=obj.medianFilter(v_img_src,5);
					cout<<"\tOperation: Median Filter (5x5) ....";
					break;
				case 2:
					//Median Filter (11x11)
					for(i=0;i<num_iteration;i++)
						v_img_dst=obj.medianFilter(v_img_src,11);
					cout<<"\tOperation: Median Filter (11x11) ....";
					break;				
				case 3:
					//Histogram
					for(i=0;i<num_iteration;i++){
						//std::pair<std::vector<float>, std::vector<int> > histogram_edges_vals = map_reduce_histogram(v_img_src, 255);
						float range_l=0.0, range_u=255.0;
						std::map<float, int>  histogram_map = map_reduce_histogram(v_img_src, range_l, range_u, 255);
					}
					cout<<"\tOperation: Histogram ....";
					break;
				default:
					cout<<"No Operation"<<endl;
				}
					   
		   }
	   
			duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
			cout<<" Time for "<<i<<" runs: "<<duration <<" seconds"<<endl;
		}

		cnt++;
    }

	/////////////////////////////////// COLOR IMAGES /////////////////////////////////////////
	
	cout<<endl<<"Processing Color Images..."<<endl;
    cout<<"========================="<<endl;

	//Iterating over color images
	for(vector<Mat>::iterator it=test_imgs_bgr.begin();it<test_imgs_bgr.end();++it){

		img_src=*it;

		Size s=img_src.size();
		string shape=(s==small)?" small shape ":" large shape ";
		string depth=(img_src.depth()==0)?"uint8":"float";
		
		cout<<endl<<"Testcase "<<cnt<<": BGR,"<<shape<<", pixeltype: "<<depth<<endl;
		
		//Iterating over operations
		for(int op=0;op<4;op++){

			if(img_src.depth()==0){
				
				//Img_Type: uint8
				
				vector<vector<Vec3b> > v_img_src,v_img_dst;
				template_image <Vec3b> obj;
				Vec3b dummy1;
				
				//Convert source Mat object to vector<vector<Img_Type> > 
				//using template_image object
				v_img_src=obj.convertMat2Vec(img_src,dummy1);
			
				vector<vector<vector<uchar> > > channels;
				uchar dummy2;
				template_image <uchar> u_obj;
			
				start=clock();
				
				//Repeat till num_iteration
				for(i=0;i<num_iteration;i++){
					
					//Split source image into vector of channels
					channels=obj.splitChannel(v_img_src,dummy2);
					vector<vector<uchar> > v_b_dst,v_g_dst,v_r_dst;
					
					if(op==0){
						//Operate Subsamping on each channel in parallel
						#pragma omp parallel sections
						{
							#pragma omp section
								v_b_dst=u_obj.subSample(channels[0]);
							
							#pragma omp section
								v_g_dst=u_obj.subSample(channels[1]);
							
							#pragma omp section
								v_r_dst=u_obj.subSample(channels[2]);
						}
						if(i==0)
							cout<<"\tOperation: Subsampling ....";
					}				
					if(op==1){
						//Operate Median Filter (5x5) on each channel in parallel
						#pragma omp parallel sections
						{
							#pragma omp section
								v_b_dst=u_obj.medianFilter(channels[0],5);
							
							#pragma omp section
								v_g_dst=u_obj.medianFilter(channels[1],5);
							
							#pragma omp section
								v_r_dst=u_obj.medianFilter(channels[2],5);
						}
						if(i==0)
							cout<<"\tOperation: Median Filter (5x5) ....";
					}
					if(op==2){
						//Operate Median Filter (11x11) on each channel in parallel
						#pragma omp parallel sections
						{
							#pragma omp section
								v_b_dst=u_obj.medianFilter(channels[0],11);
							
							#pragma omp section
								v_g_dst=u_obj.medianFilter(channels[1],11);
							
							#pragma omp section
								v_r_dst=u_obj.medianFilter(channels[2],11);
						}
						if(i==0)
							cout<<"\tOperation: Median Filter (11x11) ....";
					}
					if(op==3){
						//Calculate Histogram of each channel
						//std::pair<std::vector<float>, std::vector<int> > histogram_edges_vals = map_reduce_histogram(channels[0],255);
						//std::pair<std::vector<float>, std::vector<int> > histogram_edges_vals1 = map_reduce_histogram(channels[1],255);							
						//std::pair<std::vector<float>, std::vector<int> > histogram_edges_vals2 = map_reduce_histogram(channels[2],255);
						 
					    unsigned char range_l=0, range_u=255;
						std::map<unsigned char, int>  histogram_map1 = map_reduce_histogram(channels[0], range_l, range_u, 255);	
						std::map<unsigned char, int>  histogram_map2 = map_reduce_histogram(channels[1], range_l, range_u, 255);						
						std::map<unsigned char, int>  histogram_map3 = map_reduce_histogram(channels[2], range_l, range_u, 255);						

						if(i==0)
							cout<<"\tOperation: Histogram ....";						
					}

					//Merge operated channels together to create the processed image
					channels.clear();
					channels.push_back(v_b_dst);
					channels.push_back(v_g_dst);
					channels.push_back(v_r_dst);
				
					v_img_dst=obj.mergeChannel(channels);
				
					v_b_dst.clear();
					v_g_dst.clear();
					v_r_dst.clear();
					v_img_dst.clear();
				}
			
				v_img_src.clear();
				v_img_dst.clear();
				channels.clear();

			}
			else if(img_src.depth()==5){
				
				//Img_Type: float

				vector<vector<Vec3f> > v_img_src,v_img_dst;
				template_image <Vec3f> obj;
				Vec3f dummy1;
				
				//Convert source Mat object to vector<vector<Img_Type> > 
				//using template_image object
				v_img_src=obj.convertMat2Vec(img_src,dummy1);
			
				vector<vector<vector<float> > > channels;
				float dummy2;
			
				template_image <float> u_obj;
			
				start=clock();
				
				//Repeat till num_iteration
				for(i=0;i<num_iteration;i++){
				
					//Split source image into vector of channels
					channels=obj.splitChannel(v_img_src,dummy2);
					vector<vector<float> > v_b_dst,v_g_dst,v_r_dst;
				
					if(op==0){
						//Operate Subsampling on each channel in parallel
						#pragma omp parallel sections
						{
							#pragma omp section
								v_b_dst=u_obj.subSample(channels[0]);
							
							#pragma omp section
								v_g_dst=u_obj.subSample(channels[1]);
							
							#pragma omp section
								v_r_dst=u_obj.subSample(channels[2]);
						}
						if(i==0)
							cout<<"\tOperation: Subsampling ....";
					}				
					if(op==1){
						//Operate Median Filter(5x5) on each channel in parallel
						#pragma omp parallel sections
						{
							#pragma omp section
								v_b_dst=u_obj.medianFilter(channels[0],5);
							
							#pragma omp section
								v_g_dst=u_obj.medianFilter(channels[1],5);
							
							#pragma omp section
								v_r_dst=u_obj.medianFilter(channels[2],5);
						}
						if(i==0)
							cout<<"\tOperation: Median Filter (5x5) ....";
					}
					if(op==2){
						//Operate Median Filter(11x11) on each channel in parallel
						#pragma omp parallel sections
						{
							#pragma omp section
								v_b_dst=u_obj.medianFilter(channels[0],11);
							
							#pragma omp section
								v_g_dst=u_obj.medianFilter(channels[1],11);
							
							#pragma omp section
								v_r_dst=u_obj.medianFilter(channels[2],11);
						}
						if(i==0)
							cout<<"\tOperation: Median Filter (11x11) ....";
					}
					if(op==3){
						//Calculate Histogram of each channel
						//std::pair<std::vector<float>, std::vector<int> > histogram_edges_vals = map_reduce_histogram(channels[0],255);
						//std::pair<std::vector<float>, std::vector<int> > histogram_edges_vals1 = map_reduce_histogram(channels[1],255);							
						//std::pair<std::vector<float>, std::vector<int> > histogram_edges_vals2 = map_reduce_histogram(channels[2],255);						

					    float range_l=0.0, range_u=255.0;
						std::map<float, int>  histogram_mapf1 = map_reduce_histogram(channels[0], range_l, range_u, 255);	
						std::map<float, int>  histogram_mapf2 = map_reduce_histogram(channels[1], range_l, range_u, 255);						
						std::map<float, int>  histogram_mapf3 = map_reduce_histogram(channels[2], range_l, range_u, 255);					

						if(i==0)
							cout<<"\tOperation: Histogram ....";						
						
					}
					
					//Merge operated channels to create the processed image
					channels.clear();
					channels.push_back(v_b_dst);
					channels.push_back(v_g_dst);
					channels.push_back(v_r_dst);
				
					v_img_dst=obj.mergeChannel(channels);
				
					v_b_dst.clear();
					v_g_dst.clear();
					v_r_dst.clear();
					v_img_dst.clear();
				}
			
				v_img_src.clear();
				v_img_dst.clear();
				channels.clear();
			}
		
			duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
       		cout<<"Time for "<<i<<" runs: "<<duration <<" seconds"<<endl;
		
		}

		cnt++;
	}
	////////////////////////////////////////////////////////////////////////////
	
	return 0;
}
