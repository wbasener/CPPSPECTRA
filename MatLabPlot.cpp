#include "pch.h"
#include "MatLabPlot.h"

void MPlot(Eigen::VectorXf x, Eigen::MatrixXf y, PlotArg arg){

  // Set up for amount of rows taken from matrix (all rows or arg.amount)
  int nLines;
  if ((arg.amount == -1) || (arg.amount >  y.rows())){
    nLines = y.rows();
  } else{
    nLines = arg.amount;  
  }

  // If a vector of labels was not passed, create the labels from the single label with an index
  if ((arg.labels.empty()) || (arg.labels.size() != nLines)){
    arg.labels.clear(); // removes all elements from the vector if they are present
    for(int j = 0; j < nLines; j++){
      arg.labels.push_back(arg.label+" "+std::to_string(j+1));
    }
  } 

  Eigen::MatrixXf RLo(nLines,1);
  // Make new File
  std::string ofsName = arg.settings.outDir + "\\" + arg.fileName + ".html";
  std::ofstream ofs;
  ofs.open(ofsName, std::ofstream::out | std::ofstream::trunc);
  // Make HTML For Graph
  std::string htmlText =
    "<html>\n"
    "<head>\n"
    "   <!-- Load plotly.js into the DOM -->\n"
    "   <script src='https://cdn.plot.ly/plotly-2.11.1.min.js'></script>\n"
    "</head>\n"
    "<body style=\"background-color:#d1d1e0;\">\n\n"
    "   <div id='myDiv'><!-- Plotly chart will be drawn inside this DIV --></div>\n"
    "<script>\n";
  // Make Data  Groups (Traces)
  for(int j = 0; j < nLines; j++){
    htmlText = htmlText +
      "  var trace"+std::to_string(j)+" = {\n"
      "       x: [";
      // Make the x vector
      for (int i = 0; i < x.size(); i++){
          if (i>0) htmlText = htmlText+",";
              htmlText = htmlText+"'"+std::to_string(x(i))+" '";
      }
      htmlText = htmlText+ 
      ",'Other'],\n"
      "       y: [";
      // Make the Y vector
      int ran;
      if (arg.amount != -1) {
        ran = rand()%y.cols();
      }
      for (int i = 0; i < x.size(); i++){
          if (i>0) htmlText = htmlText+",";
          if (arg.amount != -1){
            htmlText = htmlText+"'"+std::to_string(y(ran,i))+" '"; // pixel, spectrum
          }else{
            htmlText = htmlText+"'"+std::to_string(y(j,i))+" '"; // pixel, spectrum
          }
      }      
      htmlText = htmlText+
      " ],\n"
      "      opacity: " + std::to_string(arg.alpha) + ",\n" 
      "      mode: 'lines',\n"
      "      name: '" + arg.labels.at(j) + "',\n"
      "      line: {\n"
      "         width: "+ std::to_string(arg.lineWidth) +" \n"
      "      }\n"
      "  };\n";
  }
  htmlText = htmlText +
    " \n var data = [ ";
  // make nLines number of trace objs
  for (int i = 0; i < nLines; i++){
    htmlText = htmlText + "trace" + std::to_string(i);
    if (i<nLines-1){
      htmlText = htmlText + ", ";
    }
  }

  htmlText = htmlText +
    "];\n"
    "  var layout = {\n"
    "    title: '" + arg.title + "',\n"
    "    xaxis: {\n"
    "       title: '" + arg.xLabel + "'\n"      
    "    }, \n"
    "    yaxis: {\n"
    "       title: '" + arg.yLabel + "'\n"      
    "    }  \n"
    "  };\n"
    "  Plotly.newPlot('myDiv', data, layout);\n";
  // Finsh HTML
    htmlText = htmlText +
    "</script>\n"
    "</body>\n"
    "</html>";
  
  ofs << htmlText;
  ofs.close();
}