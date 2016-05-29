/*****************************************************************************************************
 * FILENAME :        map_reduce_histogram.cpp
 *
 * DESCRIPTION :
 *       This file contains methods to compute histogram on a single channel image
 *		using map-reduce technique and parallel processing
 *
 * FUNCTIONS:
 *   std::multimap<Img_Type, int>  mapper(std::vector< std::vector<Img_Type> > image_data)
 *	std::map<Img_Type, int>  reducer(std::multimap<Img_Type, int> key_value)
 *	std::map<Img_Type, int>  merge(std::map<Img_Type, int> m1, std::map<Img_Type, int> m2)
 *	std::pair<std::vector<float>, std::vector<int> > map_reduce_histogram(std::vector<std::vector<Img_Type> > image_data, int bins=255)
 ******************************************************************************************************/

#include <omp.h>
#include <time.h>
#include <iostream>
#include <map>
#include <utility>
#include <ctime>
#include <numeric>

using namespace std;
using namespace cv;

////////////////////////////////////////////////////////////////////////////////
//
// Member Function: mapper
// Description:  Mapper function to map each pixel intensity to a value 1
// Input Parameter:
//		 std::vector< std::vector<Img_Type> > image_data 		Input Image
//	Returns:
//			std::multimap<Img_Type, int> key_value_map
////////////////////////////////////////////////////////////////////////////////

template<class Img_Type>
std::multimap<Img_Type, int>  mapper(
	std::vector< std::vector<Img_Type> > image_data){
	
	//cout<<"Mapper in thread number "<<omp_get_thread_num()<<endl;
	
	// Create a multimap (non-unique keys)
	std::multimap<Img_Type, int> key_value_map;
	
	// Iterate through each pixel and assign key=pixel and value=1
	typename std::vector< std::vector<Img_Type> >::iterator row;
    typename std::vector<Img_Type>::iterator col;
    for (row = image_data.begin(); row != image_data.end(); ++row) {
         for (col = row->begin(); col != row->end(); ++col) {
            key_value_map.insert (std::pair<Img_Type,int>(*col,1));
         }
    }
	return key_value_map;

}

////////////////////////////////////////////////////////////////////////////////
//
// Member Function: reducer
// Description:  Reducer function to build a count of ocuurence of each
//				 pixel intensity.
// Input Parameter:
//		 std::multimap<Img_Type, int> key_value
//	Returns:
//		std::map<Img_Type, int> unique_key_value
////////////////////////////////////////////////////////////////////////////////

template<class Img_Type>
std::map<Img_Type, int>  reducer(
	std::multimap<Img_Type, int> key_value){
	
	//cout<<"Reducer in thread number "<<omp_get_thread_num()<<endl;
	
	// Create a map (unique keys)
	std::map<Img_Type, int> unique_key_value;
	
	// Use the multimap key_value to get the count for each unique key
    std::vector<Img_Type> vec_keys;
    typename std::multimap<Img_Type, int>::iterator it_begin = 
		key_value.begin();
    typename std::multimap<Img_Type, int>::iterator it_end = 
		key_value.end();
    transform(it_begin, it_end, back_inserter(vec_keys), 
		[](std::pair<Img_Type, int> p) { return p.first;} );
            
    Img_Type old_val = 0;
    Img_Type new_val = 0;
    Img_Type this_count = 0;
    
    for(typename std::vector<Img_Type>::iterator it = vec_keys.begin();
		it != vec_keys.end();++it){
			
		new_val = *it;
		this_count++;
		if(new_val==old_val){
			continue;
		}else{
			unique_key_value.insert (std::make_pair(old_val,this_count));
			this_count = 0;
		}
		old_val = new_val;
	}
	
	return unique_key_value;
}

////////////////////////////////////////////////////////////////////////////////
//
// Member Function: reducer2
// Description:  Fast Reducer function to build a count of ocuurence of each
//				 pixel intensity.
// Input Parameter:
//		 std::multimap<Img_Type, int> key_value
//       Img_Type range_lower
//       Img_Type range_upper
//       int bins=255
//	Returns:
//		std::map<Img_Type, int> unique_key_value
////////////////////////////////////////////////////////////////////////////////

template<class Img_Type>
std::map<Img_Type, int>  reducer2(
	std::multimap<Img_Type, int> key_value,
	Img_Type range_lower,
	Img_Type range_upper,
	int bins=255){
	
	//cout<<"Reducer2 in thread number "<<omp_get_thread_num()<<endl;
	
	// Step size for histogram
	Img_Type stepsize = (range_upper - range_lower)/(Img_Type)bins;
	
	// Create a map (unique keys)
	std::map<Img_Type, int> unique_key_value;
	
	typename std::multimap<Img_Type, int>::iterator it_begin = key_value.begin();
	
	int map_count=0;
	int static_map_count = key_value.size();
	
	for(Img_Type i = range_lower; i <= range_upper; i += stepsize){
		int count = 0;
		//cout<<(int)it_begin->first<<endl;
		//cout<<(int)(i + stepsize);
		while(it_begin->first < (i + stepsize)){
			count += it_begin->second;
			it_begin++;
			if(map_count++ >=static_map_count)
				return unique_key_value;
		}
		//cout<<"(i, count)=("<<(int)i<<","<< count<<")"<<endl;
		unique_key_value.insert(std::pair<Img_Type, int>(i, count));
	}
	
	return unique_key_value;
}

////////////////////////////////////////////////////////////////////////////////
//
// Member Function: merge
// Description:  Function for merging 2 maps
// Input Parameter:
//		 std::map<Img_Type, int> m1
//		 std::map<Img_Type, int> m2
//	Returns:
// 		std::map<Img_Type, int> m12
////////////////////////////////////////////////////////////////////////////////

template<class Img_Type>
std::map<Img_Type, int>  merge(
	std::map<Img_Type, int> m1, 
	std::map<Img_Type, int> m2){
	
	std::map<Img_Type, int> m12 = m1;
    m12 = std::accumulate( m2.begin(), m2.end(), m12,
        []( std::map<Img_Type, int> &m, 
			const std::pair<const Img_Type, int> &p ){
            return ( m[p.first] +=p.second, m );
        } );
                
    return m12;       
}

////////////////////////////////////////////////////////////////////////////////
//
// Member Function: multi_merge
// Description:  Function for merging 4 maps
// Input Parameter:
//		 std::map<Img_Type, int> m1
//		 std::map<Img_Type, int> m2
//		 std::map<Img_Type, int> m3
//		 std::map<Img_Type, int> m4
//	Returns:
// 		std::map<Img_Type, int> m1234
////////////////////////////////////////////////////////////////////////////////


template<class Img_Type>
std::map<Img_Type, int>  multi_merge(
	std::map<Img_Type, int> m1, 
	std::map<Img_Type, int> m2,
	std::map<Img_Type, int> m3,
	std::map<Img_Type, int> m4){
	
	std::map<Img_Type, int> m1234 = m1;
    for(typename std::map<Img_Type, int>::iterator it = m1234.begin(); it != m1234.end(); it++){
		Img_Type this_key = it->first;
		m1234[this_key] = m1[this_key] + m2[this_key] + m3[this_key] + m4[this_key];
	}
    return m1234;       
}

////////////////////////////////////////////////////////////////////////////////
//
// Member Function: map_reduce_histogram
// Description:  Function to create a histogram of an image
// Input Parameter:
//		 std::vector<std::vector<Img_Type> > image_data    Input Image
//       Img_Type range_lower
//       Img_Type range_upper
//       int bins=255
//	Returns:
// 		std::map<Img_Type, int> histogram_map
////////////////////////////////////////////////////////////////////////////////

template<class Img_Type>
std::map<Img_Type, int> map_reduce_histogram(
	std::vector<std::vector<Img_Type> > image_data, 
	Img_Type range_lower,
	Img_Type range_upper,
	int bins=255){
	
	//cout<<"Starting map reduce histogram"<<endl;
	
	// Check the number of processor cores and number of threads 
	int nProcessors=omp_get_max_threads();
	//cout<<"Number of online processors: "<<nProcessors<<endl;
	omp_set_num_threads(nProcessors);
    //cout<<"Check num of threads: "<<omp_get_num_threads()<<endl;
	
	// Divide original image vector into 4 parts
	std::vector< std::vector<Img_Type> > v1, v2, v3, v4;	
	std::vector<std::vector< std::vector<Img_Type> > > v;
	int rows = image_data.size();
	int colc = image_data[0].size();
	int count = 0;
	typename std::vector< std::vector<Img_Type> >::const_iterator row;
    for (row = image_data.begin(); row != image_data.end(); ++row) {
		if(count<=rows/4)
			v1.push_back(*row);
		else if(count>rows/4 && count<=rows/2)
			v2.push_back(*row);
		else if(count>rows/2 && count<=(3*rows)/4)
			v3.push_back(*row);
		else
			v4.push_back(*row);
		count++;
    }
    v.push_back(v1);
    v.push_back(v2);
    v.push_back(v3);
    v.push_back(v4);

	// Display size of subdivisions
	//cout<<"Size of image vector: "<<image_data.size()<<"X"<<image_data[0].size()<<endl;
	//cout<<"Size of v1: "<<v1.size()<<" X "<<v1[0].size()<<endl;
	//cout<<"Size of v2: "<<v2.size()<<" X "<<v2[0].size()<<endl;
	//cout<<"Size of v3: "<<v3.size()<<" X "<<v3[0].size()<<endl;
	//cout<<"Size of v4: "<<v4.size()<<" X "<<v4[0].size()<<endl;
	
	// Vector for storing intermediate results
	std::vector<std::map<Img_Type, int> > v_map;

	// Start map and reduce operations for the 4 parts in parallel 
	// threads. 4 intermediate maps are generated.
#pragma omp parallel for 
    for(int i=0;i<4;i++){
		int tid=omp_get_thread_num();
		int nThreads=omp_get_num_threads();
		//cout<<"Starting thread number: "<<tid<<endl;
		std::map<Img_Type, int>  key_val = reducer2(mapper(v[i]),range_lower,range_upper,bins);
		//cout<<"Finished map-reduce for thread number: "<<tid<<endl;
		
#pragma omp critical		
		v_map.push_back(key_val);
	}

	// Merging the intermediate maps
	std::map<Img_Type, int> merged_map = multi_merge(v_map[0],v_map[1],v_map[2],v_map[3]);

	return merged_map;

}

