
#include "pch.h"
#include "spectral.h"

using hsi::HSIData;
using hsi::HSIDataOptions;
using hsi::HSIDataReader;


timeCheck timeCheckStart(timeCheck timeCheckIn, std::string functionName, settingsValues settings){
  Logger(functionName+"...", settings);
  timeCheckIn.functionName = functionName; // Name of the function being times
  timeCheckIn.timeIn = std::chrono::high_resolution_clock::now(); // Clock time at start of interval
  return timeCheckIn;
}

timeCheck timeCheckStart(std::string functionName, settingsValues settings){
  Logger(functionName+"...", settings);
  timeCheck timeCheckIn;
  timeCheckIn.functionName = functionName; // Name of the function being times
  timeCheckIn.timeIn = std::chrono::high_resolution_clock::now(); // Clock time at start of interval
  return timeCheckIn;
}

void timeCheckEnd(timeCheck timeCheckIn, settingsValues settings){
  auto timeEnd = std::chrono::high_resolution_clock::now();
  auto secondsElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeCheckIn.timeIn);
  Logger(timeCheckIn.functionName+" completed in "+std::to_string(secondsElapsed.count()/1000.)+"s", 
    settings); 
}

float normalPDF(float s, float x, float m){
  return (1/(s*2.50663))*std::pow(2.71828,-0.5*std::pow((x-m)/s,2.0));
}

Eigen::MatrixXf ImageResize1dto2d(Eigen::MatrixXf Im1d, int rows, int cols, std::string interleave){
  Eigen::MatrixXf Im2d;
  if (interleave == "bip"){
    Im2d = Im1d.reshaped<Eigen::RowMajor>(rows, cols);
  } else{
    Im2d = Im1d.reshaped(rows, cols);
  }
  return Im2d;
}

float percentileValue(Eigen::MatrixXf M, float pct){
  if (pct > 1){
    std::cout << "WARNING: Percentile called with pct > 1.  Setting pct=1." << "\n";
  }
  if (pct < 0){
    std::cout << "WARNING: Percentile called with pct < 0.  Setting pct=0." << "\n";
  }
  std::vector<float> Mv(M.size());
  Eigen::VectorXf::Map(&Mv[0], M.size()) = M;  
  std::sort(Mv.begin(), Mv.end());
  int idx_pct = std::round(float(M.size())*pct);
  float thresh = Mv[idx_pct];
  return thresh;
}

std::vector<std::string> stringCS2Vector(std::string str){
  // convert a comma seperate sting list of the form
  // [first, second, third, ... , last]
  // to a vector of strings
  str.erase(std::remove(str.begin(), str.end(), '}'), str.end());
  str.erase(std::remove(str.begin(), str.end(), '{'), str.end());
  str.erase(std::remove(str.begin(), str.end(), ']'), str.end());
  str.erase(std::remove(str.begin(), str.end(), '['), str.end());
  std::vector<std::string> result;
  std::stringstream ss(str);
  while (ss.good()){
    std::string substr;
    std::getline(ss, substr, ',');
    result.push_back(substr);
  }
  return result;
}

Eigen::VectorXf stringCS2VectorFloats(std::string str, int len){
  // convert a comma seperate sting list of the form
  // {first, second, third, ... , last}
  // to a Eigen::VectorXf 
  Eigen::VectorXf result(len);
  str.erase(std::remove(str.begin(), str.end(), '}'), str.end());
  str.erase(std::remove(str.begin(), str.end(), '{'), str.end());
  str.erase(std::remove(str.begin(), str.end(), ']'), str.end());
  str.erase(std::remove(str.begin(), str.end(), '['), str.end());
  std::stringstream ss(str);
  int i = 0;
  while (ss.good()){
    std::string substr;
    std::getline(ss, substr, ',');
    float num_float = std::stof(substr);
    result(i) = num_float;
    i++;
  }
  return result;
}

RGBwavelengthsStruct computeRGBwavelengths(Eigen::VectorXf wl){
  // Compute the incides for the wavelengths for the RGB colors
  RGBwavelengthsStruct RGBwavelengths;
  float NDMI2DiffWL = abs(wl(0)-990);
  float NIRDiffWL = abs(wl(0)-850);
  float NDMI1DiffWL = abs(wl(0)-795);
  float redDiffWL = abs(wl(0)-650);
  float greenDiffWL = abs(wl(0)-550);
  float blueDiffWL = abs(wl(0)-450);
  for (int band_idx = 0; band_idx < wl.size(); ++band_idx){
    if (abs(wl(band_idx)-990) < NDMI2DiffWL){
      NDMI2DiffWL = abs(wl(band_idx)-990);
      RGBwavelengths.bandNDMI2 = band_idx;
    }
    if (abs(wl(band_idx)-850) < NIRDiffWL){
      NIRDiffWL = abs(wl(band_idx)-850);
      RGBwavelengths.bandNIR = band_idx;
    }
    if (abs(wl(band_idx)-795) < NDMI1DiffWL){
      NDMI1DiffWL = abs(wl(band_idx)-795);
      RGBwavelengths.bandNDMI1 = band_idx;
    }
    if (abs(wl(band_idx)-650) < redDiffWL){
      redDiffWL = abs(wl(band_idx)-650);
      RGBwavelengths.bandRed = band_idx;
    }
    if (abs(wl(band_idx)-550) < greenDiffWL){
      greenDiffWL = abs(wl(band_idx)-550);
      RGBwavelengths.bandGreen = band_idx;
    }
    if (abs(wl(band_idx)-450) < blueDiffWL){
      blueDiffWL = abs(wl(band_idx)-450);
      RGBwavelengths.bandBlue = band_idx;
    }
  }
  if (RGBwavelengths.bandRed == 0){
    //  There were not distinct bands for Red, Gree Blue
    // Selecting arbitrary bands across wavelength range
    RGBwavelengths.type = "FalseColor";
    RGBwavelengths.bandRed = round(3*wl.size()/4);
    RGBwavelengths.bandGreen = round(2*wl.size()/4);
    RGBwavelengths.bandBlue = round(wl.size()/4);
  }
  if ((NIRDiffWL > 50) || (redDiffWL > 50) ){
    RGBwavelengths.hasNDVI = 0;
  }
  if ((NDMI1DiffWL > 50) || (NDMI2DiffWL > 50) ){
    RGBwavelengths.hasNDMI = 0;
  }
  return RGBwavelengths;
}

hsi::HSIDataOptions formatHeaderData(hsi::HSIDataOptions data_options){
  // modify data from header here if needed
  return data_options;
}

std::string getDataFname(int argc, char** argv){
  // Set paths to image files.
  if (argc == 1) {
    // if there are no input file names then use the default image
    // AVIRS which should be located in the same directory as the executable file.
    return std::filesystem::path(argv[0]).replace_filename("AVIRIS").string();
  }else {
    // input image and header file names
    return argv[1];
  }
}

std::string getHeaderFname(int argc, char** argv){
  // Set paths to image files.
  if (argc == 1) {
    // if there are no input file names then use the default
    return std::filesystem::path(argv[0]).replace_filename("AVIRIS.hdr").string();;
  }else if (argc == 2) {
    // case where file name is image and header is just the image with .hdr added
    return strcat(argv[1],".hdr");
  } else {
    // input image and header file names
    return argv[2];
  }
}

// Get the range of data (lines, columns, bands) to read
// Currently read the whole image, but subsets could be read here
hsi::HSIDataRange getDataRange(hsi::HSIDataOptions data_options){
  // Set range of data we want to read.
  hsi::HSIDataRange data_range;
  data_range.start_row = 0;
  data_range.end_row = data_options.num_data_rows;
  data_range.start_col = 0;
  data_range.end_col = data_options.num_data_cols;
  data_range.start_band = 0;
  data_range.end_band = data_options.num_data_bands;
  return data_range;
}

ImData HSIData2EigenData(const hsi::HSIData& hsi_data, hsi::HSIDataOptions data_options, settingsValues settings){
  // Create the structure to hold the image and metadata
  ImData Im;

  std::string interleave_string[3] = {"bsq", "bil", "bip"};
  Im.interleave = interleave_string[data_options.interleave_format];
  Im.rows = hsi_data.num_rows;
  Im.cols = hsi_data.num_cols;
  Im.bands = hsi_data.num_bands;
  Eigen::MatrixXf im2d(Im.rows * Im.cols, Im.bands);
  if (hsi_data.data_type == hsi::HSI_DATA_TYPE_DOUBLE){
    for (int band = 0; band < Im.bands; ++band) {
      for (int row = 0; row < Im.rows; ++row) {
        for (int col = 0; col < Im.cols; ++col) {
          im2d(row * Im.cols + col, band) = float(hsi_data.GetValue(row, col, band).value_as_double);
        }
      }
    }
  }
  if (hsi_data.data_type == hsi::HSI_DATA_TYPE_FLOAT){
    for (int band = 0; band < Im.bands; ++band) {
      for (int row = 0; row < Im.rows; ++row) {
        for (int col = 0; col < Im.cols; ++col) {
          im2d(row * Im.cols + col, band) = hsi_data.GetValue(row, col, band).value_as_float;
        }
      }
    }
  }
  if (hsi_data.data_type == hsi::HSI_DATA_TYPE_INT16){
    for (int band = 0; band < Im.bands; ++band) {
      for (int row = 0; row < Im.rows; ++row) {
        for (int col = 0; col < Im.cols; ++col) {
          im2d(row * Im.cols + col, band) = (float)(hsi_data.GetValue(row, col, band).value_as_int16)/10000.0;
        }
      }
    }
  }
  Im.im2d = im2d;

  // This creates the Eigen::VectorXf Im.wl from the wavelength string
  Im.wl = stringCS2VectorFloats(data_options.wavelength, Im.bands);

  // Im.wlScale is a scale factor on the wavelengths
  // multiplying by this factor puts the wavelengths
  // into units of nanometers
  Im.wlScale = 1;
  if (Im.wl.mean() < 10){
    Im.wlScale = 1000;
  }
  Im.wl = Im.wl*Im.wlScale;
  std::string msgWL = "Wavelengths: ";
  for (int band = 0; band < std::min(5, Im.bands); ++band) {
    msgWL = msgWL + std::to_string(Im.wl[band]) + ", ";
  }
  msgWL = msgWL +  "...";
  Logger(msgWL, settings, 1);

  // Compute the indices for the wavelengths associated with 
  // Reg, Green, Blue colors (650nm, 550nm, 450nm)
  RGBwavelengthsStruct RGBwavelengths = computeRGBwavelengths(Im.wl);
  Im.bandRed = RGBwavelengths.bandRed;
  Im.bandGreen = RGBwavelengths.bandGreen;
  Im.bandBlue = RGBwavelengths.bandBlue;
  if (RGBwavelengths.type == "RGB"){
    Logger("RGB wavelengths: ["+std::to_string(Im.wl[Im.bandRed])+","+
      std::to_string(Im.wl[Im.bandGreen])+","+
      std::to_string(Im.wl[Im.bandBlue])+"]", 
      settings);
  } else
  {
    Logger("RGB wavelengths: ["+std::to_string(Im.wl[Im.bandRed])+","+
    std::to_string(Im.wl[Im.bandGreen])+","+
    std::to_string(Im.wl[Im.bandBlue])+"]"
      " (VIS bands not covered - using false color.)", settings);
  }
  
  // This creates the Eigen::VectorXf Im.fwhm from the fwhm string
  if (data_options.fwhm != "NA"){
    Im.fwhm = stringCS2VectorFloats(data_options.fwhm, Im.bands);
  } else {
    Logger("fwhm is not present - default values will be used.", settings);
    std::string fwhm_default = "{";
    fwhm_default += std::to_string(Im.wl[1]-Im.wl[0]);
    for (int band = 1; band <Im.bands-1; ++band) {
      fwhm_default += ", ";
      fwhm_default += std::to_string(
          std::min(Im.wl[band]-Im.wl[band-1], Im.wl[band+1]-Im.wl[band]) );
    }
    fwhm_default += std::to_string(Im.wl[Im.bands-1]-Im.wl[Im.bands-2]) + "}";
    Im.fwhm = stringCS2VectorFloats(fwhm_default, Im.bands);
  }
  std::string msgFWHM = "fwhm: ";
  for (int band = 0; band < std::min(5, Im.bands); ++band) {
    msgFWHM = msgFWHM + std::to_string(Im.fwhm[band]) + ", ";
  }
  msgFWHM = msgFWHM +  "...";
  Logger(msgFWHM, settings, 1);  

  LoggerPrintDataSample(im2d, "image", settings);
  return Im;
}

void showRGBimage(ImData Im, settingsValues settings){

  if (settings.saveRGB || settings.displayRGB){
    //  create RGB image
    int cols = Im.cols;
    int rows = Im.rows;;
    float min_value = 0;
    float max_value = 0;
    std::vector<int> ind{Im.bandRed,Im.bandGreen,Im.bandBlue};
    Eigen::VectorXf maxs = Im.im2d.colwise().maxCoeff();
    Eigen::VectorXf mins = Im.im2d.colwise().minCoeff();
    std::cout << FinCov(maxs,mins) << "\n";
    std::cout << "**********************************************\n";
    // Eigen::Index sampleIdx = Eigen::seq(0,Im.im2d.rows(),std::floor(Im.im2d.rows()/10000.));
    float maxR = percentileValue(Im.im2d(Eigen::seq(0,Im.im2d.rows(),std::floor(Im.im2d.rows()/10000.)),Im.bandRed), 0.98);
    float minR = percentileValue(Im.im2d(Eigen::seq(0,Im.im2d.rows(),std::floor(Im.im2d.rows()/10000.)),Im.bandRed), 0.02);
    float maxG = percentileValue(Im.im2d(Eigen::seq(0,Im.im2d.rows(),std::floor(Im.im2d.rows()/10000.)),Im.bandGreen), 0.98);
    float minG = percentileValue(Im.im2d(Eigen::seq(0,Im.im2d.rows(),std::floor(Im.im2d.rows()/10000.)),Im.bandGreen), 0.02);
    float maxB = percentileValue(Im.im2d(Eigen::seq(0,Im.im2d.rows(),std::floor(Im.im2d.rows()/10000.)),Im.bandBlue), 0.98);
    float minB = percentileValue(Im.im2d(Eigen::seq(0,Im.im2d.rows(),std::floor(Im.im2d.rows()/10000.)),Im.bandBlue), 0.02);
    cv::Mat RGB_image(rows, cols, CV_32FC3);
    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        int idx;
        if (Im.interleave == "bsq"){
          idx = row+col*rows;
        } else {
          idx = col+row*cols;
        } 
        float r = std::min(maxR,std::max(minR,Im.im2d(idx, Im.bandRed)));
        float g = std::min(maxG,std::max(minG,Im.im2d(idx, Im.bandGreen)));
        float b = std::min(maxB,std::max(minB,Im.im2d(idx, Im.bandBlue)));
        RGB_image.at<cv::Vec3f>(row, col) = cv::Vec3f(b, g, r);
      }
    }
    RGB_image = RGB_image - cv::Scalar(minB,minG,minR);
    RGB_image = RGB_image.mul(cv::Scalar(255.0/(maxB-minB),  255.0/(maxG-minG),  255.0/(maxR-minR)));
    cv::Mat3b RGB_image_8UC3;
    RGB_image.convertTo(RGB_image_8UC3, CV_8UC3);

    if (settings.displayRGB){
      cv::namedWindow("test", cv::WINDOW_AUTOSIZE);
      cv::imshow("test", 5*RGB_image);
      cv::waitKey(0);
    }

    if (settings.saveRGB){
      cv::Mat3b RGB_image_8UC3;
      RGB_image.convertTo(RGB_image_8UC3, CV_8UC3);
      bool check = cv::imwrite(settings.outDir+"\\RGB.jpg",RGB_image_8UC3);
      if (check == false) {
        Logger("WARNING Saving RGB image failed.", settings);
      } else {
        Logger("RGB Image saves as "+settings.outDir+"\\RGB.jpg", settings);
      }
    }
  }
}

// Computes statistics for an image
ImStats computeImageStats(ImData Im, settingsValues settings) {
  // Create the structure to hold all the stats
  timeCheck timeTracking;
  ImStats stats; 

  // Compute the image mean
  timeTracking = timeCheckStart("Mean and Covariance computation", settings); 
  stats.mean = Im.im2d.colwise().mean();

  // Compute the covariance matrix
  Eigen::MatrixXf centered = (Im.im2d.rowwise() - stats.mean.transpose()).template cast<float>();
  Eigen::MatrixXf cov = (centered.transpose() * centered) / float(Im.im2d.rows() - 1);
  timeCheckEnd(timeTracking, settings);

  // Compute the eigenvalues and eigenvectors
  timeTracking = timeCheckStart("Eigenvalue and eignvector computation", settings); 
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXf> eigensolver(cov);
  auto t_end_evalsEvecs_Eigin = std::chrono::high_resolution_clock::now();
  Eigen::MatrixXf evals;
  Eigen::MatrixXf evecs;
  if (eigensolver.info() == Eigen::Success){
    timeCheckEnd(timeTracking, settings);
    evals = eigensolver.eigenvalues();
    evecs = eigensolver.eigenvectors();
  }
  else {
    Logger("WARNING: eigenvalues and eigenvectors failed to compute.", settings);
  };

  std::string msgEvals = "Eigenvalues:\n: ";
  for (int i = 0; i < 3; ++i) {
    msgEvals = msgEvals + std::to_string(evals(i))+" | "+std::to_string(evals(i)/evals(evals.rows()-1))+"\n";
  }
  msgEvals = msgEvals + "...\n";
  for (int i = evals.rows()-3; i < evals.rows(); ++i) {
    msgEvals = msgEvals + std::to_string(evals(i))+" | "+std::to_string(evals(i)/evals(evals.rows()-1))+"\n";
  }
  Logger(msgEvals, settings, 1);  

  // Computing the whitening matrix
  timeTracking = timeCheckStart("Whitening matrix computation", settings); 
  Eigen::MatrixXf D = Eigen::MatrixXf::Identity(evals.rows(),evals.rows());
  float maxEval = evals.maxCoeff();
  for (int i = 0; i < evals.rows(); ++i) {
    // regularization on eigenvalues
    evals(i) = std::max(evals(i), float(maxEval*std::pow(10,-6)));
    D(i,i) = 1/sqrt(evals(i));
  }
  Eigen::MatrixXf W =  (evecs*D);
  LoggerPrintDataSample(W, "whitening matrix 1", settings);

  // Recast as float
  stats.cov = cov.template cast<float>();
  stats.evals = evals.template cast<float>();
  stats.evecs = evecs.template cast<float>();
  stats.W = W.template cast<float>();

  Logger("im2d Matrix is "+std::to_string(Im.im2d.rows())+"x"+std::to_string(Im.im2d.cols()),
    settings);
  Logger("Covariance is "+std::to_string(stats.cov.rows())+"x"+std::to_string(stats.cov.cols()),
    settings);  
  Logger("Whitening Matrix is "+std::to_string(stats.W.rows())+"x"+std::to_string(stats.W.cols()),
    settings);
  
  timeCheckEnd(timeTracking, settings);

  // VALIDATION: If we want to validate the Whitening by comparison to the inverse
  //Eigen::MatrixXd Diff = cov.inverse() - (W)*(W.transpose());
  //std::cout << "some data from the Diff matrix: \n" << Diff(Eigen::seq(0,5),Eigen::seq(0,5)) << std::endl;

  return stats;
}

// Whitens an image using provided statistics
Eigen::MatrixXf computeWhitenedImage(ImData Im, ImStats stats, settingsValues settings) {
  
  Eigen::MatrixXf centered = Im.im2d.rowwise() - stats.mean.transpose();
  Eigen::MatrixXf whitened = centered*stats.W;
  Logger("Whitening image is "+std::to_string(whitened.rows())+"x"+std::to_string(whitened.cols()),
    settings);

  return whitened;
}

// Computes the RX anomaly detetion image from the whitened image
Eigen::MatrixXf computeRXimage(ImData Im, settingsValues settings){
  
  Eigen::MatrixXf  RX = Im.white2d.rowwise().norm();
  Logger("RX image has size: "+std::to_string(RX.size()), settings);

  return RX;
}

// Displays the RX anomaly detection image
void showSaveRXimage(ImData Im, settingsValues settings){

  if (settings.saveRX || settings.displayRX){
    //  create grayscale image
    Eigen::MatrixXf RX2d = ImageResize1dto2d(Im.RX, Im.rows, Im.cols, Im.interleave);
    const cv::Size RX_image_size(RX2d.cols(), RX2d.rows());
    float min_value = 0;
    float max_value = 0;
    cv::Mat RX_image(RX_image_size, CV_64FC1);
    for (int row = 0; row < RX2d.rows(); ++row) {
      for (int col = 0; col < RX2d.cols(); ++col) {
        const float pixel_value = RX2d(row, col);
        RX_image.at<double>(row, col) = pixel_value;
        min_value = std::min(min_value, pixel_value);
        max_value = std::max(max_value, pixel_value);
      }
    }
    RX_image = 5*(RX_image-min_value)/(max_value-min_value);
    
    if (settings.displayRX){
      cv::namedWindow("test", cv::WINDOW_AUTOSIZE);
      cv::imshow("test", RX_image);
      cv::waitKey(0);
    }

    if (settings.saveRX){
      bool check = cv::imwrite(settings.outDir+"\\RX.jpg", 255*RX_image);
      if (check == false) {
        Logger("WARNING Saving RX image failed.", settings);
      } else {
        Logger("RX Image saves as "+settings.outDir+"\\RX.jpg", settings);
      }
    }
  }
}

const std::string getTgtLibDataFname(const std::string hsi_data_path){
  std::filesystem::path p(hsi_data_path);
  return p.parent_path().string()+"\\lib_detect_fullresolution.sli";
}

const std::string getTgtLibHeaderFname(const std::string hsi_data_path){
  std::filesystem::path p(hsi_data_path);
  return p.parent_path().string()+"\\lib_detect_fullresolution.hdr";
}

const std::string getIdLibDataFname(const std::string hsi_data_path){
  std::filesystem::path p(hsi_data_path);
  return p.parent_path().string()+"\\lib_id_fullresolution.sli";
}

const std::string getIdLibHeaderFname(const std::string hsi_data_path){
  std::filesystem::path p(hsi_data_path);
  return p.parent_path().string()+"\\lib_id_fullresolution.hdr";
}

// Converts data and metadata for a spectral library into 
// a LibData structure using Eigen variable types.
LibData SpecLibData2EigenData(const hsi::HSIData& tgt_lib_data, hsi::HSIDataOptions tgt_lib_data_options, settingsValues settings){
  // Create the structure to hold the image and metadata
  PyPrint_initialize(settings);
  LibData lib_tgt;
  lib_tgt.nSpectra = tgt_lib_data.num_cols;
  PyPrint(lib_tgt.nSpectra, "nSpectra", settings);
  lib_tgt.bands = tgt_lib_data.num_rows;
  PyPrint(lib_tgt.bands, "bands", settings);

  // This creates the vector Im.wl from the wavelength string
  lib_tgt.wl = stringCS2VectorFloats(tgt_lib_data_options.wavelength, lib_tgt.bands);
  PyPrint(lib_tgt.wl, "wl", settings);

  // This creates the vector lib_tgt.specNames from the specNames string
  lib_tgt.specNames = stringCS2Vector(tgt_lib_data_options.specNames);
  PyPrint(lib_tgt.specNames, "specNames", settings);

  // Make a matrix that will hold the spectra
  Eigen::MatrixXf spectra(lib_tgt.nSpectra, lib_tgt.bands);
  PyPrint(lib_tgt.nSpectra, "nSpectra", settings);
  PyPrint(lib_tgt.bands, "bands", settings);
  for (int i = 0; i < lib_tgt.nSpectra; ++i) {
    for (int j = 0; j < lib_tgt.bands; ++j) {
      spectra(i,j) = tgt_lib_data.GetValueLib(j, i, 0).value_as_float;
    }
  }
  PyPrint(spectra, "spectra", settings);
  lib_tgt.spectra = spectra;
  LoggerPrintDataSample(spectra, "library file spectra", settings);

  return lib_tgt;
}

LibData resampleLibrary(LibData lib_tgt_fullres, ImData Im, settingsValues settings) {

  std::cout << "==== Resampling Input Sizes ====" << lib_tgt_fullres.wl.size() << "\n";
  std::cout << "Fullres spectrum wl size: " << lib_tgt_fullres.wl.size() << "\n";
  std::cout << "Fullres spectrum spectra rows: " << lib_tgt_fullres.spectra.rows() << "\n";
  std::cout << "Fullres spectrum spectra cols: " << lib_tgt_fullres.spectra.cols() << "\n";
  std::cout << "Fullres spectrum bands: " << lib_tgt_fullres.bands << "\n";

  // Create the structure to hold the image and metadata
  LibData lib_tgt;
  lib_tgt.specNames = lib_tgt_fullres.specNames;
  lib_tgt.nSpectra = lib_tgt_fullres.nSpectra;
  lib_tgt.bands = Im.bands;
  lib_tgt.wl = Im.wl;
  Eigen::MatrixXf spectra = Eigen::MatrixXf::Zero(lib_tgt_fullres.nSpectra, Im.bands);
  
  PyPrint(lib_tgt_fullres.wl, "lib_tgt_fullres_wl", settings);
  PyPrint(lib_tgt_fullres.spectra, "lib_tgt_fullres_spectra", settings);

  Eigen::VectorXf sigma(Im.bands);
  for (int i = 0; i < Im.bands; ++i) {
    sigma(i) = Im.fwhm(i)/2.355;
  }

  float sum = 0;
  float weight = 0;
  int previous_min = 0;
  int set_previous_min = 0;
  std::vector<int> nonCoveredImageWLs;
  for (int im_bnd_idx = 0; im_bnd_idx < Im.bands; ++im_bnd_idx) {  //  looping over bands in the image
    set_previous_min = 0;
    for (int lb_bnd_idx = previous_min; lb_bnd_idx < lib_tgt_fullres.bands; ++lb_bnd_idx) { // looping over all bands in the library 
      //  compute the difference between the image (target) wavelength and library wavelength so that we 
      //  only compute contributions of library bands with wavelength that are withing 3 sigma of the image wavelength
      float diff = (lib_tgt_fullres.wl[lb_bnd_idx] - Im.wl[im_bnd_idx]);
      float standard_deviations_diff = diff/sigma(im_bnd_idx);

      if (standard_deviations_diff > -3){
        if (set_previous_min == 0){
          set_previous_min = 1;
          previous_min = lb_bnd_idx;
        }
        if (standard_deviations_diff > 3){
          // stop the  iteration through library bands becasue the library band wavelength has
          // passed all wavelengths within 3 standard deviations of the image wavelength
          lb_bnd_idx = lib_tgt_fullres.bands;
        } else {
          weight = normalPDF(sigma(im_bnd_idx), 
                            Im.wl[im_bnd_idx],
                            lib_tgt_fullres.wl[lb_bnd_idx]);
          sum = sum + weight;
          for (int spec_idx = 0; spec_idx < lib_tgt.nSpectra; ++spec_idx) { // looping over all spectra in the library
            spectra(spec_idx, im_bnd_idx) = spectra(spec_idx, im_bnd_idx) + 
                                        lib_tgt_fullres.spectra(spec_idx, lb_bnd_idx)*
                                        weight;
          }          
        }
      }
    }
    if (sum > 0){
      // This image wl was covered by nearby library wls, so we normalize by the sum
      for (int spec_idx = 0; spec_idx < lib_tgt.nSpectra; ++spec_idx) { // looping over all spectra in the library
        spectra(spec_idx, im_bnd_idx) = spectra(spec_idx, im_bnd_idx)/sum;
        //std::cout << "Wavelength: " << Im.wl[im_bnd_idx] << " | Reflectance: " << spectra(spec_idx, im_bnd_idx) << std::endl;
        //std::cout << "Output Index: " << im_bnd_idx << std::endl;
      }
    } else {
      // This image wl was not covered by nearby library wls.
      nonCoveredImageWLs.push_back(im_bnd_idx);
    }
    sum = 0;    
  }

  // Clean up any image wavelengths that were not sufficiently covered by spectra wavelengthts
  // by using the spectral value for the band of the spectrum with the closest wl to the image wl.
  for(int idx : nonCoveredImageWLs){  
    int closest_lib_idx; 
    int closest_lib_wl = (lib_tgt_fullres.wl.array() - Im.wl[idx]).abs().minCoeff(&closest_lib_idx);
    for (int spec_idx = 0; spec_idx < lib_tgt.nSpectra; ++spec_idx) { // looping over all spectra in the library
      spectra(spec_idx, idx) = lib_tgt_fullres.spectra(spec_idx, closest_lib_idx);
    } 
    Logger("WARNING: image wavelength "+std::to_string(Im.wl[idx])+" does not sufficiently overlap library wavelengths."
      "Resampled library value at "+std::to_string(Im.wl[idx])+" will be set to the value at "
      +std::to_string(lib_tgt_fullres.wl[closest_lib_idx])+".", 
      settings);
  }
  
  lib_tgt.spectra = spectra;
  
  PyPrint(lib_tgt.wl, "lib_tgt_wl", settings);
  PyPrint(lib_tgt.spectra, "lib_tgt_spectra", settings);

  if (spectra.rows() > 1){
    LoggerPrintDataSample(spectra, "resampled spectra", settings);
  } else {
    Logger("resampled spectra", settings);
  }
  
  return lib_tgt;
}

Eigen::MatrixXf computeWhitenedLibrary(LibData lib_tgt, ImStats stats, settingsValues settings) {

  Eigen::MatrixXf centered = lib_tgt.spectra.rowwise() - stats.mean.transpose();
  LoggerPrintDataSample(centered, "centered spectra", settings);
  Eigen::MatrixXf spectraWhite = centered*stats.W;
  LoggerPrintDataSample(spectraWhite, "whitened spectra", settings);
  Logger("Whitening library is "+std::to_string(spectraWhite.rows())+"x"+std::to_string(spectraWhite.cols()),
    settings);

  return spectraWhite;
}

Eigen::MatrixXf computeACEimage(ImData Im, LibData lib_tgt, settingsValues settings){
  
  LoggerPrintDataSample(Im.white2d, "whitened image", settings);
  LoggerPrintDataSample(lib_tgt.spectraWhite, "whitened library", settings);
  Eigen::MatrixXf  ACE_num = Im.white2d * lib_tgt.spectraWhite.transpose();
  LoggerPrintDataSample(ACE_num, "ACE numerator", settings);
  Eigen::MatrixXf  ACE_denom = (Im.RX.replicate(1,lib_tgt.nSpectra).array() * lib_tgt.spectraWhite.transpose().colwise().norm().replicate(Im.rows*Im.cols, 1).array()).matrix();
  LoggerPrintDataSample(ACE_denom, "ACE denominator", settings);
  Eigen::MatrixXf  ACE = (ACE_num.array() / ACE_denom.array()).matrix();
  LoggerPrintDataSample(ACE, "ACE image", settings);
  Logger("ACE image is "+std::to_string(ACE_num.rows())+"x"+std::to_string(ACE_num.cols()),
    settings);

  return ACE;
}

Eigen::MatrixXf computeACEmax(Eigen::MatrixXf ACE, settingsValues settings){
  Eigen::MatrixXf ACEmax1d = ACE.rowwise().maxCoeff();
  return ACEmax1d;
}

void showACEResults(ImData Im, LibData lib_tgt, Eigen::MatrixXf ACE, settingsValues settings){
    
  if (settings.saveACE || settings.displayACE){
    // Create the openCV ACE image
    const cv::Size ACEimSize(Im.cols, Im.rows);
    cv::Mat ACEim(ACEimSize, CV_64FC1);

    for (int spec_idx = 0; spec_idx < lib_tgt.nSpectra; ++spec_idx) {
      Eigen::MatrixXf ACEslice2d = ACE(Eigen::all, spec_idx);
      //Eigen::MatrixXf ACE2d = ACEslice2d.reshaped(Im.rows, Im.cols);
      Eigen::MatrixXf ACE2d = ImageResize1dto2d(ACEslice2d, Im.rows, Im.cols, Im.interleave);
      //  create grayscale image
      float min_value = 100;
      float max_value = -100;    
      for (int row = 0; row < Im.rows; ++row) {
        for (int col = 0; col < Im.cols; ++col) {
          float pixel_value = ACE2d(row, col);
          float min_thresh = 0.25;
          pixel_value = std::max(min_thresh,pixel_value);
          ACEim.at<double>(row, col) = pixel_value;
          min_value = std::min(min_value, pixel_value);
          max_value = std::max(max_value, pixel_value);
        }
      }
      ACEim = (ACEim-min_value)/(max_value-min_value);
      Logger(lib_tgt.specNames[spec_idx], settings, 1);
      Logger("Min Value: " +std::to_string(min_value), settings, 1);
      Logger("Max Value: " +std::to_string(max_value), settings, 1);
 
      if (settings.displayACE){
        cv::namedWindow(lib_tgt.specNames[spec_idx], cv::WINDOW_AUTOSIZE);
        cv::imshow(lib_tgt.specNames[spec_idx], ACEim);
        cv::waitKey(0);
      }
      if (settings.saveACE){
        std::string specName = lib_tgt.specNames[spec_idx];
        specName.erase(std::remove(specName.begin(), specName.end(), ' '), specName.end());
        bool check = cv::imwrite(settings.outDir+"\\ACE_"+specName+".jpg", 255*ACEim);
        if (check == false) {
          Logger("WARNING Saving ACE image failed.", settings);
        } else {
          Logger("ACE Image saved as "+settings.outDir+"\\ACE_"+specName+".jpg", settings);
        }
      }
    }
  }
}
  
// Compute a covarnaince matrix and other stats for the input image
// using a 3 step process: compute stats and RX anomaly detection,
// remove the anomalies, and recompute stats from non-anomalous pixels.
// Input image matrix is pixels x bands
ImStats computeAnomalyCleanedImageStats(ImData Im, settingsValues settings) {
  timeCheck timeTracking;
  
  // build a subseted compy of the image structure containing
  // only about 10,000 pixel spectra for quick approximate
  // first covariance computation
  timeTracking = timeCheckStart("Computing subset", settings);  
  ImData Im_sampled;
  Im_sampled.im2d = Im.im2d(Eigen::seq(0,Im.im2d.rows(),std::floor(Im.im2d.rows()/10000.)), Eigen::all);
  Im_sampled.rows = Im.rows;
  Im_sampled.cols = Im.cols;
  timeCheckEnd(timeTracking, settings);

  // compute the covariance for the subset of pixel spectra
  Logger("Computing statistics using Eigen ", settings);
  timeTracking = timeCheckStart("Computing stats using Eigen", settings);  
  ImStats stats_sampled = computeImageStats(Im_sampled, settings);
  timeCheckEnd(timeTracking, settings);

  // Whiten the full image using the subset-computed covariance
  timeTracking = timeCheckStart("Whitening the image with subset statistics", settings);  
  Logger("Whitening the image with subset statistics. ", settings);
  Im_sampled.white2d = computeWhitenedImage(Im, stats_sampled, settings);
  timeCheckEnd(timeTracking, settings);

  // Compute the RX anomaly image using the subset-computed covariance
  // This will be used to remove anomalies for the 
  // primary covariance computation
  Logger("Compute the RX anomaly image. ", settings);
  timeTracking = timeCheckStart("Computing the RX image", settings);  
  Eigen::MatrixXf RX = computeRXimage(Im_sampled, settings);
  timeCheckEnd(timeTracking, settings);
  
  // compute a threshold for anomaly removeal
  // removing the most anomalous 5% of pixels
  timeTracking = timeCheckStart("Computing the 95% threshold", settings);   
  std::vector<float> RXv;
  RXv.resize(RX.size());
  Eigen::VectorXf::Map(&RXv[0], RX.size()) = RX;  
  std::sort(RXv.begin(), RXv.end());
  int idx_95pct = std::round(RX.size()*0.95);
  float thresh_95pct = RXv[idx_95pct];

  std::string msgRX = "Some RX values:\n";
  for (int i = 0; i < 10; ++i) { 
    msgRX = msgRX + std::to_string(RXv[i]) + "\n";
  }
  msgRX = msgRX + "Thresh: " + std::to_string(thresh_95pct) + "\n";
  msgRX = msgRX + "Max: " + std::to_string(RX.maxCoeff()) + "\n";
  Logger(msgRX, settings, 1);
  timeCheckEnd(timeTracking, settings);
   
  timeTracking = timeCheckStart("Subsetting Image", settings);   
  Eigen::VectorXi is_selected = (RX.array() < thresh_95pct).cast<int>(); 
  Eigen::VectorXi is_selectedindices(is_selected.sum());
  int idx = 0;
  for (int i = 0; i < Im.rows; ++i) { 
    if (is_selected(i) == 1){
      is_selectedindices(idx) = i;
      idx++;
    }
  }
  Im_sampled.im2d = Im.im2d(is_selectedindices,Eigen::all);
  Logger("Image subset dimensions: ["+std::to_string(Im_sampled.im2d.rows())+","+
    std::to_string(Im_sampled.im2d.cols())+"]", settings);
  timeCheckEnd(timeTracking, settings);

  // Now we compute the stats for the data with anomalies removed
  // Create the structure to hold all the stats
  ImStats stats; 

  // Compute the image mean
  timeTracking = timeCheckStart("Mean computation and subtraction", settings);   
  stats.mean = Im_sampled.im2d.colwise().mean();
  // Compute the covariance matrix with Eigen
  Eigen::MatrixXf centered = (Im_sampled.im2d.rowwise() - stats.mean.transpose()).template cast<float>();
  timeCheckEnd(timeTracking, settings);
  timeTracking = timeCheckStart("*Covariance computation", settings);   
  Eigen::MatrixXf cov = (centered.transpose() * centered) / float(centered.rows() - 1);
  timeCheckEnd(timeTracking, settings);

  // Compute the eigenvalues and eigenvectors
  timeTracking = timeCheckStart("Eigenvalue and eignvector computation", settings);   
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXf> eigensolver(cov);
  Eigen::MatrixXf evals;
  Eigen::MatrixXf evecs;
  if (eigensolver.info() == Eigen::Success){
    evals = eigensolver.eigenvalues();
    evecs = eigensolver.eigenvectors();
    timeCheckEnd(timeTracking, settings);
  }
  else {
    Logger("WARNING: eigenvalues and eigenvectors failed to compute.", settings);
    timeCheckEnd(timeTracking, settings);
  };

  std::string msgEvals = "Eigenvalues:\n: ";
  for (int i = 0; i < 3; ++i) {
    msgEvals = msgEvals + std::to_string(evals(i))+" | "+std::to_string(evals(i)/evals(evals.rows()-1))+"\n";
  }
  msgEvals = msgEvals + "...\n";
  for (int i = evals.rows()-3; i < evals.rows(); ++i) {
    msgEvals = msgEvals + std::to_string(evals(i))+" | "+std::to_string(evals(i)/evals(evals.rows()-1))+"\n";
  }
  Logger(msgEvals, settings, 1);  

  // Computing the whitening matrix
  timeTracking = timeCheckStart("Whitening matrix computation", settings);   
  Eigen::MatrixXf D = Eigen::MatrixXf::Identity(evals.rows(),evals.rows());
  float maxEval = evals.maxCoeff();
  for (int i = 0; i < evals.rows(); ++i) {
    // Optional regularization on eigenvalues
    evals(i) = std::max(evals(i),maxEval*float(std::pow(10,-6)));
    D(i,i) = 1/sqrt(evals(i));
  }
  Eigen::MatrixXf W =  (evecs*D);
  LoggerPrintDataSample(W, "whitening matrix", settings);
  timeCheckEnd(timeTracking, settings);

  // Recast as float
  stats.cov = cov.template cast<float>();
  stats.evals = evals.template cast<float>();
  stats.evecs = evecs.template cast<float>();
  std::cout <<  settings.nPCs << "\n";
  long int nPCs = settings.nPCs;  
  if ((nPCs > 1) && (nPCs < Im.bands)){
    std::cout << "Number of PCs: " << nPCs << "\n";
    stats.W = (W.template cast<float>()).leftCols(nPCs);
  } else{
    stats.W = W.template cast<float>();
  }

  Logger("im2d Matrix is "+std::to_string(Im.im2d.rows())+"x"+std::to_string(Im.im2d.cols()),
    settings);
  Logger("Covariance is "+std::to_string(stats.cov.rows())+"x"+std::to_string(stats.cov.cols()),
    settings);  
  Logger("Whitening Matrix is "+std::to_string(stats.W.rows())+"x"+std::to_string(stats.W.cols()),
    settings);

  // VALIDATION: If we want to validate the Whitening by comparison to the inverse
  //Eigen::MatrixXd Diff = cov.inverse() - (W)*(W.transpose());
  //std::cout << "some data from the Diff matrix: \n" << Diff(Eigen::seq(0,5),Eigen::seq(0,5)) << std::endl;

  return stats;
}


float FinCov(Eigen::VectorXf x, Eigen::VectorXf y){ 
  // Set up
  float z = x.rows();
  std::cout<< "z: " << z <<std::endl;
  float x_sum = 0, y_sum = 0, xy_sum = 0;
  float x_sqsum = 0, y_sqsum = 0, xy_sqsum = 0;
  // Fill out the sums
  for(int i = 0; i < z; i++){
    x_sum = x_sum + x[i];
    y_sum = y_sum + y[i];
    xy_sum = xy_sum + x[i] * y[i];
    x_sqsum = x_sqsum + x[i] * x[i];
    y_sqsum = y_sqsum + y[i] * y[i];
  }
  std::cout<< "x_sum: " << x_sum <<std::endl;
  std::cout<< "y_sum: " << y_sum <<std::endl;
  std::cout<< "xy_sum: " << xy_sum <<std::endl;
  std::cout<< "x_sqsum: " << x_sqsum <<std::endl;
  std::cout<< "y_sqsum: " << y_sqsum <<std::endl;

  // Calculate corilatoin
  float corr = ( z * xy_sum - x_sum * y_sum )/sqrt(( z * x_sqsum - x_sum * x_sum )*( z * y_sqsum - y_sum * y_sum ));
  std::cout<< "( z * xy_sum - x_sum * y_sum ): " <<( z * xy_sum - x_sum * y_sum )<<std::endl;
  std::cout<< "( z * x_sqsum - x_sum * x_sum )*( z * y_sqsum - y_sum * y_sum ): " <<(( z * x_sqsum - x_sum * x_sum )*( z * y_sqsum - y_sum * y_sum ))<<std::endl;

  return corr;
}


