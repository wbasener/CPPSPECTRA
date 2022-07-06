#pragma once
#include "spectral.h"
#include "settings.h"
/*
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <filesystem>
*/

// Gets the universal average spectrum
// resampled to the image wavelengths.
LibData getUniversalSpectrum(ImData, settingsValues);
