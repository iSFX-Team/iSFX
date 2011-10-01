#ifndef _iSFX_Sound_Hpp_
#define _iSFX_Sound_Hpp_

#include <iostream>
#include <cmath>
#include <assert.h>
#include <boost/signals2.hpp>
#include <sstream>
#include "fmod.hpp"
#include "fmod_errors.h"

#include "Error.hpp"
#include "System.hpp"
#include "Effects.hpp"
//#include "Actions.hpp"

namespace iSFX {

////////////////////////////////////////////////////////////////////////////////
///
///   Sound Declaration
///
////////////////////////////////////////////////////////////////////////////////

struct Sound {
  System &system;
  FMOD::Sound *sound;
  FMOD::Channel *channel;
  EffectManager<double> volume_manager;
  Fader fade_in;
  Fader fade_out;
  Fader fade_stop;

  std::string url;
  std::string name;

  float fade_amount;
  uint32_t fade_duration; //ms


  bool paused;
  bool playing;
  
  uint32_t open_state;
  bool opening;
  uint32_t buffered;
  bool starving;
  bool disk_busy;
  
  uint32_t length_ms;
  uint32_t length_pcm;
  uint32_t length_pcmbytes;
  
  double percent;
  uint32_t position;
  uint32_t start;
  uint32_t stop;
  double volume;
  float max_volume;
  float min_volume;
  struct {
    float volume;
    float frequency;
    float pan;
    int priority;
  } defaults;  
  
  //Fader fade_on_enter;
  //Fader fade_on_play;
  //Fader fade_on_stop;
  //Fader fade_on_pause;
  //Fader fade_on_exit;
  //boost::signals2::signal<void(uint32_t)> effect_signal;
  //std::map<uint32_t, std::list<Effect*> > effects;
  
  FMOD_SOUND_TYPE type;
  FMOD_SOUND_FORMAT format;
  int channels;
  int bits;
  int numtags;
  std::map<std::string, std::vector<FMOD_TAG> > tags;

  boost::signals2::scoped_connection system_update_connection;
  boost::signals2::signal<void( uint32_t    )> now_at_position;
  boost::signals2::signal<void( double      )> now_at_percent;
  boost::signals2::signal<void(             )> now_playing;
  boost::signals2::signal<void(             )> now_paused;
  boost::signals2::signal<void(             )> now_unpaused;
  boost::signals2::signal<void(             )> now_stopped;
  boost::signals2::signal<void(             )> now_finished;
                                           
  boost::signals2::signal<void( uint32_t    )> now_in_state;
  boost::signals2::signal<void( bool        )> now_starving;
                                           
  boost::signals2::signal<void( std::string )> new_name;
  boost::signals2::signal<void( uint32_t    )> new_length_ms;
  
  boost::signals2::signal<float(uint32_t)> change_volume;
  
  Sound(System* sys, std::string u);
  void Reset();
  void Update();
  ~Sound();
  
  void GetStaticInfo();

  /////////////////////////////////////////////////////////////////////////////
  ///                                                                          
  ///   Getters and Setters                                                   
  ///                                                                          
  ///

  std::string GetUrl() { return url; }
  std::string GetName() {
    if (opening) return url;
    return name;
  }

  void SetStartPosition(uint32_t ms) { start = ms; }
  void SetStopPosition(uint32_t ms) { stop = ms; }

  std::string GetTag(std::string name, int i=0);

  uint32_t GetLengthMs() { return length_ms; }
  uint32_t GetLengthPCM() { return length_pcm; }
  uint32_t GetLengthPCMBytes() { return length_pcmbytes; }
  
  void GetPCMData();
  uint32_t MemoryUsed();

  void SetPositionMs(long ms);
  //void Skip(uint32_t to);
  void SetDefaults(float v = -2, float pa = -2, float f = -2, int pr = -2);
  
  void Play(bool fade = 0);
  void Pause(bool fade = 0);
  void Unpause(bool fade = 0);
  void Stop(bool fade = 0);
  void PlayStop(bool fade = 1);
  void PlayPause(bool fade = 0);
  
  bool fadeOnPause;
  bool fadeOnPlay;
  bool fadeOnStop;
  
protected:
  
  void _Play(bool paused = 0);
  void _Pause(bool paused);
  void _Stop();
  void _SetVolume(double);
  
  void _FadeStopCompleteCallback();

public:
  //void ScheduleEffect(uint32_t t, fun_type f);
  //
  ////template<typename _Callable>
  ////void CallAt( _Callable& callable, uint32_t at);
  //
  //void ScheduleFade(double dv, uint32_t start, uint32_t len, std::string method);
  void SetFadeOnEnter(uint32_t len);
  void SetFadeOnExit(uint32_t len);
  void SetFadeOnStop(uint32_t len);
  void SetFadeOnPause(uint32_t len);
  void SetFadeOnPlay(uint32_t len);

  void SetLoop(uint32_t begin, uint32_t end, int count = 1);

  void UnsetLoop();
  
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///
///   Sound Implementation
///
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  
Sound::Sound(System* sys, std::string u) 
  : system((System&)*sys),  // WARNING THIS IS BAD
    fade_in(+1.0, 0, 2),
    fade_out(-1.0, 0, 2),
    fade_stop(-1.0, 0, 3000)
    //volume_manager(dvolume),
    //fade_on_enter(1000, +1.0, Fader::LinearFade),
    //fade_on_play(1000, +1.0, Fader::LinearFade),
    //fade_on_exit(1000, -1.0, Fader::LinearFade),
    //fade_on_pause(1000, -1.0, Fader::LinearFade),
    //fade_on_stop(1000, -1.0, Fader::LinearFade)
{
  
  channel = NULL;
  url = u;
  //FMOD_OPENONLY
  checked(system->createSound(url.c_str(), FMOD_HARDWARE | FMOD_NONBLOCKING | FMOD_ACCURATETIME, 0, &sound), __LINE__);
  channel = NULL;
  opening = true;
  open_state = FMOD_OPENSTATE_MAX;
  buffered = 0;
  length_ms = 0;
  length_pcm = 0;
  length_pcmbytes = 0;
  starving = false;
  disk_busy = 0;
  fadeOnPause = true;
  fadeOnPlay = true;
  fadeOnStop = true;
  max_volume = 1.0;
  min_volume = 0;
  start = 0;
  
  Reset();
  system_update_connection = system.update.connect(boost::bind(&Sound::Update, this));
}

void Sound::GetStaticInfo() {
  checked(sound->setMode(FMOD_LOOP_OFF), __LINE__);
  checked(sound->getLength(&length_ms, FMOD_TIMEUNIT_MS), __LINE__);
  checked(sound->getLength(&length_pcm, FMOD_TIMEUNIT_PCM), __LINE__);
  checked(sound->getLength(&length_pcmbytes, FMOD_TIMEUNIT_PCMBYTES), __LINE__);
  checked(sound->getFormat(&type, &format, &channels, &bits), __LINE__);
  checked(sound->getNumTags(&numtags, NULL), __LINE__);
  checked(sound->getDefaults(&defaults.frequency, &defaults.volume, &defaults.pan, &defaults.priority), __LINE__);
  
  //checked(sound->setDefaults(defaults.frequency, 0.0, defaults.pan, defaults.priority), __LINE__);
  stop = length_ms;
  
  for(int i = 0; i < numtags; i++) {
    FMOD_TAG t;
    checked(sound->getTag(NULL, i, &t), __LINE__);
    tags[t.name].push_back(t);
    //std::cout << t.name << ": " << std::string((char*)t.data, t.datalen) << std::endl;
  }
  
  const int len = 256;
  char buf[len] = {0};
  checked(sound->getName(buf, len), __LINE__);
  name = std::string(buf); // stops at null
  
  new_name(name);
  new_length_ms(length_ms);
  
  volume_manager.schedule(fade_in);
  fade_out.start = length_ms-fade_out.length-1;
  volume_manager.schedule(fade_out);
  fade_stop.on_stop(boost::bind(&Sound::_FadeStopCompleteCallback, this));
  
  uint32_t min = (length_ms/1000)/60;
  uint32_t sec = (length_ms/1000)%60;
  uint32_t dec = length_ms%1000;
  printf("Finished loading: \"%s\" : %02d:%02d.%03d\n", name.c_str(), min, sec, dec);
  
  //fade_on_exit.stop_at(length_ms);
  //fade_on_enter.start_at(start);
  //fade_on_stop.on_stop(boost::bind(&Sound::_Stop, this));
  //fade_on_pause.on_stop(boost::bind(&Sound::_Pause, this, true));
  //fade_on_exit.enabled(false);
  //fade_on_enter.enabled(false);
  //fade_on_play.enabled(false);
  //fade_on_stop.enabled(false);
  //fade_on_pause.enabled(false);
}

void Sound::_FadeStopCompleteCallback() {
  volume_manager.bypass(false);
  Stop(false);
}

//void Sound::SetFadeOnEnter(uint32_t len) {
//  fade_on_enter.length = len;
//  if (!len) fade_on_enter.enabled(false);
//  else fade_on_enter.enabled(true);
//}
//
//void Sound::SetFadeOnPlay(uint32_t len) {
//  fade_on_play.length = len;
//  if (!len) fade_on_play.enabled(false);
//  else fade_on_play.enabled(true);
//}
//
//void Sound::SetFadeOnPause(uint32_t len) {
//  fade_on_pause.length = len;
//  if (!len) fade_on_pause.enabled(false);
//  else fade_on_pause.enabled(true);
//}
//
//void Sound::SetFadeOnExit(uint32_t len) {
//  fade_on_exit.length = len;
//  if (!len) fade_on_exit.enabled(false);
//  else fade_on_exit.enabled(true);
//}
//
//void Sound::SetFadeOnStop(uint32_t len) {
//  fade_on_stop.length = len;
//  if (!len) fade_on_stop.enabled(false);
//  else fade_on_stop.enabled(true);
//}
  
void Sound::Reset() {
  playing = false;
  paused = false;
  position = 0;
  percent = 0;
}

Sound::~Sound() {
  if (playing) Stop();
  checked(sound->release(), __LINE__);
  std::cout << "Deleting sound \"" << url << "\"..." << std::endl;
}

/////////////////////////////////////////////////////////////////////////////
///                                                                          
///   Protected                                                   
///                                                                          
///

void Sound::_Play(bool pause) {
  if (!opening) {
    assert(system != NULL);
    checked(system->playSound(FMOD_CHANNEL_FREE, sound, pause, &channel), __LINE__);
    if (!system_update_connection.connected())
      system_update_connection = system.update.connect(boost::bind(&Sound::Update, this));
    if (start != 0) SetPositionMs(start);
    //if (fade_on_play.enabled()) {
    volume = 0.0;
    _SetVolume(0.0);
    //} else {
    //  _SetVolume(max_volume);
    //}
    if (!pause) { _Pause(false); }
  }
}

void Sound::_Pause(bool pause) {
  if (channel != NULL)
    lazy_check(channel->setPaused(pause), __LINE__);
}

void Sound::_Stop() {
  if (channel != NULL)
    lazy_check(channel->stop(), __LINE__);
}

void Sound::_SetVolume(double v) {
  if (channel != NULL) {
    lazy_check(channel->setVolume(v), __LINE__);
  }
}



/////////////////////////////////////////////////////////////////////////////
///                                                                          
///   Multiline Getters and Setters                                                   
///                                                                          
///
std::string Sound::GetTag(std::string name, int i) {
  std::vector<FMOD_TAG> &matches = tags[name];
  if (matches.empty())
    return "";
  return std::string((char*)matches[i].data, matches[i].datalen);
}

void Sound::SetPositionMs(long ms) {
  if (ms < 0) ms = 0;
  if (ms > length_ms-1) ms = length_ms-1;
  lazy_check(channel->setPosition((uint32_t)ms, FMOD_TIMEUNIT_MS), __LINE__);
}

uint32_t Sound::MemoryUsed() {
  uint32_t bits;
  checked(sound->getMemoryInfo(FMOD_MEMBITS_ALL, FMOD_EVENT_MEMBITS_ALL, &bits, 0), __LINE__);
  return bits/8 + (bits%8 != 0);
}

void Sound::PlayStop(bool fade) {
  if (playing) {
    Stop(fade);
  } else {
    if (paused) Unpause(fade);
    else Play(fade);
  }
}

void Sound::PlayPause(bool fade) {
  if (playing && paused) Unpause();
  else if (!playing) Play(fade);
  else Pause(fade);
}

void Sound::Play(bool fade) {
  if (!playing) {
    if (fade) {
      //fade_on_play.enabled(true);
      //fade_on_play.start_at(position+1);
    }
    _Play(false);
  }
}
void Sound::Pause(bool fade) {
  if (!paused) {
    //if (fade) {
    //  //fade_on_pause.enabled(true);
    //  //fade_on_pause.start_at(position);
    //} else {
      _Pause(true);
    //}
  }
}
void Sound::Unpause(bool fade) {
  if (paused) {
    if (fade) {
      //fade_on_play.enabled(true);
      //fade_on_play.start_at(position);
    }
    _Pause(false);
  }
}
void Sound::Stop(bool fade) {
  if (playing) {
    if (fade) {
      fade_stop.start = position;
      fade_stop.last_t = fade_stop.start-1;
      volume_manager.bypass(true);
      volume_manager.clear_slots();
      volume_manager.once(fade_stop);
    } else {
      _Stop();
    }
  }
}

void Sound::SetDefaults(float v, float pa, float f, int pr) {
  if (f > 0) defaults.frequency = f;
  if (v >= 0.0 && v <= 1.0) defaults.volume = v;
  if (pa >= -1.0 && pa <= 1.0) defaults.pan = pa;
  if (pr >= 0 && pr <= 255 ) defaults.priority = pr; // TODO fix arbitrary negative limit
  sound->setDefaults(defaults.frequency, defaults.volume, defaults.pan, defaults.priority);
}

void Sound::GetPCMData() {
  uint32_t  offset;
  uint32_t  length;
  void *  ptr1;
  void *  ptr2;
  uint32_t  len1;
  uint32_t  len2;

  sound->lock(0, length_pcmbytes, &ptr1, &ptr2, &len1, &len2);
  std::cout << len1 << " " << len2 << "fmt:" << bits << std::endl;
}

void Sound::SetLoop(uint32_t begin, uint32_t end, int count) {
  checked(sound->setLoopPoints(begin, FMOD_TIMEUNIT_MS, end, FMOD_TIMEUNIT_MS), __LINE__);
  checked(sound->setLoopCount(count), __LINE__);
}

void Sound::UnsetLoop() {
  checked(sound->setLoopCount(0), __LINE__);
}

void Sound::Update() {
  // TODO: Define behavior when position is changed or song is paused during a fade.
  uint32_t pos = position;
  float vol = volume;
  bool isplaying = playing;
  bool ispaused = paused;
  
  bool starv = starving;
  uint32_t buffer = buffered;
  FMOD_OPENSTATE state = (FMOD_OPENSTATE)open_state;
  bool disk = disk_busy;
  
  checked(sound->getOpenState(&state, &buffer, &starv, &disk), __LINE__);
  if (state != open_state) {
    open_state = state;
    
    if (state == FMOD_OPENSTATE_READY && opening) {
      opening = false;
      GetStaticInfo();
    }
    
    now_in_state(open_state);
  }
  if (starv != starving) {
    starving = starv;
    now_starving(starving);
  }
  
  
  if (channel != NULL) {
    // Get the Channel/Songs Current State
    lazy_check(channel->getVolume(&vol), __LINE__);
    lazy_check(channel->getPosition(&pos, FMOD_TIMEUNIT_MS), __LINE__);
    lazy_check(channel->isPlaying(&isplaying), __LINE__);
    lazy_check(channel->getPaused(&ispaused), __LINE__);
    // TODO: undefined behavior ^ when channel becomes NULL in the middle of them?
    
    std::cout << "volume: " << volume << std::endl;
    double dv = volume_manager(pos);
    //std::cout << "dv: " << dv << std::endl;
    volume = vol;
    _SetVolume(volume + dv);
    
    uint32_t ellapsed = pos - position;
    if (pos != position) {
      // check for new effects to add
      position = pos;
      now_at_position(position);
      
      percent = 100.0*position/length_ms;
      if (percent == percent) {
        now_at_percent(percent);
      }
    }
    if (!isplaying && playing) {
      system_update_connection.disconnect();
      playing = isplaying;
      now_stopped();
    } else if (isplaying && !playing) {  
      playing = isplaying;
      now_playing();
    }
    
    if (!ispaused && paused) {
    paused = ispaused;
      now_unpaused();
    } else if (ispaused && !paused) {    
      paused = ispaused;
      now_paused();
    }
    if (position >= stop) { 
      Stop();
      now_finished();
    }
    
  } else if (!opening) {
    paused = 0;
    playing = 0;
    position = 0;
    percent = 0;
    system_update_connection.disconnect();
    return;
  }
}

};

#endif