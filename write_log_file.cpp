#include "pch.h"
#include "write_log_file.h"


std::string getCurrentDateTime(std::string s){
    time_t now = time(0);
    struct tm  tstruct;
    char  buf[80];
    tstruct = *localtime(&now);
    if(s=="now")
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    else if(s=="date")
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return std::string(buf);
}

void Logger(std::string logMsg, settingsValues settings, int verboseLevel){
    // output a test string to the logger file and command window, depending on settings
    if (settings.verbose >= verboseLevel){
        std::string now = getCurrentDateTime("now");
        std::fstream file;
        file.open(settings.loggerPath, std::ios_base::out | std::ios_base::app );
        file << now << '\t' << logMsg << std::endl;
        file.close();
        if (settings.verbose > 1){
            std::cout  << "===" << logMsg << "===" << std::endl;
        }
    }
}

void Logger(std::string logMsg, settingsValues settings){
    Logger(logMsg, settings, 0);
}

void LoggerPrintDataSample(Eigen::MatrixXf arrData, std::string arrName, settingsValues settings){
  // output data to log file and command window, depending on settings
  if (settings.verbose >= 1){
    std::string now = getCurrentDateTime("now");
    std::fstream file;
    int numCols = std::min(static_cast<int>(arrData.cols()-1),10);
    file.open(settings.loggerPath, std::ios_base::out | std::ios_base::app );
    file << now << '\t' << "Some data from the " << arrName <<": dimensions = [" << arrData.rows() << "," << arrData.cols() << "]\n"; 
    file << arrData(Eigen::seq(0,5),Eigen::seq(0,numCols)) << std::endl;
    file.close();
    if (settings.verbose >= 2){
      std::cout << "Some data from the " << arrName <<": dimensions = [" << arrData.rows() << "," << arrData.cols() << "]\n"; 
      std::cout << arrData(Eigen::seq(0,5),Eigen::seq(0,numCols)) << std::endl;
    }
  }
}

void LoggerPrintDataSample(Eigen::MatrixXd arrData, std::string arrName, settingsValues settings){
  // output data to log file and command window, depending on settings
  if (settings.verbose >= 1){
    std::string now = getCurrentDateTime("now");
    std::fstream file;
    int numCols = std::min(static_cast<int>(arrData.cols()-1),10);
    file.open(settings.loggerPath, std::ios_base::out | std::ios_base::app );
    file << now << '\t' << "Some data from the " << arrName <<": dimensions = [" << arrData.rows() << "," << arrData.cols() << "]\n"; 
    file << arrData(Eigen::seq(0,5),Eigen::seq(0,numCols)) << std::endl;
    file.close();
    if (settings.verbose >= 2){
      std::cout << "Some data from the " << arrName <<": dimensions = [" << arrData.rows() << "," << arrData.cols() << "]\n"; 
      std::cout << arrData(Eigen::seq(0,5),Eigen::seq(0,numCols)) << std::endl;
    }
  }
}

void LoggerPrintHeaderInfo(hsi::HSIDataOptions data_options, settingsValues settings){  
  std::string interleave_string[3] = {"bsq", "bil", "bip"};
  std::string logMsg = 
      "Information in Header: \n"
      "   Interleave Format = "+interleave_string[data_options.interleave_format]+"\n"+
      "   Datatype = "+std::to_string(data_options.data_type)+" (4=float, 5=double, etc.)\n"+
      "   Header offset = "+std::to_string(data_options.header_offset)+"\n"+
      "   Number of Rows = "+std::to_string(data_options.num_data_rows)+"\n"+
      "   Number of Columns = "+std::to_string(data_options.num_data_cols)+"\n"+
      "   Number of Bands = "+std::to_string(data_options.num_data_bands)+"\n"+
      "   Spectra Names = "+data_options.specNames.substr(0, 40)+"...\n"+
      "   Wavelengths = "+data_options.wavelength.substr(0, 40)+"...\n"+
      "   Bad Bands List = "+data_options.bbl.substr(0, 40)+"...\n"+
      "   Full Width Half Max = "+data_options.fwhm.substr(0, 40)+"...";
  Logger(logMsg, settings);
}

void Logger_initialize(settingsValues settings){
    // clear log file 
    std::ofstream ofs;
    ofs.open(settings.loggerPath, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
    // Always print this image name and log file name to command window
    // unless verbose is set to -1 (no output to command window)
    if (settings.verbose > -1){
        std::cout << "Processing Image: " << settings.imagePath << "\n";
        std::cout << "Log Filename: " << settings.loggerPath << "\n";       
    }
    
    std::string logMsg = 
        "Settings: \n"
        "   verbose = "+std::to_string(settings.verbose)+"\n"+
        "   imagePath = "+settings.imagePath+"\n"+
        "   loggerPath = "+settings.loggerPath+"\n"+
        "   outDir = "+settings.outDir+"\n"+
        "   displayRX = "+std::to_string(settings.displayRX)+"\n"+
        "   saveRX = "+std::to_string(settings.saveRX)+"\n"+
        "   displayACE = "+std::to_string(settings.displayACE)+"\n"+
        "   saveACE = "+std::to_string(settings.saveACE);
    Logger(logMsg, settings);
}

void LoggerExecutionTimeTally(settingsValues settings, std::string total_time){
  // Creates HTML with BarChart of runtimes
  if (settings.verbose >= 4){  

    // Read time info from log file
    std::vector<std::string> execTime;
    std::vector<std::string> execFunction;
    std::string txtLine;
    std::ifstream fileInLog;
    fileInLog.open(settings.loggerPath);
    float measured_time = 0;
    while(fileInLog.good()){
      std::getline(fileInLog, txtLine, '\n'); // get the next line of text, up to the end of line.
      if (txtLine.find("completed in") != std::string::npos){
        std::size_t pos = txtLine.find("completed in");    // position of"completed in" in the line
        execTime.push_back(txtLine.substr(pos+13,5)); // get the time
        execFunction.push_back(txtLine.substr(20,pos-21));
        measured_time = measured_time + std::stof(txtLine.substr(pos+13,5));
      }
    }
    float unmeasred_time = std::stof(total_time) - measured_time;

    // clear executionTime HTML file 
    std::string ExecTimeFname = settings.outDir+"\\ExecTimes.html";
    std::ofstream ofsExecTime;
    ofsExecTime.open(ExecTimeFname, std::ofstream::out | std::ofstream::trunc);
    std::string htmlText =
      "<html>\n"
      "<head>\n"
	    " <!-- Load plotly.js into the DOM -->\n"
	    " <script src='https://cdn.plot.ly/plotly-2.11.1.min.js'></script>\n"
      "</head>\n"
      "  "
      "<body>\n"
	    " <div id='myDiv'><!-- Plotly chart will be drawn inside this DIV --></div>\n"
      " <script>";

    htmlText = htmlText+ 
      "  var data = [\n"
      "    {\n"
      "      y: ["; 
    // Add functions stored in Vector
    for (int i = 0; i < execFunction.size(); i++){
      if (i>0) htmlText = htmlText+",";
      htmlText = htmlText+"'"+std::to_string(i+1)+". "+execFunction[i]+" '";
    }
    htmlText = htmlText+ 
      ",'Other'],\n"
      "      x: [";
    // Add times stored in Vector
    for (int i = 0; i < execTime.size(); i++){
      if (i>0) htmlText = htmlText+",";
      htmlText = htmlText+execTime[i];
    }      
    htmlText = htmlText+ 
      ","+std::to_string(unmeasred_time)+"],\n"
      "      type: 'bar',\n"
      "      orientation: 'h',\n"
      "      marker: {\n"
      "        color: 'rgba(51,153,255,0.6)',\n"
      "        width: 1\n"
      "      }\n"
      "    }\n"
      "  ];\n"
      " var layout = \n"
      "  {\n"
      "    title: 'Execution Time Per Function [Total Time: "+total_time+"s]',\n"
      "    height: 800,\n"
      "    yaxis: {\n"
      "     automargin: true\n,"      
      "     autorange:'reversed'\n"
      "    }	\n"
      "  };\n"
      "  Plotly.newPlot('myDiv', data, layout);\n"
      " </script>\n"
      "</body>\n"
      "</html>";
    
    ofsExecTime << htmlText;
    ofsExecTime.close();

  }
}


void PyPrint_initialize(settingsValues settings){
    // clear log file 
    std::ofstream ofs;
    ofs.open(settings.outDir+"\\pyVars.py", std::ofstream::out | std::ofstream::trunc);
    ofs.close();
    std::fstream file;
    file.open(settings.outDir+"\\pyVars.py", std::ios_base::out | std::ios_base::app );
    file << "import numpy as np" << std::endl;
    file.close();
}
// PyPrint will print a variable of multiple variable types to a Python-reabable file.
// Supported variable types are: int, float, double,  
// std::vector<int>, std::vector<float>, std::vector<double>, std::vector<std::string>
// Eigen::VectorXf,  Eigen::MatrixXf, Eigen::ArrayXf, and Eigen::ArrayXi. 
// Example USage:
// PyPrint_initialize(settings);
// PyPrint(a, "a", settings);
// PyPrint(X, "X", settings);
void PyPrint(int var, std::string varName, settingsValues settings){
    // output a variable value and name to python-readable file, depending on settings
    if (settings.verbose >= 2){
        std::fstream file;
        file.open(settings.outDir+"\\pyVars.py", std::ios_base::out | std::ios_base::app );
        file << varName << " = " << var << std::endl;
        file.close();
    }
}
void PyPrint(float var, std::string varName, settingsValues settings){
    // output a variable value and name to python-readable file, depending on settings
    if (settings.verbose >= 2){
        std::fstream file;
        file.open(settings.outDir+"\\pyVars.py", std::ios_base::out | std::ios_base::app );
        file << varName << " = " << var << std::endl;
        file.close();
    }
}
void PyPrint(double var, std::string varName, settingsValues settings){
    // output a variable value and name to python-readable file, depending on settings
    if (settings.verbose >= 2){
        std::fstream file;
        file.open(settings.outDir+"\\pyVars.py", std::ios_base::out | std::ios_base::app );
        file << varName << " = " << var << std::endl;
        file.close();
    }
}
void PyPrint(Eigen::Index var, std::string varName, settingsValues settings){
    // output a variable value and name to python-readable file, depending on settings
    if (settings.verbose >= 2){
        std::fstream file;
        file.open(settings.outDir+"\\pyVars.py", std::ios_base::out | std::ios_base::app );
        file << varName << " = " << var << std::endl;
        file.close();
    }
}
void PyPrint(Eigen::VectorXf var, std::string varName, settingsValues settings){
    // output a variable value and name to python-readable file, depending on settings
    if (settings.verbose >= 2){
        std::fstream file;
        file.open(settings.outDir+"\\pyVars.py", std::ios_base::out | std::ios_base::app );
        file << varName << " =  np.array([" << var(0);
        for (int i = 1; i < var.rows(); i++){
          file << "," << var(i);
        }
        file << "])" << std::endl;
        file.close();
    }
}
void PyPrint(Eigen::MatrixXf var, std::string varName, settingsValues settings){
    // output a variable value and name to python-readable file, depending on settings
    if (settings.verbose >= 2){
        std::fstream file;
        file.open(settings.outDir+"\\pyVars.py", std::ios_base::out | std::ios_base::app );
        file << varName << " =  np.array([";
        for (int i = 0; i < var.rows(); i++){
          file << "[" << var(i,0);
          for (int j = 1; j < var.cols(); j++){
            file << "," << var(i,j);
          }
          file << "]";
          if (i < var.rows()-1){
            file << ",";
          }
        }
      file << "])" << std::endl;
      file.close();
    }
}
void PyPrint(Eigen::ArrayXf var, std::string varName, settingsValues settings){
    // output a variable value and name to python-readable file, depending on settings
    if (settings.verbose >= 2){
        std::fstream file;
        file.open(settings.outDir+"\\pyVars.py", std::ios_base::out | std::ios_base::app );
        file << varName << " = np.array([";
        for (int i = 0; i < var.rows(); i++){
          file << "[" << var(i,0);
          for (int j = 1; j < var.cols(); j++){
            file << "," << var(i,j);
          }
          file << "]";
          if (i < var.rows()-1){
            file << ",";
          }
        }
      file << "])" << std::endl;
      file.close();
    }
}
void PyPrint(Eigen::ArrayXi var, std::string varName, settingsValues settings){
    // output a variable value and name to python-readable file, depending on settings
    if (settings.verbose >= 2){
        std::fstream file;
        file.open(settings.outDir+"\\pyVars.py", std::ios_base::out | std::ios_base::app );
        file << varName << " = np.array([";
        for (int i = 0; i < var.rows(); i++){
          file << "[" << var(i,0);
          for (int j = 1; j < var.cols(); j++){
            file << "," << var(i,j);
          }
          file << "]";
          if (i < var.rows()-1){
            file << ",";
          }
        }
      file << "])" << std::endl;
      file.close();
    }
}
void PyPrint(std::vector<int> var, std::string varName, settingsValues settings){
    // output a variable value and name to python-readable file, depending on settings
    if (settings.verbose >= 2){
        std::fstream file;
        file.open(settings.outDir+"\\pyVars.py", std::ios_base::out | std::ios_base::app );
        file << varName << " = np.array([" << var.at(0);
        for (int i = 1; i < var.size(); i++){
          file << "," << var.at(i);
        }
        file << "])" << std::endl;
        file.close();
    }
}
void PyPrint(std::vector<float> var, std::string varName, settingsValues settings){
    // output a variable value and name to python-readable file, depending on settings
    if (settings.verbose >= 2){
        std::fstream file;
        file.open(settings.outDir+"\\pyVars.py", std::ios_base::out | std::ios_base::app );
        file << varName << " = np.array([" << var.at(0);
        for (int i = 1; i < var.size(); i++){
          file << "," << var.at(i);
        }
        file << "])" << std::endl;
        file.close();
    }
}
void PyPrint(std::vector<double> var, std::string varName, settingsValues settings){
    // output a variable value and name to python-readable file, depending on settings
    if (settings.verbose >= 2){
        std::fstream file;
        file.open(settings.outDir+"\\pyVars.py", std::ios_base::out | std::ios_base::app );
        file << varName << " = np.array([" << var.at(0);
        for (int i = 1; i < var.size(); i++){
          file << "," << var.at(i);
        }
        file << "])" << std::endl;
        file.close();
    }
}
void PyPrint(std::vector<std::string> var, std::string varName, settingsValues settings){
    if (settings.verbose >= 2){
        std::fstream file;
        file.open(settings.outDir+"\\pyVars.py", std::ios_base::out | std::ios_base::app );
        file << varName << " =[" << "\"" << var.at(0) << "\"";
        for (int i = 1; i < var.size(); i++){
          file << "," <<"\"" << var.at(i) << "\"";
        }
        file << "]" << std::endl;
        file.close();
    }
}