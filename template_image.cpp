/*****************************************************************************************************
* FILENAME :        template_image.cpp
*
* DESCRIPTION :
*       This class contains the following image processing methods that are datatype
*		independent.
*		- Subsampling
*		- Median Filter
*
* FUNCTIONS:
*       vector<vector<Img_Type> > convertMat2Vec(Mat ,const Img_Type& )
*    	template<class T> vector<vector<vector<T> > > splitChannel(vector<vector<Img_Type> > , const T& )
*   	template<class T> vector<vector<Img_Type> >  mergeChannel(vector<vector<vector<T> > > )
*   	vector<vector<Img_Type> > subSample(vector<vector<Img_Type> > )
*   	vector<vector<Img_Type> > medianFilter(vector<vector<Img_Type> > ,int )
*
******************************************************************************************************/
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<vector>
#include <algorithm>
#include<array>

using namespace cv;
using namespace std;


static bool toggle=false;
static int cIdx=0;
static int idx=0;

////////////////////////////////////////////////////////////////////////////////
// 	Class:	template_image
////////////////////////////////////////////////////////////////////////////////

template <class Img_Type> class template_image
{
  	
  public:
	vector<vector<Img_Type> > convertMat2Vec(Mat img,const Img_Type& dummy );
    template<class T> vector<vector<vector<T> > > splitChannel(vector<vector<Img_Type> > vec, const T& dummy);
    template<class T> vector<vector<Img_Type> >  mergeChannel(vector<vector<vector<T> > > vec);
    vector<vector<Img_Type> > subSample(vector<vector<Img_Type> > img_src);
    vector<vector<Img_Type> > medianFilter(vector<vector<Img_Type> > img_src,int kernelSize);
	
	//Static method to toggle the static variable toggle
    static bool IsToggle(Img_Type x){
		toggle=!toggle;	
		return toggle;
	}
	
};

////////////////////////////////////////////////////////////////////////////////
//	Function: op_div4
//	Description: This function is used in Subsampling operation to divide
//				 each element of a vector by 4
////////////////////////////////////////////////////////////////////////////////

template <class T>
T op_div4(T i){
    T temp=i/4;
	return temp;
}

////////////////////////////////////////////////////////////////////////////////
// Function: operator+ overloaded
// Description: This function adds two vectors and returns the resultant
//				vector
////////////////////////////////////////////////////////////////////////////////

template <class Img_Type>
vector<Img_Type> operator+(const vector<Img_Type>& a, const vector<Img_Type>& b)
{
    vector<Img_Type> result;
    result.reserve(a.size());

    /*std::transform(a.begin(), a.end(), b.begin(), 
                   std::back_inserter(result), std::plus<Img_Type>());*/
                   
    for (int i = 0; i < a.size(); i++)
    	result[i] = a[i] + b[i];

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Function: operator- overloaded
// Description: This function subtracts a vector from another and returns the resultant
//				vector
////////////////////////////////////////////////////////////////////////////////

template <class Img_Type>
vector<Img_Type> operator-(const vector<Img_Type>& a, const vector<Img_Type>& b)
{
    vector<Img_Type> result;
    result.reserve(a.size());

    /*std::transform(a.begin(), a.end(), b.begin(), 
                   std::back_inserter(result), std::minus<Img_Type>());*/

    for (int i = 0; i < a.size(); i++)
    	result[i] = a[i] - b[i];

    return result;
}
////////////////////////////////////////////////////////////////////////////////
// Function: operator/ overloaded
// Description: This function divides each element of a vector by a constant
//				returns the output vector
////////////////////////////////////////////////////////////////////////////////

template <class Img_Type>
vector<Img_Type> operator/(const vector<Img_Type>& a1, const int& d)
{
  std::vector<Img_Type> a;
  a.reserve(a1.size());

  std::transform(a1.begin(), a1.end(), std::back_inserter(a),op_div4<Img_Type>);
  return a;
}

////////////////////////////////////////////////////////////////////////////////
// Member Function: convertMat2Vec
// Description:  This function converts the image from opencv Mat format to
//				 vector<vector<Img_Type> > format.
// Input Parameter: 
//		 			Mat 		img 		Input Image
//		 			Img_Type& 	dummy 		A dummy variable that signifies the
//											type of the input image (uchar or flaot)
//	Return Type:
//				vector<vector<Img_Type> >	
////////////////////////////////////////////////////////////////////////////////

template<class Img_Type>
vector<vector<Img_Type> > template_image<Img_Type>::convertMat2Vec(Mat img,const Img_Type& dummy ){

	vector<vector<Img_Type> > vec;
	
	for(int x=0;x<img.rows;x++){
		const Img_Type* p=img.ptr<Img_Type>(x);
		vector<Img_Type> v(p,p+img.cols);
		vec.push_back(v);
	}
	
	return vec;
}

////////////////////////////////////////////////////////////////////////////////
// Member Function: splitChannel
// Description:  This function splits a 3 channel image and returns a vector of 
//				 split channels.
// Input Parameter: 
//		 vector<vector<Img_Type> > 		vec 		source image
//		 			T& 					dummy 		A dummy variable that signifies the
//													type of the each channel (uchar or float)
//	Return Type:
//				vector<vector<vector<T> > >
////////////////////////////////////////////////////////////////////////////////


template<class Img_Type>
template<class T>
vector<vector<vector<T> > > template_image<Img_Type>::splitChannel(vector<vector<Img_Type> > vec, const T& dummy){
	
		vector<vector<vector<T> > > ret ;
		vector<vector<T> > b_vec,g_vec,r_vec;
		
		for(auto it=vec.begin();it<vec.end();it++){
			//extract row
			vector<Img_Type> r1=*it;
			
			vector<T> b,g,r;
			//Iterate over the row and copy each channel
			for(auto it1=r1.begin();it1<r1.end();it1++){
				Img_Type t=*it1;
				b.push_back((T)t[0]);
				g.push_back((T)t[1]);
				r.push_back((T)t[2]);

			}
			b_vec.push_back(b);
			g_vec.push_back(g);
			r_vec.push_back(r);

		}
		ret.push_back(b_vec);
		ret.push_back(g_vec);
		ret.push_back(r_vec);

		return ret;
}

////////////////////////////////////////////////////////////////////////////////
// Member Function: mergeChannel
// Description:  This function merges 3 channels to form a multichannel image 
//				 and returns the image. 
// Input Parameter: 
//		 vector<vector<vector<T> > > 	vec 		vector of channels
//	Return Type:
//		vector<vector<Img_Type> >
////////////////////////////////////////////////////////////////////////////////


template<class Img_Type>
template<class T>
vector<vector<Img_Type> >  template_image<Img_Type>::mergeChannel(vector<vector<vector<T> > > vec){
	
	//extract B,G,R channels
	vector<vector<T> > b,g,r;
	b=vec[0];
	g=vec[1];
	r=vec[2];
	
	auto it1=b.begin();
	auto it2=g.begin();
	auto it3=r.begin();
	
	vector<Img_Type> row;
	vector<vector<Img_Type> > ret;
	
	//Iterate over all three channels
	for(;it1<b.end() && it2<g.end() && it3<r.end();it1++,it2++,it3++){
		
		//extract rows
		vector<T> b_row=*it1;
		vector<T> g_row=*it2;	
		vector<T> r_row=*it3;
		
		auto it11=b_row.begin();
		auto it22=g_row.begin();
		auto it33=r_row.begin();
		
		//Iterate over row and merge 
		for(;it11<b_row.end() && it22<g_row.end() && it33<r_row.end();it11++,it22++,it33++){
			Img_Type cell(*it11,*it22,*it33);
			row.push_back(cell);
		}
		
		ret.push_back(row);
		row.clear();
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
// Member Function: subSample
// Description:  This function does a subsampling operation on a single channel  
//				 image and returns the subsampled image. 
// Input Parameter: 
//		 vector<vector<vector<Img_Type> > > 	img_src 	Input image
//	Return Type:
//		vector<vector<Img_Type> >
////////////////////////////////////////////////////////////////////////////////

template <class Img_Type>
vector<vector<Img_Type> > template_image<Img_Type>::subSample(vector<vector<Img_Type> > img_src)
{
	//Row wise odd even index partition
	
	int row_num=0,s=4;
	vector<vector<Img_Type> > part1,part2,part3,part4,res;
	vector<Img_Type> part1_row,part2_row,part3_row,part4_row;
	
	//Iterate over all rows of image
	for(auto it=img_src.begin();it<img_src.end();it++){
			row_num++;
			//extract row
			vector<Img_Type> row=*it;
		
		//For Odd row
			if(row_num%2!=0){
			
					//Copy the values at odd index to part1
					//and the values at even index to part2
					std::partition_copy(row.begin(),row.end(),std::back_inserter(part1_row),std::back_inserter(part2_row),IsToggle);
					part1.push_back(part1_row);
					part2.push_back(part2_row);
			}
			else if(row_num%2==0){
		//For Even row
					//Copy the values at odd index to part3
					//and the values at even index to part4
					std::partition_copy(row.begin(),row.end(),std::back_inserter(part3_row),std::back_inserter(part4_row),IsToggle);
					part3.push_back(part3_row);
					part4.push_back(part4_row);
					
					//Compute average of subsample window for every two row
					vector<Img_Type> temp=(part1_row + part2_row + part3_row + part4_row) / s;
					res.push_back(temp);
					
					//Clear temporary part variables after every two row
					part1_row.clear();
					part2_row.clear();
					part3_row.clear();
					part4_row.clear();
			}
			toggle=false;
	}
	    /*cout<<"RESULT"<<endl;
        for(auto it=res.begin();it<res.end();it++){
                vector<Img_Type> row=*it;
                for(auto it1=row.begin();it1<row.end();it1++){
                        float t=*it1;
                        cout<<t<<" ";
                }
                cout<<endl;
        }*/
	return res;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////

template <class T>
T operator+(const T& a1, const T& a2)
{
  T a;
  for (typename T::size_type i = 0; i < a1.size(); i++)
    a[i] = a1[i] + a2[i];
  return a;
}

template <class T1>
T1 operator-(const T1& a1, const T1& a2)
{
  T1 a;
  for (typename T1::size_type i = 0; i < a1.size(); i++)
    a[i] = a1[i] - a2[i];
  return a;
}

////////////////////////////////////////////////////////////////////////////////
// Member Function: medianFilter
// Description:  This function operates a median filter on a single channel  
//				 image and returns the filtered image. This is an implementation
//				 of the "Median Filtering in Constant Time" by Simon Perreault 
//				 and Patrick Hebert
// Input Parameter: 
//		 vector<vector<vector<Img_Type> > > 	img_src 	Input image
//					int							kernelSize	Mask window size
//	Return Type:
//		vector<vector<Img_Type> >
////////////////////////////////////////////////////////////////////////////////

template <class Img_Type>
vector<vector<Img_Type> > template_image<Img_Type>::medianFilter(vector<vector<Img_Type> > img_src,int kernelSize){
	
	//cout<<"Median filter called"<<endl;
	
	/*cout<<"Source image:"<<endl;
	for(int x=0;x<img_src.size();x++){
		for(int y=0;y<img_src[x].size();y++){
			cout<<(float)img_src[x][y]<<" ";
		}
		cout<<endl;
	}*/

	vector<vector<Img_Type> > img_dst(img_src.size(),(vector<Img_Type>(img_src[0].size(),0)));
		
	//Initialise column histograms for row 0 to kernelSize
    array<int,256> * col_hist;
    col_hist = new array<int,256> [img_src[0].size()];

    for(int p=0;p<img_src[0].size();p++){
        col_hist[p].fill(0);
    }

    for(int rowK=0;rowK<kernelSize;rowK++){
		for(int col=0;col<img_src[0].size();col++){
            int t=img_src[rowK][col];
            col_hist[col][t]+=1;
        }
    }
	
	//Initialize kernel histogram - centered at (kernelSize/2,kernelSize/2)
    array<int,256> hist{};
    for(int i=0;i<kernelSize;i++){
        array<int,256> temp=col_hist[i];
        hist=hist+temp;
    }

	int middle=(kernelSize*kernelSize+1)/2;
    int cum_sum=0,median=0;
    int radius=kernelSize/2;
	
	//Iterate over image
    for(int row=radius;row<img_src.size()-radius;row++){

        for(int col=radius;col<img_src[0].size()-radius;col++){

            //For all columns except the starting column, recompute histogram of the window
            //by adding column hist after and subtracting column hist before 
            //the lastwindow position
            if(col>radius){
                array<int,256> next=col_hist[col+radius];
                array<int,256> prev=col_hist[col-1-radius];
                hist=hist+next-prev;
            }

            //Compute median of the current window
            cum_sum=0;
            for(int g=0;g<256;g++)
            {
                cum_sum+=hist[g];
                if(cum_sum>=middle){
                    median=g;
                    break;
                }
            }
			
			//Update target image
            img_dst[row][col]=median;
       }
       
		//Re-initialising histogram window and Updating Column histograms after every row
       if(row<img_src.size()-radius-1){
            //Reset histogram window
            hist.fill(0);

            //Update column histogram
            //by subtracting value at the row before
            //and by adding value at next row
            for(int i=0;i<img_src[0].size();i++){
                array<int,256> temp=col_hist[i];
                int t1=img_src[row-radius][i];
                int t2=img_src[row+1+radius][i];

                temp[t1]--;
                temp[t2]++;
                col_hist[i]=temp;

                //Re-initialize hist
                if(i<kernelSize)
                    hist=hist+temp;
            }
        }
	}

	
	
	/*cout<<"Destination image:"<<endl;
	for(int x=0;x<img_dst.size();x++){
		for(int y=0;y<img_dst[x].size();y++){
			cout<<(float)img_dst[x][y]<<" ";
		}
		cout<<endl;
	}*/

	return img_dst;
	
}

