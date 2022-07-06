# CPPSPECTRA
C++ code for reading hyperspectral images and libraries, whitening data, and target detection.

Running works by compiling the main file and typing main <hyperspectral_filename> in the command line, for example:

\Documents\GitHub\CPPSPECTRA>.\main AVIRIS \n
Processing Image: AVIRIS
Log Filename: output\log.txt
===Settings:
   verbose = 4   
   imagePath = AVIRIS   
   loggerPath = output\log.txt   
   outDir = output   
   displayRX = 0   
   saveRX = 1
   displayACE = 0   
   saveACE = 1===   
===Number of Eigen Threads: 16===
===Reading header...===
===Information in Header:
   Interleave Format = bsq
   Datatype = 4 (4=float, 5=double, etc.)
   Header offset = 0
   Number of Rows = 700
   Number of Columns = 800
   Number of Bands = 181
   Spectra Names = NA...
   Wavelengths = {  365.929810,  375.593994,  385.262512,...
   Bad Bands List = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,...
   Full Width Half Max = {  9.852108,  9.796976,  9.744104,  9.69...===
===Reading header completed in 0.002000s===
===Reading image...=== 
===Reading image completed in 3.418000s===
===Converting HSIDATA image data to Eigen matrix...===
===Wavelengths: 365.929810, 375.593994, 385.262512, 394.935486, 404.612915, ...===
===RGB wavelengths: [647.973572,550.299988,453.065491]===
===fwhm: 9.852108, 9.796976, 9.744104, 9.693492, 9.645140, ...===
Some data from the image: dimensions = [560000,181]
0.0095 0.0086 0.0057 0.0117 0.0183 0.0226 0.0218 0.0239 0.0286 0.0322 0.0344
0.0048 0.0027 0.0043 0.0047 0.0047 0.0047 0.0042 0.0053 0.0064 0.0074 0.0072
0.0114  0.006 0.0073 0.0045 0.0042 0.0038 0.0035 0.0051 0.0051 0.0061 0.0067
0.0095  0.004 0.0064 0.0047 0.0055 0.0044 0.0047 0.0058 0.0055 0.0061 0.0063
0.0128 0.0042 0.0067 0.0046 0.0054  0.004 0.0047 0.0051 0.0053 0.0061 0.0058
0.0112 0.0056 0.0076 0.0051 0.0065 0.0047 0.0047  0.005 0.0056 0.0058 0.0064
===Converting HSIDATA image data to Eigen matrix completed in 1.726000s===
==== Resampling Input Sizes ====4401
Fullres spectrum wl size: 4401
Fullres spectrum spectra rows: 1
Fullres spectrum spectra cols: 4401
Fullres spectrum bands: 4401
===resampled spectra===
Not running Atm Comp...
===Compute the RGB visual image...===
z: 181
x_sum: 357.366
y_sum: 0.4797
xy_sum: 0.788213
x_sqsum: 863.618
y_sqsum: 0.00240625
( z * xy_sum - x_sum * y_sum ): -28.7619
( z * x_sqsum - x_sum * x_sum )*( z * y_sqsum - y_sum * y_sum ): 5875.88
-0.375216
**********************************************
===RGB Image saves as output\RGB.jpg===
===Compute the RGB visual image completed in 0.258000s===
===Computing statistics.===
===Computing subset...===
===Computing subset completed in 0.018000s===
===Computing statistics using Eigen ===
===Computing stats using Eigen...===
===Mean and Covariance computation...===
===Mean and Covariance computation completed in 0.059000s===
===Eigenvalue and eignvector computation...===
===Eigenvalue and eignvector computation completed in 0.003000s===
===Eigenvalues:
: 0.000000 | 0.000000
0.000000 | 0.000000
0.000000 | 0.000000
... 
0.008515 | 0.042868
0.155670 | 0.783669
0.198643 | 1.000000
===
===Whitening matrix computation...===
Some data from the whitening matrix 1: dimensions = [181,181]
-0.111426 -0.092963  -5.11191  -1.70158  -5.52688   3.30269  0.472823  0.704573   3.83303   7.59342  0.252898
  0.83434  0.982423  0.327119  -6.90136  -4.34078  -1.82539   2.05831  0.501774   7.41156   1.03057  -0.27011
-0.296349 -0.232872 -0.751395 -0.597452  -4.27013   5.67222  -2.75095   2.21799  -1.40977 -0.672124  -4.39163
-0.885943    1.3451  -2.59503   3.00609  -9.40339  -9.95813   2.77006  -3.35988  -15.1779  -8.27069   13.6643
0.0801352   1.49834   11.3487  -8.60078   -4.0032  -14.5966  -7.23801  0.560521  -14.9723   -35.193   12.2743
  1.72468   9.81703  -4.48472   8.21829   15.4343  -2.37095  -3.73486   10.9846 -0.175886  -7.42015  -15.8218
===im2d Matrix is 10001x181===
===Covariance is 181x181===
===Whitening Matrix is 181x181===
===Whitening matrix computation completed in 0.026000s===
===Computing stats using Eigen completed in 0.105000s===
===Whitening the image with subset statistics...===
===Whitening the image with subset statistics. ===
===Whitening image is 560000x181===
===Whitening the image with subset statistics completed in 1.298000s===
===Compute the RX anomaly image. ===
===Computing the RX image...===
===RX image has size: 560000===
===Computing the RX image completed in 0.275000s===
===Computing the 95% threshold...===
===Some RX values:
8.568159
8.568159
8.661111
8.722978
8.742734
8.800611
8.803618
8.803618
8.803618
8.803618
Thresh: 15.801888
Max: 1790.311768
===
===Computing the 95% threshold completed in 0.121000s===
===Subsetting Image...===
===Image subset dimensions: [532000,181]===
===Subsetting Image completed in 0.291000s===
===Mean computation and subtraction...===
===Mean computation and subtraction completed in 0.141000s===
===*Covariance computation...===
===*Covariance computation completed in 1.303000s===
===Eigenvalue and eignvector computation...===
===Eigenvalue and eignvector computation completed in 0.003000s===
===Eigenvalues:
: -0.000000 | -0.000010
-0.000000 | -0.000009
-0.000000 | -0.000009
...
0.000009 | 0.006767
0.000103 | 0.079097
0.001300 | 1.000000
===
===Whitening matrix computation...===
Some data from the whitening matrix: dimensions = [181,181]
-156.311 -598.443 -36.4051 -250.186  9.26748   414.07   264.15  395.118   290.31 -536.044 -362.788
 479.298  73.0103 -29.3363 -339.864 -606.765 -383.272  108.842  463.344 -53.2267 -469.976 -235.619
 423.726 -32.4147 -199.487 -331.563 -93.8431  217.521  265.925  469.066 -485.941  109.365  158.148
 413.884 -290.851  45.4855  114.837 -166.944 -250.639 -46.6347    234.3  294.078 -369.509 -367.466
 8.16681 -216.306  75.6931  181.294  277.184  151.771 -531.844 -269.044   170.69 -93.0683 -60.2985
-163.529  19.1013  248.057    394.7  242.259  343.507  -281.06   114.94  125.766 -337.319  514.386
===Whitening matrix computation completed in 0.015000s===
