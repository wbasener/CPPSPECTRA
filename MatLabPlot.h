#pragma once
#include "spectral.h"
#include "settings.h"
/*
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
*/
/*
Holds the controls for the MPlot funtoin
Required inputs: settings & fileName
Other inputs: xLabel, yLabel, title, labels, label, lineWidth, alpha, amount
*/
struct PlotArg {
  std::string fileName = "PlotName";  // Name of the file the output is in: string
  settingsValues settings;            // The settings struct for the output file name: settings 
  std::string xLabel = "Wavelength";           // The label on the x axsis: string
  std::string yLabel = " ";           // The label on the y axsis: string
  std::string title = " ";            // The title of the graph: string
  std::vector<std::string> labels;    // A list of the names for the lines plotted, must be the same size as number of funtoins, only for malti-line plots: vector of strings
  std::string label = " ";            // The lable for each line ploted followed by a number, only for malti-line plots: string
  int lineWidth = 2;                  // The with of each line ploted, 2 is the normal size: int
  float alpha = 1;                    // The level of opacity 0 to 1: float
  int amount = -1;                    // Number of random rows to be taken from the matrix, if not defined or bigger then posable then all lines will be plotted: int
};

// Creates a plot in an html file using javascript
// x = vector, y = matrix (or vector), controls = PlotArg
void MPlot(Eigen::VectorXf,Eigen::MatrixXf, PlotArg);