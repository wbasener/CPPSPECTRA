#include "pch.h"
#include "hsi_data_reader.h"
#include "spectral.h"
#include "settings.h"
#include "write_log_file.h"
#include "atm_comp.h"
#include "material_id.h"

using namespace hsi;
using hsi::HSIData;
using hsi::HSIDataOptions;
using hsi::HSIDataReader;


int main(int argc, char** argv) {
  // Start timeing
  auto t_start = std::chrono::high_resolution_clock::now();
  timeCheck timeTracking;

  // Get the file names for the image and header
  // from user input or use a default
  const std::string hsi_data_path = getDataFname(argc, argv);
  const std::string hsi_header_path = getHeaderFname(argc, argv);

  // Initialize the settings and log file
  settingsValues settings = createSettings(argv, hsi_data_path);
  Logger_initialize(settings);
  PyPrint_initialize(settings);

  // Set parameters for Eigen computations
  Logger("Number of Eigen Threads: "+std::to_string(Eigen::nbThreads()), settings);
  Eigen::initParallel();

  // Determine the metadata from the header file
  timeTracking = timeCheckStart("Reading header", settings);
  HSIDataOptions data_options(hsi_data_path);
  data_options.ReadHeaderFromFile(hsi_header_path);
  data_options = formatHeaderData(data_options);
  hsi::HSIDataRange data_range = getDataRange(data_options);
  LoggerPrintHeaderInfo(data_options, settings);
  timeCheckEnd(timeTracking, settings);
  
  // Read the image data.
  timeTracking = timeCheckStart("Reading image", settings);
  HSIDataReader reader(data_options);  
  reader.ReadData(data_range);
  const hsi::HSIData& hsi_data = reader.GetData();
  timeCheckEnd(timeTracking, settings);
  
  // Create eigen array of image from the data. 
  timeTracking = timeCheckStart("Converting HSIDATA image data to Eigen matrix", settings);
  ImData Im = HSIData2EigenData(hsi_data, data_options, settings);  
  timeCheckEnd(timeTracking, settings);

  // Atmospheric Compensation
  Im = atmComp(Im, settings);
  
  // Display and.or save the image if desired
  timeTracking = timeCheckStart("Compute the RGB visual image", settings); 
  showRGBimage(Im, settings);
  timeCheckEnd(timeTracking, settings);

  // Computing the covariance (and outputting computation time)
  Logger("Computing statistics.", settings);
  ImStats stats = computeAnomalyCleanedImageStats(Im, settings);
  //ImStats stats = computeImageStats(Im, settings);

  // Whiten the image
  timeTracking = timeCheckStart("Whitening the image with RX-cleaned statistics", settings); 
  Im.white2d = computeWhitenedImage(Im, stats, settings);
  timeCheckEnd(timeTracking, settings);

  // Compute the RX anomaly image
  timeTracking = timeCheckStart("Compute and save the RX anomaly image", settings); 
  Im.RX = computeRXimage(Im, settings);
  // Display the image if desired
  showSaveRXimage(Im, settings);
  timeCheckEnd(timeTracking, settings);
  
  // Get the file names for the target library and header
  // from user input or use a default
  timeTracking = timeCheckStart("Read spectral libary", settings); 
  const std::string tgt_lib_data_path = getTgtLibDataFname(hsi_data_path);
  const std::string tgt_lib_header_path = getTgtLibHeaderFname(hsi_data_path);
  Logger("Target library file: "+tgt_lib_data_path,settings, 0);

  // Determine the metadata from the header file
  HSIDataOptions tgt_lib_data_options(tgt_lib_data_path);
  tgt_lib_data_options.ReadHeaderFromFile(tgt_lib_header_path);
  hsi::HSIDataRange tgt_lib_data_range = getDataRange(tgt_lib_data_options);

  // Read the library data.
  Logger("Reading detection Spectral library.", settings);
  HSIDataReader tgt_lib_reader(tgt_lib_data_options);  
  tgt_lib_reader.ReadData(tgt_lib_data_range);
  const hsi::HSIData& tgt_lib_data = tgt_lib_reader.GetData();
  LoggerPrintHeaderInfo(tgt_lib_data_options, settings);
  timeCheckEnd(timeTracking, settings);

  // Create eigen array of library from the data. 
  timeTracking = timeCheckStart("Converting library data to Eigen data", settings); 
  LibData lib_tgt_fullres = SpecLibData2EigenData(tgt_lib_data, tgt_lib_data_options, settings);
  timeCheckEnd(timeTracking, settings);

  // Resample the library. 
  timeTracking = timeCheckStart("Resampling library to image wavelengths", settings); 
  LibData lib_tgt = resampleLibrary(lib_tgt_fullres, Im, settings);
  timeCheckEnd(timeTracking, settings);

  // Whiten the library. 
  timeTracking = timeCheckStart("Whiten the library", settings);
  lib_tgt.spectraWhite = computeWhitenedLibrary(lib_tgt, stats, settings);
  timeCheckEnd(timeTracking, settings);

  // Compute the ACE target detection image
  timeTracking = timeCheckStart("Compute ACE image", settings);
  Eigen::MatrixXf ACE = computeACEimage(Im, lib_tgt, settings);
  Eigen::MatrixXf ACEmax1d = computeACEmax(ACE, settings);
  timeCheckEnd(timeTracking, settings);

  // Output results to a subdirectory called \output
  timeTracking = timeCheckStart("Save ACE images", settings);
  showACEResults(Im, lib_tgt, ACE, settings);
  timeCheckEnd(timeTracking, settings);

  // Get the file names for the identificaiton library and header
  // from user input or use a default
  timeTracking = timeCheckStart("Read identification spectral libary", settings); 
  const std::string id_lib_data_path = getIdLibDataFname(hsi_data_path);
  const std::string id_lib_header_path = getIdLibHeaderFname(hsi_data_path);
  Logger("Identificaiton library file: "+id_lib_data_path,settings, 0);

  // Determine the metadata from the header file
  HSIDataOptions id_lib_data_options(id_lib_data_path);
  id_lib_data_options.ReadHeaderFromFile(id_lib_header_path);
  hsi::HSIDataRange id_lib_data_range = getDataRange(id_lib_data_options);

  // Read the library data.
  Logger("Reading Identification Spectral library.", settings);
  HSIDataReader id_lib_reader(id_lib_data_options);  
  id_lib_reader.ReadData(id_lib_data_range);
  const hsi::HSIData& id_lib_data = id_lib_reader.GetData();
  LoggerPrintHeaderInfo(id_lib_data_options, settings);
  timeCheckEnd(timeTracking, settings);

  // Create eigen array of library from the data. 
  timeTracking = timeCheckStart("Converting identificaiton library data to Eigen data", settings); 
  LibData lib_id_fullres = SpecLibData2EigenData(id_lib_data, id_lib_data_options, settings);
  timeCheckEnd(timeTracking, settings);

  // Resampling identificaiton library to image wavelengths. 
  timeTracking = timeCheckStart("Resampling identificaiton library to image wavelengths", settings); 
  LibData lib_id = resampleLibrary(lib_id_fullres, Im, settings);
  timeCheckEnd(timeTracking, settings);

  // Whiten identificaiton library. 
  timeTracking = timeCheckStart("Whiten the identificaiton library", settings);
  lib_id.spectraWhite = computeWhitenedLibrary(lib_id, stats, settings);
  timeCheckEnd(timeTracking, settings);  

  // Grow the ROIs
  timeTracking = timeCheckStart("Grow the ROIs", settings);
  std::vector<IDstruct> ID = growROIs(Im, ACEmax1d, settings);
  timeCheckEnd(timeTracking, settings);

  // Material identificaiton on the ROIs
  timeTracking = timeCheckStart("Identify the ROIs", settings);
  ID = identifyROIs(Im, stats, ID, lib_tgt, lib_id, ACEmax1d, settings);
  timeCheckEnd(timeTracking, settings);
  
  // Tally and output the total runtime
  auto t_end = std::chrono::high_resolution_clock::now();
  auto secondsElapsedWhite = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
  std::string total_time = std::to_string(secondsElapsedWhite.count()/1000.);
  Logger("Total Runtime: "+total_time+"s", settings);
  LoggerExecutionTimeTally(settings, total_time);

  return 0;
}
