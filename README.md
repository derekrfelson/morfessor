# morfessor
A demonstration of morphological segmentation using Creutz &amp; Lagus' 2005 Morfessor Baseline algorithm

A report describing this project and its results can be found online at https://docs.google.com/document/d/1bSt95uOvm7MHE7v0AQnM6LDRPaKIdquGsMM-PHAzYj8/pub.

To compile the code, you will need a C++-14 compiler, cmake, the Boost math library (I used 1.58.0), and gflags. To compile the unit tests you will need pthreads and googletest. When you run cmake, it should check the requirements for your system and tell you if you are missing anything.

To get the code:

git clone https://github.com/derekrfelson/morfessor

To build the code:

cd morfessor  
mkdir build  
cd build  
cmake ..  
make  

To run the code:

cd scripts  
./evaluate.sh  

The evaluation script will create a results directory under which (deep in a subtree) you can find three files:

* model.txt    Contains the morphs (and their frequencies) identified by the algorithm during training.  
* test-segmentation.txt    Contains the program's attempt to break down words in the test data into their component parts.  
* results.txt    Contains the results of analyzing the accuracy of the program's proposed segmentation against the correct segmentation.  

If you look in evaluation.sh you will see where the training data and test data are stored (both under the testdata directory).
