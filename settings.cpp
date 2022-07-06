#include "pch.h"
#include "settings.h"



// Function definition
settingsValues createSettings(char** argv,const std::string hsi_data_path) {
  
  // Default settings
  settingsValues settings;
  settings.verbose = 0;
  settings.imagePath = hsi_data_path; 
  settings.loggerPath = std::filesystem::path(hsi_data_path).replace_filename("output").string()+"\\log.txt";
  settings.outDir = std::filesystem::path(hsi_data_path).replace_filename("output").string();
  settings.displayRGB = false;
  settings.saveRGB = false;
  settings.displayRX = false;
  settings.saveRX = true;
  settings.displayACE = false;
  settings.saveACE = true;
  settings.atmComp = false;
  settings.aceThresh = 0.5;
  settings.minROIsize = 2;
  settings.nPCs = -1;
  settings.localWindowRadius = 25;

  // Read the config file (ninja_config.csv) and use any settings from there
  std::string configPath = std::filesystem::path(argv[0]).replace_filename("ninja_config.csv").string();
  std::string key, val;
  std::ifstream fileIn;
  fileIn.open(configPath);
  while(fileIn.good()){
    std::getline(fileIn, key, ','); // get the frist string, from beginning of line to the comma.
    std::getline(fileIn, val, '\n') ; // get the second string, between first comma and end of line.
    // If these config has a setting for Verbose or verbose, set the value
    if (key.find("erbose") != std::string::npos){
      settings.verbose = std::stoi(val);
    }
    if (key.find("displayRGB") != std::string::npos){
      settings.displayRGB = std::stoi(val);
    }
    if (key.find("saveRGB") != std::string::npos){
      settings.saveRGB = std::stoi(val);
    }
    if (key.find("displayRX") != std::string::npos){
      settings.displayRX = std::stoi(val);
    }
    if (key.find("saveRX") != std::string::npos){
      settings.saveRX = std::stoi(val);
    }
    if (key.find("displayACE") != std::string::npos){
      settings.displayACE = std::stoi(val);
    }
    if (key.find("saveACE") != std::string::npos){
      settings.saveACE = std::stoi(val);
    }
    if (key.find("aceThresh") != std::string::npos){
      settings.aceThresh = std::stof(val);
    }
    if (key.find("minROIsize") != std::string::npos){
      settings.minROIsize = std::stoi(val);
    }
    if (key.find("nPCs") != std::string::npos){
      settings.nPCs = std::stol(val);
    }
  }  

  std::filesystem::create_directory(settings.outDir);

  return settings;
}


