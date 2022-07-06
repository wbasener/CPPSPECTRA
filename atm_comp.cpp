#include "pch.h"
#include "atm_comp.h"
/*
#include <iostream>
#include <chrono>
#include <thread>
*/

ImData atmComp(ImData Im, settingsValues settings) {
    LibData sp = getUniversalSpectrum(Im, settings);

    if (settings.atmComp){
      std::cout << "Running Atm Comp..." << std::endl;
    } else{
      std::cout << "Not running Atm Comp..." << std::endl;
    }

    return Im;
};

