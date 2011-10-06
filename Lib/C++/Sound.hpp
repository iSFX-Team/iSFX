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

using boost::uint32_t;
using boost::int64_t;

namespace iSFX {

////////////////////////////////////////////////////////////////////////////////
///
///   Sound Declaration
///
////////////////////////////////////////////////////////////////////////////////


struct Sound {
private:
  System &system;
  FMOD::Sound *sound;
  FMOD::Channel *channel;
  EffectManager<double> volume_manager;
  
  Fader fade_on_enter;
  Fader fade_on_exit;
  Fader fade_to_stop;
  Fader fade_to_pause;
  Fader fade_on_resume;

  // constantly updated
  bool paused;
  bool playing;
  uint32_t position;
  double percent;
  double volume;
  uint32_t open_state;
  bool opening;
  uint32_t buffered; // this looks like you only get a value for streams
  bool starving;
  bool disk_busy;

  // semi-constant
  std::string url;
  std::string name;
  uint32_t length_ms;
  uint32_t length_pcm;
  uint32_t length_pcmbytes;
  uint32_t start;
  uint32_t stop;
  struct {
    float volume;
    float frequency;
    float pan;
    int priority;
  } defaults;
  float max_volume;
  float min_volume;
  FMOD_SOUND_TYPE type;
  FMOD_SOUND_FORMAT format;
  int channels;
  int bits;
  int numtags;
  std::map<std::string, std::vector<FMOD_TAG> > tags;

public:
  /////////////////////////////////////////////////////////////////////////////
  ///                                                                          
  ///   Signals to signal Events                                                   
  ///                                                                          
  ///
  boost::signals2::scoped_connection system_update_connection;
  boost::signals2::signal<void( uint32_t    )> sigPosition;
  boost::signals2::signal<void( double      )> sigPercent;
  
  boost::signals2::signal<void(             )> sigEntered;
  boost::signals2::signal<void( bool        )> sigPlaying;
  boost::signals2::signal<void( bool        )> sigPaused;
  boost::signals2::signal<void(             )> sigExited;
  
  boost::signals2::signal<void( uint32_t    )> sigState;
  boost::signals2::signal<void( bool        )> sigStarving;
  
  boost::signals2::signal<void( std::string )> sigName;
  boost::signals2::signal<void( uint32_t    )> sigLengthMs;
  
  // Signals used to calculate values
  boost::signals2::signal<float(uint32_t)> calculate_volume;
  
  Sound(System* sys, std::string u);
  void GetStaticInfo();
  void Reset();
  void Update();
  ~Sound();

public:
  /////////////////////////////////////////////////////////////////////////////
  ///                                                                          
  ///   Getters                                                 
  /// 
  std::string GetUrl() { return url; }
  std::string GetName() { return opening ? url : name; }

  std::string GetTag(std::string name, int i=0);

  uint32_t GetLengthMs() { return length_ms; }
  uint32_t GetLengthPCM() { return length_pcm; }
  uint32_t GetLengthPCMBytes() { return length_pcmbytes; }
  
  void GetPCMData();
  uint32_t MemoryUsed();

public:
  /////////////////////////////////////////////////////////////////////////////
  ///                                                                          
  ///   Setters                                                   
  /// 
  void SetStartPosition(uint32_t ms) { start = ms; }
  void SetStopPosition(uint32_t ms) { stop = ms; }
  void SetDefaults(float v = -2, float pa = -2, float f = -2, int pr = -2);
  
  void SetFadeOnEnter(uint32_t len);
  void SetFadeOnExit(uint32_t len);
  void SetFadeOnPlay(uint32_t len);
  void SetFadeOnStop(uint32_t len);
  void SetFadeOnPause(uint32_t len);
  void SetFadeOnResume(uint32_t len);
  
  void SetLoop(uint32_t begin, uint32_t end, int count = 1);

public:
  /////////////////////////////////////////////////////////////////////////////
  ///                                                                          
  ///   Controls                                                   
  ///
  void Play();
  void Pause();
  void Unpause();
  void Stop();
  void PlayStop();
  void PlayPause();
  void SkipTo(long ms);
  
protected:
  /////////////////////////////////////////////////////////////////////////////
  ///                                                                          
  ///   Internal                                                   
  ///
  void _Play(bool paused = 0);
  void _Pause(bool paused);
  void _Stop();
  void _SetVolume(double);
  void _FadeStopCompleteCallback();
  void _FadePauseCompleteCallback();
};


////////////////////////////////////////////////////////////////////////////////
///
///
///
///
///         Sound Implementation
///
///
/// 
///
////////////////////////////////////////////////////////////////////////////////
  
Sound::Sound(System* sys, std::string u) 
  : system((System&)*sys),  // BAD PRACTICE, but because you can't pass references in python (afaik)
    fade_on_enter(+1.0, 1 /*, Fader::LinearFade*/),
    fade_on_exit(-1.0, 1 /*, Fader::LinearFade*/),
    fade_to_stop(-1.0, 1000 /*, Fader::LinearFade*/),
    fade_to_pause(-1.0, 1 /*, Fader::LinearFade*/),
    fade_on_resume(+1.0, 1 /*, Fader::LinearFade*/)
{
  assert(sys != NULL);
  
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
  max_volume = 1.0;
  min_volume = 0;
  start = 0;
  stop = ~(uint32_t)0;
  
  Reset(); // the initial connection below is necessary for nonblocking loading status
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
  
  sigName (name);
  sigLengthMs (length_ms);
  
  if (length_ms < fade_on_enter.length+fade_on_exit.length) {
    double ratio = (1.0*fade_on_enter.length)/fade_on_exit.length;
    fade_on_enter.length = ratio*(length_ms/2);
    fade_on_exit.length = (length_ms/2)/ratio;
  }
  fade_on_enter.start = 0;
  volume_manager.schedule(fade_on_enter);
  fade_on_exit.start = length_ms-fade_on_exit.length-1;
  volume_manager.schedule(fade_on_exit);
  fade_to_stop.on_stop(boost::bind(&Sound::_FadeStopCompleteCallback, this));
  fade_to_pause.on_stop(boost::bind(&Sound::_FadePauseCompleteCallback, this));
  
  uint32_t min = (length_ms/1000)/60;
  uint32_t sec = (length_ms/1000)%60;
  uint32_t dec = length_ms%1000;
  printf("Finished loading: \"%s\" - %02d:%02d.%03d\n", name.c_str(), min, sec, dec);
}
  
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
///   Internal Protected                                                   
///
void Sound::_Play(bool pause) {
  if (!opening) {
    assert(system != NULL);
    checked(system->playSound(FMOD_CHANNEL_FREE, sound, pause, &channel), __LINE__);
    if (!system_update_connection.connected())
      system_update_connection = system.update.connect(boost::bind(&Sound::Update, this));
    if (start != 0) SkipTo(start);
    //volume = min_volume;
    _SetVolume(min_volume);
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

void Sound::_FadeStopCompleteCallback() {
  volume_manager.bypass(false);
  volume_manager.clear_slots();
  _Stop();
}

void Sound::_FadePauseCompleteCallback() {
  volume_manager.bypass(false);
  volume_manager.clear_slots();
  _Pause(true);
}


/////////////////////////////////////////////////////////////////////////////
///                                                                          
///   Getters                                                   
///
std::string Sound::GetTag(std::string name, int i) {
  std::vector<FMOD_TAG> &matches = tags[name];
  if (matches.empty())
    return "";
  return std::string((char*)matches[i].data, matches[i].datalen);
}

uint32_t Sound::MemoryUsed() {
  uint32_t bits;
  checked(sound->getMemoryInfo(FMOD_MEMBITS_ALL, FMOD_EVENT_MEMBITS_ALL, &bits, 0), __LINE__);
  return bits/8 + (bits%8 != 0);
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

/////////////////////////////////////////////////////////////////////////////
///                                                                          
///   Setters                                                   
///
void Sound::SetFadeOnEnter(uint32_t len) {
  fade_on_enter.length = len;
}

void Sound::SetFadeOnExit(uint32_t len) {
  fade_on_exit.length = len;
}

void Sound::SetFadeOnPause(uint32_t len) {
  fade_to_pause.length = len;
}

void Sound::SetFadeOnResume(uint32_t len) {
  fade_on_resume.length = len;
}

void Sound::SetFadeOnPlay(uint32_t len) {
  fade_on_resume.length = len;
}

void Sound::SetFadeOnStop(uint32_t len) {
  fade_to_stop.length = len;
}

void Sound::SetDefaults(float v, float pa, float f, int pr) {
  if (f > 0) defaults.frequency = f;
  if (v >= 0.0 && v <= 1.0) defaults.volume = v;
  if (pa >= -1.0 && pa <= 1.0) defaults.pan = pa;
  if (pr >= 0 && pr <= 255 ) defaults.priority = pr; // TODO fix arbitrary negative limit
  sound->setDefaults(defaults.frequency, defaults.volume, defaults.pan, defaults.priority);
}

void Sound::SetLoop(uint32_t begin, uint32_t end, int count) {
  if (count > 0)
    checked(sound->setLoopPoints(begin, FMOD_TIMEUNIT_MS, end, FMOD_TIMEUNIT_MS), __LINE__);
  checked(sound->setLoopCount(count), __LINE__);
}


///////////////////////////////////////////////////////////////////////////////
///
///   Controls
///
void Sound::SkipTo(long ms) {
  if (ms < 0) ms = 0;
  if (ms > length_ms-1) ms = length_ms-1;
  lazy_check(channel->setPosition((uint32_t)ms, FMOD_TIMEUNIT_MS), __LINE__);
}

void Sound::PlayStop() {
  if (playing) {
    Stop();
  } else {
    if (paused) Unpause();
    else Play();
  }
}

void Sound::PlayPause() {
  if (playing && paused) Unpause();
  else if (!playing) Play();
  else Pause();
}

void Sound::Play() {
  if (!playing) {
    _Play(false);
  }
}
void Sound::Pause() {
  if (!paused && !fade_to_pause.is_connected()) {
    fade_to_pause.start = position;
    fade_to_pause.last_t = fade_to_pause.start-1;
    volume_manager.bypass(true);
    volume_manager.clear_slots();
    volume_manager.once(fade_to_pause);
  }
}
void Sound::Unpause() {
  if (paused && !fade_on_resume.is_connected()) {
    fade_on_resume.start = position;
    fade_on_resume.last_t = fade_on_resume.start-1;
    volume_manager.once(fade_on_resume);
    _Pause(false);
  }
}
void Sound::Stop() {
  if (playing && !fade_to_stop.is_connected()) {
    fade_to_stop.start = position;
    fade_to_stop.last_t = fade_to_stop.start-1;
    volume_manager.bypass(true);
    volume_manager.clear_slots();
    volume_manager.once(fade_to_stop);
  }
}

///////////////////////////////////////////////////////////////////////////////
///
///   Update Function
///
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
    
    sigState(open_state);
  }
  if (starv != starving) {
    starving = starv;
    sigStarving(starving);
  }
  
  
  if (channel != NULL) {
    // Get the Channel/Songs Current State
    lazy_check(channel->getVolume(&vol), __LINE__);
    lazy_check(channel->getPosition(&pos, FMOD_TIMEUNIT_MS), __LINE__);
    lazy_check(channel->isPlaying(&isplaying), __LINE__);
    lazy_check(channel->getPaused(&ispaused), __LINE__);
    // TODO: undefined behavior ^ when channel becomes NULL in the middle of them?
    int64_t ellapsed = (int64_t)pos - (int64_t)position;
    std::cout << "t: " << pos << " ellapsed: " << ellapsed << std::endl;
    
    if (pos != position) {
      position = pos;
      sigPosition(position);
      percent = 100.0*position/length_ms;
      if (percent == percent) {
        sigPercent(percent);
      }
    }
    if (!isplaying && playing) {
      system_update_connection.disconnect();
      playing = isplaying;
      sigPlaying(false);
      std::cout << "stopped" << std::endl;
    } else if (isplaying && !playing) {  
      playing = isplaying;
      sigPlaying(true);
      std::cout << "playing" << std::endl;
    }
    
    if (!ispaused && paused) {
    paused = ispaused;
      sigPaused(false);
      std::cout << "unpaused" << std::endl;
    } else if (ispaused && !paused) {    
      paused = ispaused;
      sigPaused(true);
      std::cout << "paused" << std::endl;
    }
    
    // Volume
    std::cout << "Volume: " << volume << std::endl;
    double dv = 0;
    if (isplaying)
      dv = volume_manager(pos);
    if (!isplaying) {
      volume_manager.reset();
    }
    //std::cout << "dv: " << dv << std::endl;
    volume = vol;
    _SetVolume(volume + dv);
    
  } else if (!opening) {
    std::cout << "channel == null" << std::endl;
    paused = 0;
    playing = 0;
    position = 0;
    percent = 0;
    sigExited();
    std::cout << "finished" << std::endl;
    system_update_connection.disconnect();
    return;
  }
  
  if (position == 0 || position >= stop) {
    if (position != 0)
      Stop();
    //sigExited();
    //std::cout << "finished" << std::endl;
  }
}

};

#endif