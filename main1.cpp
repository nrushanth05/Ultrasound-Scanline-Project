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
            //cout<<RFData[i][j];
        }
    }

    return RFData;   
}

int loadRFData(float **RFData, const char *fileName, int numElement, int numSample)
{
    // Open the text file fileName, read the data and store into RFData
    // You can use the getline() command to extract the data lines from the txt files
    ifstream inFile;
    inFile.open(fileName);
    if(!inFile.fail()){
        for(int i =0;i<numElement;i++){
            for(int j =0;j<numSample;j++){
                inFile>>RFData[i][j];
                //cout<<RFData[i][j];
            }    
        }
    }
    else{
        //cout <<"File cannot be opened"<<endl;
        return -1;
    }
    return 0;
}


// Create an array containing the depth location (in z-direction) for each pixel on the scanline
float *genScanlineLocation(int &numPixel)
{
    float depth;
    cout<<"How deep should the scanline be: "<<endl;
    cin>>depth;
    cout<<"Enter number of pixels: "<<endl;
    cin>>numPixel;
    float* scanlineLocation = new float[numPixel];
    float a = 0;
    for(int i =0;i<numPixel;i++){
        scanlineLocation[i]=a;
        a+=depth/(numPixel-1);
        //cout<<a<<endl;
    }
    //cout<<"genScanline done"<<endl;
    return scanlineLocation;
}

// Create an array containing the element location (in x-direction) of the ultrasound transducer
float *genElementLocation(int numElement, float PITCH)
{
    float* eleLocation= new float[numElement];
    for(int i =0;i<numElement;i++){
        eleLocation[i]=(i-(numElement-1)/2)*PITCH;
        //cout<<eleLocation[i]<<endl;
    }
    //cout<<"genElementlocation done"<<endl;
    return eleLocation;
}
// Allocate memory to store the beamformed scanline
float *createScanline(int numPixel)
{
    float* scanline = new float[numPixel];
    //cout<<"scanline created"<<endl;
    return scanline;
    
}

// Beamform the A-mode scanline
void beamform(float *scanline, float **realRFData, float **imagRFData, float *scanlinePosition, float *elementPosition, int numElement, int numSample, int numPixel, float FS, float SoS)
{
    //float** s = new float*[numPixel];
    float pReal[numPixel];
    float pImag[numPixel];
    //cout<<"beamform done1"<<endl;
    //cout<<numPixel<<endl; // 256
    //cout<<numElement<<endl; //128
    for(int i =0;i<numPixel;i++){
        float tforward = scanlinePosition[i]/SoS;
        //s[i] = new float[numElement];
        pReal[i] =0;
        pImag[i] = 0;
        //cout<<i<<endl;
        for(int k =0;k<numElement;k++){
            float tbackward = (sqrt(pow(scanlinePosition[i],2)+pow(elementPosition[k],2)))/(SoS);
            //s[i][k]= floor((tforward+tbackward)*FS);
            pReal[i] += realRFData[k][(int)floor((tforward+tbackward)*FS)];
            pImag[i] += imagRFData[k][(int)floor((tforward+tbackward)*FS)];
        }
        scanline[i] = sqrt(pow(pReal[i],2)+pow(pImag[i],2));
        //cout<<scanline[i]<<endl;
    }
}

// Write the scanline to a csv file
int outputScanline(const char *fileName, float *scanlinePosition, float *scanline, int numPixel)
{
    ofstream csvFile;
    csvFile.open(fileName);
    if(csvFile.fail()){
        return -1;
    }
    else{
        for(int i =0;i<numPixel;i++){
            csvFile<<scanlinePosition[i]<<","<<scanline[i]<<endl;
        }
    }
    csvFile.close();
    //cout<<"csv written"<<endl;
    return 0;
}
// Destroy all the allocated memory
void destroyAllArrays(float *scanline, float **realRFData, float **imagRFData, float *scanlinePosition, float *elementPosition, int numElement, int numSample, int numPixel)
{
    //delete scanline, realRFData, imagRFData, scanlinePosition, elementPosition
    for(int i =0;i< numElement;i++){
        delete[] realRFData[i];
        delete[] imagRFData[i];
    }
    delete[] realRFData;
    delete[] imagRFData;
    delete[] scanline;
    delete[] scanlinePosition;
    delete[] elementPosition;
    //cout<<"arrays deleted"<<endl;
}

int main () {

    //This section initializes the constant values you will need
    const int N = 128; //Number of channels 
    const int K = 3338; //Number of Samples
    const float PITCH = 3.048e-4; //This is the transducer pitch
    const float SOS = 1540; //Speed of sound variable
    const float FS = 40e6; // Sampling frequency variable
    const float TS = 1.0f/FS; //Sampling time variable

    //Use this section to create your array of pointers, and the arrays the pointers point to.
    float **realRFData = createDataMatrix(N, K);
    float **imagRFData = createDataMatrix(N, K);

    int result = 0;

    //Use this section to input the data from the txt files. 
    result = loadRFData(realRFData, "RealRFData_mystery.txt", N, K);
    if (result == -1)
    {
        cerr << "Cannot load from realRFData.txt, exiting program" << endl;
        return -1;
    }
    result = loadRFData(imagRFData, "ImagRFData_mystery.txt",N, K);
    if (result == -1)
    {
        cerr << "Cannot load from imagRFData.txt, exiting program" << endl;
        return -1;
    }

    //Create your imaging location array here
    int numPixel = 256;
    float *scanlinePosition = genScanlineLocation(numPixel);

    //Create you element locations here
    float* elementPosition; 
    elementPosition = genElementLocation(N,PITCH);

    //Perform your A-mode beamforming here
    float *scanline;
    scanline = createScanline(numPixel);
    beamform(scanline, realRFData, imagRFData, scanlinePosition, elementPosition, N, K, numPixel, FS, SOS);

    //Use this section to output your data into a .csv file. Make sure to seperate columns with commas and
    //use new lines to separate the rows.
    result = outputScanline("output_mystery.csv", scanlinePosition, scanline, numPixel);
    if (result == -1)
    {
        cerr << "Cannot write scanline to output.csv, exiting program" << endl;
        return -3;
    }

    destroyAllArrays(scanline, realRFData, imagRFData, scanlinePosition, elementPosition, N, K, numPixel);

    return 0;
}

