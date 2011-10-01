#ifndef __iSFX__Error_hpp__
#define __iSFX__Error_hpp__
#include "fmod_errors.h"

namespace iSFX {

struct Error {};

inline void checked(FMOD_RESULT result, int line) {
  if (result != FMOD_OK) {
    printf("FMOD error! (%d) %s line:%d\n", result, FMOD_ErrorString(result), line);
    throw Error();
  }
}

inline void lazy_check(FMOD_RESULT result, int line) {
  if (result != FMOD_OK && result != FMOD_ERR_INVALID_HANDLE && result != FMOD_ERR_CHANNEL_STOLEN) {
    printf("FMOD error! (%d) %s line:%d\n", result, FMOD_ErrorString(result), line);
    throw Error();
  }
}

}

#endif //__iSFX__Error_hpp__