#ifndef _ISFX_SYSTEM_HPP_
#define _ISFX_SYSTEM_HPP_

#include <iostream>
#include <vector>
#include <cmath>
#include <boost/signals2.hpp>
#include "fmod.hpp"
#include "fmod_errors.h"

#include "Error.hpp"

namespace iSFX {

  struct System {
    FMOD::System *system;
    FMOD_RESULT result;
    
    boost::signals2::signal<void()> updateSignal;
    std::vector<float> waveData;
    
    System() 
      : waveData(512)
    {
      checked(FMOD::System_Create(&system), __LINE__);
      checked(system->init(32, FMOD_INIT_NORMAL | FMOD_HARDWARE, NULL), __LINE__);
    }
    
    void update() {
      system->update();
      updateSignal();
    }
    
    const std::vector<float>& getWaveData(uint8_t right=0) {
      system->getWaveData(&waveData[0], waveData.size(), right);
      return waveData;
    }
    
    operator FMOD::System* () {
      return system;
    }
    
    FMOD::System* operator -> () {
      return system;
    }
  };

}

#endif





















