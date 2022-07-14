# CPPSPECTRA
C++ code for reading hyperspectral images and libraries, whitening data, and target detection. (Requires Eigen and OpenCV packages, and includes modified versions of hsi_data_reader.cpp and hsi_data_reader.h from https://github.com/rteammco/hsi-data-reader by Richard Teammco.) 

<p float="left">
   <img src="https://github.com/wbasener/CPPSPECTRA/blob/main/output/RGB.jpg" alt="drawing" width="300"/>
   <img src="https://github.com/wbasener/CPPSPECTRA/blob/main/output/RX.jpg" alt="drawing" width="300"/>
</p>

Running works by compiling the main file and typing main <hyperspectral_filename> in the command line resulting in output to the command line, with results written to a created subfoleder called output with RGB, RX andomaly, and ACE target detection images.  Variable values are written to a python file that can be read and reviewed in the included jupyter notebook.  For example:

\Documents\GitHub\CPPSPECTRA>.\main AVIRIS <br />
Processing Image: AVIRIS<br />
Log Filename: output\log.txt<br />
===Settings:<br />
   verbose = 4   <br />
   imagePath = AVIRIS   <br />
   loggerPath = output\log.txt   <br />
   outDir = output   <br />
   displayRX = 0   <br />
   saveRX = 1<br />
   displayACE = 0 <br />
   saveACE = 1===  <br /> 
===Number of Eigen Threads: 16===<br />
===Reading header...===<br />
===Information in Header:<br />
   Interleave Format = bsq<br />
   Datatype = 4 (4=float, 5=double, etc.)<br />
   Header offset = 0<br />
   Number of Rows = 700<br />
   Number of Columns = 800<br />
   Number of Bands = 181<br />
   Spectra Names = NA...<br />
   Wavelengths = {  365.929810,  375.593994,  385.262512,...<br />
   Bad Bands List = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,...<br />
   Full Width Half Max = {  9.852108,  9.796976,  9.744104,  9.69...===<br />
===Reading header completed in 0.002000s===<br />
===Reading image...=== <br />
===Reading image completed in 3.418000s===<br />
===Converting HSIDATA image data to Eigen matrix...===<br />
===Wavelengths: 365.929810, 375.593994, 385.262512, 394.935486, 404.612915, ...===<br />
===RGB wavelengths: [647.973572,550.299988,453.065491]===<br />
===fwhm: 9.852108, 9.796976, 9.744104, 9.693492, 9.645140, ...===<br />
Some data from the image: dimensions = [560000,181]<br />
0.0095 0.0086 0.0057 0.0117 0.0183 0.0226 0.0218 0.0239 0.0286 0.0322 0.0344<br />
0.0048 0.0027 0.0043 0.0047 0.0047 0.0047 0.0042 0.0053 0.0064 0.0074 0.0072<br />
0.0114  0.006 0.0073 0.0045 0.0042 0.0038 0.0035 0.0051 0.0051 0.0061 0.0067<br />
0.0095  0.004 0.0064 0.0047 0.0055 0.0044 0.0047 0.0058 0.0055 0.0061 0.0063<br />
0.0128 0.0042 0.0067 0.0046 0.0054  0.004 0.0047 0.0051 0.0053 0.0061 0.0058<br />
0.0112 0.0056 0.0076 0.0051 0.0065 0.0047 0.0047  0.005 0.0056 0.0058 0.0064<br />
===Converting HSIDATA image data to Eigen matrix completed in 1.726000s===<br />
==== Resampling Input Sizes ====4401<br />
Fullres spectrum wl size: 4401<br />
Fullres spectrum spectra rows: 1<br />
Fullres spectrum spectra cols: 4401<br />
Fullres spectrum bands: 4401<br />
===resampled spectra===<br />
Not running Atm Comp...<br />
===Compute the RGB visual image...===<br />
z: 181<br />
x_sum: 357.366<br />
y_sum: 0.4797<br />
xy_sum: 0.788213<br />
x_sqsum: 863.618<br />
y_sqsum: 0.00240625<br />
( z * xy_sum - x_sum * y_sum ): -28.7619<br />
( z * x_sqsum - x_sum * x_sum )*( z * y_sqsum - y_sum * y_sum ): 5875.88<br />
-0.375216<br />
===RGB Image saves as output\RGB.jpg===<br />
===Compute the RGB visual image completed in 0.258000s===<br />
===Computing statistics.===<br />
===Computing subset...===<br />
===Computing subset completed in 0.018000s===<br />
===Computing statistics using Eigen ===<br />
===Computing stats using Eigen...===<br />
===Mean and Covariance computation...===<br />
===Mean and Covariance computation completed in 0.059000s===<br />
===Eigenvalue and eignvector computation...===<br />
===Eigenvalue and eignvector computation completed in 0.003000s===<br />
===Eigenvalues:<br />
: 0.000000 | 0.000000<br />
0.000000 | 0.000000<br />
0.000000 | 0.000000<br />
... <br />
0.008515 | 0.042868<br />
0.155670 | 0.783669<br />
0.198643 | 1.000000<br />
===<br />
===Whitening matrix computation...===<br />
Some data from the whitening matrix 1: dimensions = [181,181]<br />
-0.111426 -0.092963  -5.11191  -1.70158  -5.52688   3.30269  0.472823  0.704573   3.83303   7.59342  0.252898<br />
  0.83434  0.982423  0.327119  -6.90136  -4.34078  -1.82539   2.05831  0.501774   7.41156   1.03057  -0.27011<br />
-0.296349 -0.232872 -0.751395 -0.597452  -4.27013   5.67222  -2.75095   2.21799  -1.40977 -0.672124  -4.39163<br />
-0.885943    1.3451  -2.59503   3.00609  -9.40339  -9.95813   2.77006  -3.35988  -15.1779  -8.27069   13.6643<br />
0.0801352   1.49834   11.3487  -8.60078   -4.0032  -14.5966  -7.23801  0.560521  -14.9723   -35.193   12.2743<br />
  1.72468   9.81703  -4.48472   8.21829   15.4343  -2.37095  -3.73486   10.9846 -0.175886  -7.42015  -15.8218<br />
===im2d Matrix is 10001x181===<br />
===Covariance is 181x181===<br />
===Whitening Matrix is 181x181===<br />
===Whitening matrix computation completed in 0.026000s===<br />
===Computing stats using Eigen completed in 0.105000s===<br />
===Whitening the image with subset statistics...===<br />
===Whitening the image with subset statistics. ===<br />
===Whitening image is 560000x181===<br />
===Whitening the image with subset statistics completed in 1.298000s===<br />
===Compute the RX anomaly image. ===<br />
===Computing the RX image...===<br />
===RX image has size: 560000===<br />
===Computing the RX image completed in 0.275000s===<br />
===Computing the 95% threshold...===<br />
===Some RX values:<br />
8.568159<br />
8.568159<br />
8.661111<br />
8.722978<br />
8.742734<br />
8.800611<br />
8.803618<br />
8.803618<br />
8.803618<br />
8.803618<br />
Thresh: 15.801888<br />
Max: 1790.311768<br />
===<br />
===Computing the 95% threshold completed in 0.121000s===<br />
===Subsetting Image...===<br />
===Image subset dimensions: [532000,181]===<br />
===Subsetting Image completed in 0.291000s===<br />
===Mean computation and subtraction...===<br />
===Mean computation and subtraction completed in 0.141000s===<br />
===*Covariance computation...===<br />
===*Covariance computation completed in 1.303000s===<br />
===Eigenvalue and eignvector computation...===<br />
===Eigenvalue and eignvector computation completed in 0.003000s===<br />
===Eigenvalues:<br />
: -0.000000 | -0.000010<br />
-0.000000 | -0.000009<br />
-0.000000 | -0.000009<br />
...<br />
0.000009 | 0.006767<br />
0.000103 | 0.079097<br />
0.001300 | 1.000000<br />
===<br />
===Whitening matrix computation...===<br />
Some data from the whitening matrix: dimensions = [181,181]<br />
-156.311 -598.443 -36.4051 -250.186  9.26748   414.07   264.15  395.118   290.31 -536.044 -362.788<br />
 479.298  73.0103 -29.3363 -339.864 -606.765 -383.272  108.842  463.344 -53.2267 -469.976 -235.619<br />
 423.726 -32.4147 -199.487 -331.563 -93.8431  217.521  265.925  469.066 -485.941  109.365  158.148<br />
 413.884 -290.851  45.4855  114.837 -166.944 -250.639 -46.6347    234.3  294.078 -369.509 -367.466<br />
 8.16681 -216.306  75.6931  181.294  277.184  151.771 -531.844 -269.044   170.69 -93.0683 -60.2985<br />
-163.529  19.1013  248.057    394.7  242.259  343.507  -281.06   114.94  125.766 -337.319  514.386<br />
===Whitening matrix computation completed in 0.015000s===<br />
