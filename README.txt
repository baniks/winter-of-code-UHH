###############################################################
# Winter of Code Submission
###############################################################


###############################################################
# Student Details
###############################################################
Name: Soubarna Banik
Email: 4banik@informatik.uni-hamburg.de


###############################################################
# Assigned tasks:
###############################################################
1. Subsampling
2. Median Filtering (5X5)
3. Median Filtering (11X11)
4. Histogram


###############################################################
# Dependencies
###############################################################
1. OpenCV - Version 2.4 
2. Openmp - Version 3.1
3. The -fopenmp flag is used during compilation
4. C++11 features have been used. The -std=c++11 flag is used while compilation


###############################################################
# Configuration of test machine
###############################################################
CPU - Intel(R) Core(TM) i3-4005U CPU @ 1.70GHz (4 cores)
RAM - 8GB
OS - Ubuntu 14.04 LTS (64 bit)


###############################################################
# Build Instructions
###############################################################
1. Edit Makefile and set required paths:
   	i.   CCFLAGS- set of the path of the file omp.h using -I flag
    ii.  Target mainWOC: set the lib path of openmp using -L flag
	iii. Rules: set the lib path of openmp using -L flag
2. Run - make
3. Execute - ./mainWOC


###############################################################
# Code Structure
###############################################################
File: mainWOC.cpp
Description :
This file contains a main function which calls following image processing methods from
template_image.cpp and map_reduce_histogram.cpp to run on grey and color images
- Subsampling
- Median Filter (5x5) 
- Median Filter (11x11)
- Histogram

File: template_image.cpp
Description: 
This class contains the following image processing methods that are datatype independent.
- Subsampling
- Median Filter

File: map_reduce_histogram.cpp
Description: 
This file contains methods to compute histogram on a single channel image
using map-reduce technique and parallel processing


###############################################################
# Miscellaneous Information
###############################################################
1. Only the histogram (using map-reduce) was implemented using multi-threading.
2. For multi-channel images, operations on the 3 channels were done in parallel.


###############################################################
# Results
###############################################################
Please see the file woc-test-results.txt


