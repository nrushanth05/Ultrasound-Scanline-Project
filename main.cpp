/*  
    Name: Nrushanth Suthaharan
    Student ID: 21070183
    Date: 2023-11-28
    Description: Generate a graph using input files, and user input to represent the scanline which is produced by an ultrasound
*/
#include <iostream>
#include "Amode.h"

using namespace std;


int main () {

    //This section initializes the constant values you will need
    const int N = 128; //Number of channels 
    const int K = 3338; //Number of Samples
    const float PITCH = 3.048e-4; //This is the transducer pitch
    const float SOS = 1540; //Speed of sound variable
    const float FS = 40e6; // Sampling frequency variable
    const float TS = 1.0f/FS; //Sampling time variable

    //Use this section to create your array of pointers, and the arrays the pointers point to.
    float **realRFData = createDataMatrix(N, K); // call function to create 2d array for real data
    float **imagRFData = createDataMatrix(N, K); // call function to create 2d array for imaginary data

    int result = 0;

    //Use this section to input the data from the txt files. 
    result = loadRFData(realRFData, "RealRFData_mystery.txt", N, K); //import data from file 'RealRFData.txt'
    if (result == -1)
    {
        cerr << "Cannot load from RealRFData_mystery.txt, exiting program" << endl; // error message, end program
        return -1;
    }
    result = loadRFData(imagRFData, "ImagRFData_mystery.txt",N, K); // import data from file 'ImagRFData.txt'
    if (result == -1)
    {
        cerr << "Cannot load from ImagRFData_mystery.txt, exiting program" << endl; // error message, end program
        return -1;
    }

    //Create your imaging location array here
    int numPixel = 256;
    float *scanlinePosition = genScanlineLocation(numPixel); // call function to create an array for scanline positions (x-direction)

    //Create you element locations here
    float* elementPosition; 
    elementPosition = genElementLocation(N,PITCH); // call funciton to create an array for element positions (z-directions)

    //Perform your A-mode beamforming here
    float *scanline;
    scanline = createScanline(numPixel); // call function to create scanline
    beamform(scanline, realRFData, imagRFData, scanlinePosition, elementPosition, N, K, numPixel, FS, SOS); // call function to apply beamforming

    //Use this section to output your data into a .csv file. Make sure to seperate columns with commas and
    //use new lines to separate the rows.
    result = outputScanline("output_mystery.csv", scanlinePosition, scanline, numPixel); // save the beamformed data to file 'output.csv'
    if (result == -1)
    {
        cerr << "Cannot write scanline to output.csv, exiting program" << endl; // error message, end program
        return -3;
    }

    destroyAllArrays(scanline, realRFData, imagRFData, scanlinePosition, elementPosition, N, K, numPixel); // call function to destroy all arrays

    return 0;
}