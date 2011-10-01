#ifndef _ISFX_SYSTEM_HPP_
#define _ISFX_SYSTEM_HPP_

#include <iostream>
#include <cmath>
#include <boost/signals2.hpp>
#include "fmod.hpp"
#include "fmod_errors.h"

#include "Error.hpp"

namespace iSFX {

  struct System {
    FMOD::System *system;
    FMOD_RESULT result;
    
    boost::signals2::signal<void()> update;
    
    System() {
      checked(FMOD::System_Create(&system), __LINE__);
      checked(system->init(32, FMOD_INIT_NORMAL | FMOD_HARDWARE, NULL), __LINE__);
    }
    
    void Update() {
      system->update();
      update();
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





















