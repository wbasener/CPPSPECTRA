#pragma once
/*
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
*/

struct settingsValues {
  int verbose;
  std::string imagePath;
  std::string loggerPath;
  std::string outDir;
  bool displayRGB;
  bool saveRGB;
  bool displayRX;
  bool saveRX;
  bool displayACE;
  bool saveACE;
  bool atmComp;
  float aceThresh;
  int minROIsize;
  long int nPCs;
  int localWindowRadius;
};

// Create the settings (verbose value, logger filename)
// Verbose priority meaning: 
//    -1 = logger only, nothing to command window
//    0 = logger only, (print image and log filenames only in command window)
//    1 = logger with data, (print image and log filenames only in command window)
//    2 = logger logger with data and command window output, 
//    3 = all output
settingsValues createSettings(char**, const std::string);