#pragma once
#include "settings.h"
#include "hsi_data_reader.h"
#include <Eigen/Core>
#include <Eigen/Dense>

/*
#include <Eigen/Dense>
#include <iostream>
#include <fstream>
#include <string> 
*/

// Get the current date and time in nice format
std::string getCurrentDateTime(std::string);

// Output a log message for a given image being processed
// overloaded function:
//   if no int is provided for the priority then the level is assumed to be 0
void Logger(std::string, settingsValues, int);
void Logger(std::string, settingsValues);

// Print some sample data from an array (overloaded function)
void LoggerPrintDataSample(Eigen::MatrixXf, std::string, settingsValues);
void LoggerPrintDataSample(Eigen::MatrixXd, std::string, settingsValues);

// Print header information to log file and/or command window
void LoggerPrintHeaderInfo(hsi::HSIDataOptions, settingsValues);

// Create empty log file for a given image being processed
void Logger_initialize(settingsValues);

// Writes an HTML file with a barchart of execution times
void LoggerExecutionTimeTally(settingsValues, std::string);


// Initialize a Python-readable file to write variabels for debugging.
void PyPrint_initialize(settingsValues);
// PyPrint will print a variable of multiple variable types to a Python-reabable file.
// Supported variable types are: int, float, double,  
// std::vector<int>, std::vector<float>, std::vector<double>, std::vector<std::string>
// Eigen::VectorXf,  Eigen::MatrixXf, Eigen::ArrayXf, and Eigen::ArrayXi. 
// Example USage:
// PyPrint_initialize(settings);
// PyPrint(a, "a", settings);
// PyPrint(X, "X", settings);
void PyPrint(int, std::string, settingsValues);
void PyPrint(float, std::string, settingsValues);
void PyPrint(double, std::string, settingsValues);
void PyPrint(Eigen::Index, std::string, settingsValues);
void PyPrint(Eigen::VectorXf, std::string, settingsValues);
void PyPrint(Eigen::MatrixXf, std::string, settingsValues);
void PyPrint(Eigen::ArrayXf, std::string, settingsValues);
void PyPrint(Eigen::ArrayXi, std::string, settingsValues);
void PyPrint(std::vector<int>, std::string, settingsValues);
void PyPrint(std::vector<float>, std::string, settingsValues);
void PyPrint(std::vector<double>, std::string, settingsValues);
void PyPrint(std::vector<std::string>, std::string, settingsValues);