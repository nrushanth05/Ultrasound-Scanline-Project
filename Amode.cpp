/*  
    Name: Nrushanth Suthaharan
    Student ID: 21070183
    Date: 2023-11-28
    Description: Generate a graph using input files, and user input to represent the scanline which is produced by an ultrasound
*/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
using namespace std;

float **createDataMatrix(int numElement, int numSample)
{
    // Create a 2D data matrix of size numElement and numSample
    float **RFData = new float*[numElement];
    for(int i =0;i<numElement;i++){
        RFData[i] = new float[numSample];
        for(int j =0;j<numSample;j++){
            RFData[i][j] = 0;
        }
    }
    return RFData;   
}
int loadRFData(float **RFData, const char *fileName, int numElement, int numSample)
{
    // Open the text file fileName, read the data and store into RFData
    // You can use the getline() command to extract the data lines from the txt files
    ifstream inFile;
    inFile.open(fileName); // open input file
    if(!inFile.fail()){
        for(int i =0;i<numElement;i++){
            for(int j =0;j<numSample;j++){
                string line; //variable to store line from input text file
                getline(inFile, line); // get one line from txt file
                istringstream bup(line); // convert string to float using bup()
                bup >> RFData[i][j]; //save each line on inFile to seperate index on RFData
            }    
        }
    }
    else{ //if file cannot be accessed
        return -1;
    }
    return 0;
}
ha
// Create an array containing the depth location (in z-direction) for each pixel on the scanline
float *genScanlineLocation(int &numPixel)
{
    float depth; // initialize variable for depth
    cout<<"How deep should the scaline be: "<<endl; // ask user for depth
    cin>>depth;
    cout<<"Enter number of pixels: "<<endl; //ask user for number of pixels
    cin>>numPixel;
    float* scanlineLocation = new float[numPixel]; // create dynamic array for each pixel on scanline
    float a = 0; //variable to count each position on scanline, starts at 0
    for(int i =0;i<numPixel;i++){
        scanlineLocation[i]=a; 
        a+=depth/(numPixel-1); // increase 'a' by constant difference
    }
    return scanlineLocation;
}

// Create an array containing the element location (in x-direction) of the ultrasound transducer
float *genElementLocation(int numElement, float PITCH)
{
    float* eleLocation= new float[numElement]; //create dynamic array to store location positions of transducer
    for(int i =0;i<numElement;i++){
        eleLocation[i]=(i-(numElement-1.0)/2)*PITCH; //calculate and save current index of eleLocation
    }
    return eleLocation;
}
// Allocate memory to store the beamformed scanline
float *createScanline(int numPixel)
{
    float* scanline = new float[numPixel]; // create dynamic array to store scanline amplitudes
    return scanline;
}
// Beamform the A-mode scanline
void beamform(float *scanline, float **realRFData, float **imagRFData, float *scanlinePosition, float *elementPosition, int numElement, int numSample, int numPixel, float FS, float SoS)
{
    float* pReal = new float[numPixel]; // create array to store appropriate real value from realRFData
    float* pImag = new float[numPixel]; // create array to store appropriate imaginary value from imagRFData
    for(int i =0;i<numPixel;i++){
        float tforward = scanlinePosition[i]/SoS; // calculate tforward using given formula
        pReal[i] =0; // define current index of pReal and pImag
        pImag[i] = 0;
        for(int k =0;k<numElement;k++){ // loop through all elements of realRFData and imagRFData
            float tbackward = sqrt(pow(scanlinePosition[i],2)+pow(elementPosition[k],2))/(SoS); // calculate tbackward using given formula
            pReal[i] += realRFData[k][(int)floor((tforward+tbackward)*FS)]; // calculate and change current index of pReal
            pImag[i] += imagRFData[k][(int)floor((tforward+tbackward)*FS)]; // calculate and change current index of pImag
        }
        scanline[i] = sqrt(pow(pReal[i],2)+pow(pImag[i],2)); // save amplitude to current scanline index
    }
    delete[] pReal; // delete dynamically allocated arrays to avoid memory issues
    delete[] pImag;
}
// Write the scanline to a csv file
int outputScanline(const char *fileName, float *scanlinePosition, float *scanline, int numPixel)
{
    ofstream outFile;
    outFile.open(fileName); // open file 'fileName'
    if(outFile.fail()){ // error statement if outfile cant be accessed
        return -1;
    }
    else{
        for(int i =0;i<numPixel;i++){ // write to outfile
            outFile<<scanlinePosition[i]<<","<<scanline[i]<<endl; // writes the scanline postion and calculated magnitude of amplitude
        }
    }
    outFile.close(); // close file
    return 0;
}
// Destroy all the allocated memory
void destroyAllArrays(float *scanline, float **realRFData, float **imagRFData, float *scanlinePosition, float *elementPosition, int numElement, int numSample, int numPixel)
{
    //delete scanline, realRFData, imagRFData, scanlinePosition, elementPosition
    for(int i =0;i< numElement;i++){ // loop through 2D arrays to delete each dynamically allocated array
        delete[] realRFData[i];
        delete[] imagRFData[i];
    }
    //delete all 1D arrays
    delete[] realRFData;
    delete[] imagRFData;
    delete[] scanline;
    delete[] scanlinePosition;
    delete[] elementPosition;
}