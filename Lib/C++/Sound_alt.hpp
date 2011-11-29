#ifndef _iSFX_Sound_Hpp_
#define _iSFX_Sound_Hpp_

#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <cmath>
#include <boost/signals2.hpp>
#include "fmod.hpp"
#include "fmod_errors.h"

#include "Error.hpp"
#include "System.hpp"

using boost::uint64_t;
using boost::uint32_t;
using boost::uint16_t;
using boost::uint8_t;
using boost::int64_t;
using boost::int32_t;
using boost::int16_t;
using boost::int8_t;

namespace iSFX {
  
template <typename _A, typename _B, typename _C>
_A force_range(_A a, _B b, _C c) {
  //if (a < b) a = b;
  //else if (a > c) a = c;
  return (a < b ? b : (a > c ? c : a));
}

template<typename _T>
struct Property {
private:
  _T value;
  boost::signals2::signal<void(_T)> changed;
public:
  Property(_T v) : value(v) {}
  operator _T& () {
    return value;
  }
  template<typename _fun>
  void connect(_fun f) {
    changed.connect(f);
    f(value);
  }
  void set(_T v) {
    if (value != v) {
      value = v;
      changed(value);
    }
  }
};

struct Sound {
  
  //boost::signals2::signal<void(std::string)> nameChanged;
  //boost::signals2::signal<void(std::string)> filePathChanged;
  //boost::signals2::signal<void(uint32_t)> indexChanged;
  //boost::signals2::signal<void(uint32_t)> lengthChanged;
  //
  //boost::signals2::signal<void(uint32_t)> startChanged;
  //boost::signals2::signal<void(uint32_t)> stopChanged;
  //
  //boost::signals2::signal<void(uint8_t)> fadingInChanged;
  //boost::signals2::signal<void(uint8_t)> fadingOutChanged;
  //boost::signals2::signal<void(uint8_t)> fadingStopChanged;
  //boost::signals2::signal<void(uint32_t)> fadeInChanged;
  //boost::signals2::signal<void(uint32_t)> fadeOutChanged;
  //boost::signals2::signal<void(uint32_t)> fadeStopChanged;
  //
  //boost::signals2::signal<void(uint32_t)> positionChanged;
  //boost::signals2::signal<void(double)> percentChanged;
  //
  //boost::signals2::signal<void(uint8_t)> playingChanged;
  //boost::signals2::signal<void(uint8_t)> pausedChanged;
  //boost::signals2::signal<void(uint8_t)> loadingChanged;
  //
  //boost::signals2::signal<void(float)> masterVolumeChanged;
  //boost::signals2::signal<void(float)> actualVolumeChanged;
  //
  //boost::signals2::signal<void(std::string)> waveformFileChanged;
  
  Property<std::string> name;
  Property<std::string> filePath;
  Property<uint32_t> index;
  Property<uint32_t> length;
  Property<uint32_t> start;
  Property<uint32_t> stop;
  Property<uint8_t> fadingIn;
  Property<uint8_t> fadingOut;
  Property<uint8_t> fadingStop;
  Property<uint32_t> fadeIn;
  Property<uint32_t> fadeOut;
  Property<uint32_t> fadeStop;
  Property<uint32_t> position;
  Property<double> percent;
  Property<uint8_t> playing;
  Property<uint8_t> paused;
  Property<uint8_t> loading;
  Property<float> masterVolume;
  Property<float> actualVolume;
  Property<std::string> waveformFile;
  
  private:
    iSFX::System& system;
    FMOD::Sound *sound;
    FMOD::Channel *channel;
    
    boost::signals2::scoped_connection system_update_connection;
    
    
    //bool playing;
    //bool paused;
    //bool loading;
    //bool fadingIn;
    //bool fadingOut;
    //bool fadingStop;
    
    //float masterVolume;
    float effectVolume;
    float maxVolume;
    float minVolume;
    float actualVolume;
    
    //uint32_t position;
    
    //uint32_t start;
    //uint32_t end;
    //uint32_t fadeIn;
    //uint32_t fadeOut;
    //uint32_t fadeStop;
    
    //std::string waveformPath;
    
    // Stuff that doesn't change
    std::string path;
    std::string name;
    uint32_t length;
    FMOD_SOUND_TYPE type;
    FMOD_SOUND_FORMAT format;
    int channels;
    int bits;
    
    float default_volume;
    float frequency;
    float pan;
    int priority;
    
  public:
    Sound(iSFX::System* sys, std::string u, uint32_t l, std::string waveformPath) 
      : system((System&)*sys),
        channel(NULL),
        path(u),
        length(l),
        playing(false),
        paused(false),
        loading(true),
        fadingIn(false),
        fadingOut(false),
        fadingStop(false),
        masterVolume(1.0),
        effectVolume(0.0),
        actualVolume(0.0),
        maxVolume(1.0),
        minVolume(0.0),
        position(0),
        start(0),
        end(0),
        fadeIn(1),
        fadeOut(1),
        fadeStop(1),
        waveformPath(waveformPath)
    {
      assert(system.system != NULL);//                                                           length == 0
      FMOD_RESULT result = system->createSound(path.c_str(), FMOD_SOFTWARE | FMOD_NONBLOCKING | (length == 0 ? FMOD_ACCURATETIME : FMOD_CREATESTREAM), 0, &sound);
      if (result) printf("FMOD error! (%d) %s line:%d\n", result, FMOD_ErrorString(result), __LINE__);
      system_update_connection = system.updateSignal.connect(boost::bind(&Sound::initialState, this));
      delayInitialization = true;
    }
    
    ~Sound() {
      if (playing) stop();
      sound->release();
      std::cout << "Deconstructing sound object for '" << path << "'..." << std::endl;
    }
    
    // This function is called the first time system updates. It may not be necessary
    void initialState() {
      //signalStateChange("length", length);
      
      system.updateSignal.disconnect(&Sound::initialState);
      system_update_connection = system.updateSignal.connect(boost::bind(&Sound::update, this));
    }
    
    void finishedLoading() {
      if (delayInitialization) return;
      //sound->setMode(FMOD_LOOP_OFF);
      sound->getFormat(&type, &format, &channels, &bits);
      sound->getDefaults(&frequency, &default_volume, &pan, &priority);
      sound->setDefaults(frequency, 0.0, pan, priority);
      
      const int len = 256;
      char buf[len] = {0};
      FMOD_RESULT result = sound->getName(buf, len);
      if (result) printf("FMOD error! (%d) %s line:%d\n", result, FMOD_ErrorString(result), __LINE__);
      name = std::string(buf); // stops at null
      
      if (length == 0) {
        sound->getLength(&length, FMOD_TIMEUNIT_MS);
        lengthChanged(length);
        nameChanged(name);
        
        std::cout << "finishedLoading() - initial \""<<name<<"\"" << std::endl;
        
        if (waveformPath != "") {
          getWaveform(waveformPath, true);
          printf("Finished generating waveform.\n");
        }
        
        //printf("Freeing sound.\n");
        //sound->release();
        //printf("Creating stream.\n");
        //result = system->createSound(path.c_str(), FMOD_SOFTWARE | FMOD_NONBLOCKING | FMOD_CREATESTREAM, 0, &sound);
        //if (result) printf("FMOD error! (%d) %s line:%d\n", result, FMOD_ErrorString(result), __LINE__);
        //return;
      }
      
      // sooo...
      if (end == 0 || end == 1) {
        end = length-1;
        std::cout << "stopChanged(end); @ line 194" << std::endl;
        stopChanged(end);
      }
      
      std::cout << "\nName: '" << name << "'" << std::endl
                << "[start   fadeIn   fadeOut   end   length ]" << std::endl
                << "[" << start << " " << fadeIn << " " << fadeOut << " " << end << " " << length << "]"<< std::endl
                << "masterVolume: " << masterVolume << std::endl << std::endl;
      
      uint32_t min = (length/1000)/60;
      uint32_t sec = (length/1000)%60;
      uint32_t dec = length%1000;
      printf("Finished loading: \"%s\" - %02d:%02d.%03d\n", name.c_str(), min, sec, dec);
      
      loading = false;
      loadingChanged(loading);
    }

    void update() {
      if (sound == NULL) return;
      paused = isPaused();
      playing = isPlaying();
      if (loading) {
        if (isReady())
          finishedLoading();
        return;
      }
      
      // Get the current position of the sound and how long it has been since update()
      // executed last. 
      uint32_t new_pos = getPosition();
      int64_t dt = (int64_t)new_pos-(int64_t)position;
      if (dt == 0) return;
      std::cout << "t: " << new_pos << " dt: " << dt << std::endl;
      std::cout << "start: " << start << " stop: " << end << std::endl;
      
      // If the start has skipped forward, skip forward.
      if (new_pos < start) {
        setPosition(start);
      }
      
      // Calculate the volume based on start, end, fadeIn, fadeOut, and position
      effectVolume = 1.0;
      if (start <= new_pos && new_pos <= start+fadeIn) {
        effectVolume *= 1.0/fadeIn*(new_pos-start);
        fadingIn = true;
      } else if (new_pos < start) {
        effectVolume = 0;
      } else if (fadingIn) {
        fadingIn = false;
      }
      if (end-fadeOut <= new_pos && new_pos <= end) {
        effectVolume *= 1.0-(1.0/fadeOut*(new_pos-(end-fadeOut)));
        fadingOut = true;
      } else if (fadingOut) {
        effectVolume = 0.0;
        fadingOut = false;
      }
      if (new_pos >= end) {
        stop();
      }
      position = new_pos;
      
      // Update volume
      float vol = (minVolume+effectVolume*(maxVolume-minVolume))*masterVolume;
      if (vol != actualVolume) {
        actualVolume = vol;
        if (channel != NULL) {
          channel->setVolume(actualVolume);
        }
        //signalStateChange("actualVolume", actualVolume);
        actualVolumeChanged(actualVolume);
      }
      // Signal other new states
      //signalStateChange("position", position);
      positionChanged(position);
      //signalStateChange("percent", (length ? (100.0*position)/length : 0));
      percentChanged((length ? (100.0*position)/length : 0));
      
      // Print statements for debugging
      //std::cout << "fadeIn: " << fadeIn << " fadeOut: " << fadeOut << std::endl
      //          << "playing: " << playing << " paused: " << paused << std::endl
      //          << "loading: " << loading << " length: " << length << std::endl
      //          << "volume: " << actualVolume << std::endl << std::endl;
      
      // determine if the sound has stopped playing
      if ((!playing && !loading) || channel == NULL) {
        std::cout << "disconnecting song from system update signal" << std::endl;
        system_update_connection.disconnect();
        position = 0;
        effectVolume = 0.0;
      }
    }
    
    void setStart(int64_t ms) {
      std::cout << "setStart("<<ms<<") => ";
      start = force_range(ms, 0, end-fadeOut-fadeIn);
      if (start != ms)
        startChanged(start);
      std::cout << start << std::endl;
    }
    
    void setStop(int64_t ms) {
      std::cout << "setStop("<<ms<<") => ";
      if (ms == 0) ms = length-1;
      end = force_range(ms, start+fadeIn+fadeOut+1, length-1);
      if (end != ms) {
        std::cout << "   stopChanged(end);" << std::endl;
        stopChanged(end);
      }
      std::cout << end << std::endl;
    }
    
    void setFadeIn(int64_t len) {
      std::cout << "setFadeIn("<<len<<") => ";
      fadeIn = force_range(len, 1, end-start-fadeOut-1);
      if (fadeIn != len)
        fadeInChanged(fadeIn);
      std::cout << fadeIn << std::endl;
    }
    
    void setFadeOut(int64_t len) {
      std::cout << "setFadeOut("<<len<<") => ";
      fadeOut = force_range(len, 1, end-start-fadeIn-1);
      if (fadeOut != len)
        fadeOutChanged(fadeOut);
      std::cout << fadeOut << std::endl;
    }
    void setFadeStop(int64_t len) {
      fadeStop = len;
    }
    
    void playStop() {
      if (!playing) play();
      else stop();
    }
    
    void play(bool dont_pause = true) {
      if (loading) return;
      paused = true;
      FMOD_RESULT result = system->playSound(FMOD_CHANNEL_FREE, sound, true, &channel);
      if (result)
        printf("FMOD error! (%d) %s line:%d\n", result, FMOD_ErrorString(result), __LINE__);
      setPosition(start);
      if (dont_pause) {
        std::cout << "attempting to unpause" << std::endl;
        unpause();
      }
      if (!system_update_connection.connected())
        system_update_connection = system.updateSignal.connect(boost::bind(&Sound::update, this));
    }
    
    void pause() {
      if (channel != NULL) {
        channel->setPaused(true);
        paused = true;
        std::cout << "Pausing" << std::endl;
      }
    }
    
    void unpause() {
      if (channel != NULL) {
        channel->setPaused(false);
        paused = false;
        std::cout << "Unpausing" << std::endl;
      }
    }
    
    void setPause(bool b) {
      if (channel != NULL && paused != b && playing) {
        channel->setPaused(b);
        paused = b;
      }
    }
    
    bool isPlaying() {
      bool b = false;
      if (channel != NULL) {
        channel->isPlaying(&b);
      }
      return b;
    }
    
    //bool isLoading() {
    //  FMOD_OPENSTATE state = FMOD_OPENSTATE_ERROR;
    //  if (channel != NULL) {
    //    sound->getOpenState(&state, NULL, NULL, NULL);
    //  }
    //  return state == FMOD_OPENSTATE_LOADING;
    //}
    
    bool isReady() {
      FMOD_OPENSTATE state = FMOD_OPENSTATE_ERROR;
      FMOD_RESULT result = sound->getOpenState(&state, NULL, NULL, NULL);
      if (result) printf("FMOD error! (%d) %s line:%d\n", result, FMOD_ErrorString(result), __LINE__);
      return state == FMOD_OPENSTATE_READY;
    }
    
    bool isPaused() {
      bool b = false;
      if (channel != NULL) {
        channel->getPaused(&b);
      }
      return b;
    }
    
    void stop() {
      if (channel != NULL) { 
        channel->stop();
        effectVolume = 0;
        playing = false;
        paused = false;
      }
    }
    
    void setPosition(int64_t ms) {
      if (channel != NULL) { 
        if (ms < 0) ms = 0;
        else if (ms > length) ms = length;
        channel->setPosition(ms, FMOD_TIMEUNIT_MS);
        position = ms;
      }
    }
    
    uint32_t getPosition() {
      uint32_t pos = 0;
      if (channel != NULL) { 
        channel->getPosition(&pos, FMOD_TIMEUNIT_MS);
      }
      return pos;
    }
    
    float getVolume() {
      float v = 0;
      if (channel != NULL) { 
        channel->getVolume(&v);
      }
      return v;
    }
    
    void setMasterVolume(float v) {
      if (channel != NULL) {
        if (v < 0.0) v = 0.0;
        else if (v > 1.0) v = 1.0;
        masterVolume = v;
      }
    }
    void setMinVolume(float v) {
      if (channel != NULL) {
        if (v < 0.0) v = 0.0;
        else if (v > 1.0) v = 1.0;
        minVolume = v;
      }
    }
    void setMaxVolume(float v) {
      if (channel != NULL) {
        if (v < 0.0) v = 0.0;
        else if (v > 1.0) v = 1.0;
        maxVolume = v;
      }
    }
    
    void getWaveform(std::string filename, bool force=false) 
    {
      if (!force && loading) return; // or wait
      
    	std::stringstream left_speaker, right_speaker;
    	
    	void *  ptr1;
    	void *  ptr2;
    	uint32_t  len1;
    	uint32_t  len2;
    	
      uint32_t length_pcmbytes; // probably should use FMOD_TIMEUNIT_PCM
      sound->getLength(&length_pcmbytes, FMOD_TIMEUNIT_PCMBYTES);
      
    	sound->lock(0, length_pcmbytes, &ptr1, &ptr2, &len1, &len2);
      //std::cout << "len1: " << len1 << std::endl;
      //std::cout << "len2: " << len2 << std::endl;
    	if (ptr1 == NULL) {
        std::cout << "lock returned NULL" << std::endl;
        //signalStateChange("svg_path", "waveform.svg");
        waveformFileChanged("blank.svg");
        sound->unlock(ptr1, ptr2, len1, len2);
        return;
    	}
    	std::ofstream svgfile;
    	int height = 1 << bits;
    	int width = len1+len2;
    	int skip = 1;
    	
      skip = (len1/(bits/8))/(1<<12);
      if (skip == 0) skip = 1;

    	size_t v = 0;
    	if (bits == 8) {
    		//std::cout << "8 bit" << std::endl;
    		for(size_t i=0; i < len1/(bits/8); i+=skip) {
    			if(channels == 1) {
    				left_speaker << v << "," << ((long)((boost::int8_t*)ptr1)[i]) /*- height/2*/ << "\n";
    				v+=1;
    			} else if(channels == 2) {
    				left_speaker << v << "," << ((long)((boost::int8_t*)ptr1)[i]) /*- height/2*/ << "\n";
    				if (i+1 < len1)
    				  right_speaker << v << "," << ((long)((boost::int8_t*)ptr1)[i+1]) /*+ height/2*/ << "\n";
    				v++;
    			} else {
    			  std::cerr << "channels > 2" << std::endl;
    			}
    		}
    	} else if (bits == 16) {
    		//std::cout << "16 bit" << std::endl;
    		for(size_t i=0; i < len1/(bits/8); i+=skip) {
    			if(channels == 1) {
    				left_speaker << v << "," << ((long)((boost::int16_t*)ptr1)[i]) /*- height/2*/ << "\n";
    				v+=1;
    			} else if(channels == 2) {
    				left_speaker << v << "," << ((long)((boost::int16_t*)ptr1)[i]) /*- height/2*/ << "\n";
    				if (i+1 < len1)
    				  right_speaker << v << "," << ((long)((boost::int16_t*)ptr1)[i+1]) /*+ height/2*/ << "\n";
    				v++;
    			} else {
    			  std::cout << "channels > 2" << std::endl;
    			}
    		}
    	} else if (bits == 24) {
    		std::cerr << "ERROR: no 24 bit integer exists, add code that uses uint8_t to make int24_t respecting byte ordering." << std::endl;
    	} else if (bits == 32) {
    		//std::cout << "32 bit" << std::endl;
    		for(size_t i=0; i < len1/(bits/8); i+=skip) {
    			if(channels == 1) {
    				left_speaker << v << "," << ((long)((boost::int32_t*)ptr1)[4*i]) /*- height/2*/ << "\n";
    				v+=1;
    			} else if(channels == 2) {
    				left_speaker << v << "," << ((long)((boost::int32_t*)ptr1)[4*i]) /*- height/2*/ << "\n";
    				if (i+1 < len1)
    				  right_speaker << v << "," << ((long)((boost::int32_t*)ptr1)[4*(i+1)]) /*+ height/2*/ << "\n";
    				v++;
    			} else {
    			  std::cout << "channels > 2" << std::endl;
    			}
        }
      }

    	int line_width = (skip != 1 ? 2 : 3);
    	svgfile.open(filename.c_str());
    	svgfile << "<?xml version=\"1.0\" standalone=\"yes\"?>\n"
    	        << "<svg version=\"1.1\"\n"
              << "     viewBox=\"0 -" << height << " " << v << " " << height*2 <<"\"\n"
    			    << "     xmlns=\"http://www.w3.org/2000/svg\"\n" 
    			    << "     xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
    			    << "     preserveAspectRatio=\"none\">\n"
    			    << "<g transform=\"translate(0," << -height/2 << ")\">\n"
    			    << "<polyline points=\""
    			    << left_speaker.str()
    			    << "\"\nstyle=\"fill:none;stroke:green;stroke-width:" << line_width << "\">\n"
    	        << "</polyline>\n"
              << "</g>\n";
    	if (channels == 2) {
      			  svgfile << "<g transform=\"translate(0," << +height/2 << ")\">\n"
    			    << "<polyline points=\""
    			    << right_speaker.str()
    			    << "\"\nstyle=\"fill:none;stroke:green;stroke-width:" << line_width << "\">\n"
    	        << "</polyline>\n"
    	        << "</g>\n";
    	}
    	svgfile << "</svg>\n";
    	svgfile.flush();
    	svgfile.close();
      sound->unlock(ptr1, ptr2, len1, len2);
      //signalStateChange("svg_path", new_file.str());
      waveformFileChanged(filename);
    }
};

}

#endif