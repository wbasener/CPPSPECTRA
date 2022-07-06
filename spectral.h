#pragma once
#include "hsi_data_reader.h" 
#include "settings.h"
#include "write_log_file.h"
#include <Eigen/Core>
#include <Eigen/Dense>
/*
#include "opencv2/highgui.hpp"
#include <opencv2/opencv.hpp> 
#include <vector>
#include <iostream>
#include <sstream>
#include <ctype.h>
#include <cstdlib>
#include <unordered_map>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <string>
#include <chrono>
#include <cmath>
#include <filesystem>
//*/

// Structure to hold the image data and metadata
struct ImData {
  Eigen::MatrixXf im2d;       // The hypersepctral image (pixels x bands)
  Eigen::MatrixXf white2d;    // The hyperspectral image, whitened (pixels x bands)
  cv::Mat3b RGB_image_8UC3;   // The rgb image (Unsigned 8-bit, 0-255) for creagint jpgs
  Eigen::MatrixXf RX;         // RX anomaly detection result
  Eigen::VectorXf fwhm;       // Full width at half-max, will be [-1] if no valid fwhm was present in header
  Eigen::VectorXf wl;         // Wavelengths
  std::string interleave = "";// interleave type
  int wlScale;                // 1 if wl is in nm, 1000 if wl is in micrometers
  int rows;                   // Number of rows
  int cols;                   // Number of cols
  int bands;                  // Number of bands
  int bandRed = 0;
  int bandGreen = 0;
  int bandBlue = 0;
  int bandNDMI2 = 0;
  int bandNIR = 0;
  int bandNDMI1 = 0;
};

struct RGBwavelengthsStruct {
  int bandNDMI2 = 0;
  int bandNIR = 0;
  int bandNDMI1 = 0;
  int bandRed = 0;
  int bandGreen = 0;
  int bandBlue = 0;
  std::string type = "RGB";
  int hasNDVI = 1;
  int hasNDMI = 1;
};

struct timeCheck {
    std::string functionName; // Name of the function being times
    std::chrono::time_point<std::chrono::high_resolution_clock> timeIn; // Clock time at start of interval
};

// Structure to hold the spectral library data and metadata
struct LibData {
  Eigen::MatrixXf spectra;
  Eigen::MatrixXf spectraWhite;
  Eigen::VectorXf wl;
  std::vector<std::string> specNames;
  int nSpectra;
  int bands;
};

// Structure to hold the image statistics
struct ImStats {
  Eigen::MatrixXf cov;
  Eigen::VectorXf mean;
  Eigen::MatrixXf evals;
  Eigen::MatrixXf evecs;
  Eigen::MatrixXf W;
};

// Starts a time check interval for elapsed time of a funciton
// overloadrd function: will use timecheck struct if passed in, 
// or create a new one if only a string is passed in.
timeCheck timeCheckStart(timeCheck, std::string, settingsValues);
timeCheck timeCheckStart(std::string, settingsValues);

// Starts a time check interval for elapsed time of a funciton
void timeCheckEnd(timeCheck, settingsValues);

// Compute the pdf for the normal distribution
float normalPDF(float, float, float);

// Reshapes a 1-dimensional Eigen::Matrixf of an image to the 2d image
// respecting the data interleave order
Eigen::MatrixXf ImageResize1dto2d(Eigen::MatrixXf, int, int, std::string);

// Reshapes a 2-dimensional Eigen::Matrixf of an image to the 3d image
// respecting the data interleave order
Eigen::MatrixXf ImageResize2dto3d(Eigen::MatrixXf, int, int, std::string);

// Computes the threshold at the percentile (0-1) of the matrix.
float percentileValue(Eigen::MatrixXf, float);

// convert a comma seperate sting list of the form
// {first, second, third, ... , last}
// to a Eigen::VectorXf 
std::vector<std::string> stringCS2Vector(std::string);

// convert a comma seperate sting list of the form
// {first, second, third, ... , last}
// to a vector of floats
Eigen::VectorXf stringCS2VectorFloats(std::string, int);

// Compute the incides for the wavelengths for the RGB colors
RGBwavelengthsStruct computeRGBwavelengths(Eigen::VectorXf);

// Get the name of the data file based on user input
std::string getDataFname(int, char**);

// Get the name of the header file based on user input
std::string getHeaderFname(int, char**);

// Get the range of data (lines, columns, bands) to read
// Currently read the whole image, but subsets could be read here
hsi::HSIDataRange getDataRange(hsi::HSIDataOptions);

// format and cleanup header information
// convert all wl and fwhm units to nanometers
hsi::HSIDataOptions formatHeaderData(hsi::HSIDataOptions);

// Compute a covarnaince matrix for the input image
// Input image matrix is pixels x bands
ImData HSIData2EigenData(const hsi::HSIData&, hsi::HSIDataOptions, settingsValues);

// Show the RGB image
void showRGBimage(ImData, settingsValues);

// Compute a covarnaince matrix and other stats for the input image
// Input image matrix is pixels x bands
ImStats computeImageStats(ImData, settingsValues);

// Compute the whitened image
// The whitened image is the mean-subtracted image
// projected on the eignevectors scaled by eigenvalues
Eigen::MatrixXf computeWhitenedImage(ImData, ImStats, settingsValues);

// Compute the RX anomaly image
Eigen::MatrixXf computeRXimage(ImData, settingsValues);

// Show the RX image
void showSaveRXimage(ImData, settingsValues);

// Get the name of the spectral target library
const std::string getTgtLibDataFname(const std::string);

// Get the name of the spectral target library header
const std::string getTgtLibHeaderFname(const std::string);

// Get the name of the spectral id library
const std::string getIdLibDataFname(const std::string);

// Get the name of the spectral id library header
const std::string getIdLibHeaderFname(const std::string);

// Compute a structure to hold the spectral library information
LibData SpecLibData2EigenData(const hsi::HSIData&, hsi::HSIDataOptions, settingsValues);

// Resample library to a given set os wavelengths
LibData resampleLibrary(LibData, ImData, settingsValues);

// Compute the whitened spectral library
// The whitened library spectra are the mean-subtracted spectra
// projected on the eignevectors scaled by eigenvalues
Eigen::MatrixXf computeWhitenedLibrary(LibData, ImStats, settingsValues);

// Compute the ACE target detection image
Eigen::MatrixXf computeACEimage(ImData, LibData, settingsValues);
Eigen::MatrixXf computeACEmax(Eigen::MatrixXf, settingsValues);

//  Outputs results to a folder
void showACEResults(ImData, LibData, Eigen::MatrixXf, settingsValues);

// Compute a covarnaince matrix and other stats for the input image
// using a 3 step process: compute stats and RX anomaly detection,
// remove the anomalies, and recompute stats from non-anomalous pixels.
// Input image matrix is pixels x bands
ImStats computeAnomalyCleanedImageStats(ImData, settingsValues);


float FinCov(Eigen::VectorXf, Eigen::VectorXf);
