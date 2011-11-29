#ifndef _iSFX_Sound_Hpp_
#define _iSFX_Sound_Hpp_

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <cmath>
#include <assert.h>
#include <boost/signals2.hpp>
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
  Fader fade_on_stop;
  Fader fade_on_pause;
  Fader fade_on_resume;

  // constantly updated
  bool paused;
  bool playing;
  uint32_t position;
  double percent;
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
  
  float actual_volume; // from the channel
  float effect_volume;
  float master_volume;
  float max_volume;
  float min_volume;
  FMOD_SOUND_TYPE type;
  FMOD_SOUND_FORMAT format;
  int channels;
  int bits;
  int numtags;
  std::map<std::string, std::vector<FMOD_TAG> > tags;

public:
  //std::map<std::string, std::string> options;
  boost::signals2::signal<void(std::string, std::string)> sigStateChange;
  void setOption(std::string name, std::string value) {
    //options[name] = value;
    std::stringstream stream;
    stream << value;
    if (name == "name") {
      this->name = value;
    } else if (name == "master_volume") {
      double d;
      stream >> d;
      this->master_volume = d;
    } else if (name == "start") {
      int i;
      stream >> i;
      SetStartPosition(i);
    } else if (name == "stop") {
      int i;
      stream >> i;
      SetStopPosition(i);
    } else if (name == "fade_in") {
      int i;
      stream >> i;
      SetFadeOnEnter(i);
    } else if (name == "fade_out") {
      int i;
      stream >> i;
      SetFadeOnExit(i);
    } else if (name == "fade_stop") {
      int i;
      stream >> i;
      SetFadeOnStop(i);
    } else {
      std::cerr << "Unknown property \"" << name << "\"" << std::endl;
    }
  }
  void performAction(std::string action) {
    if (action == "play") {
      Play();
    } else if (action == "pause") {
      Pause();
    } else if (action == "resume") {
      Unpause();
    } else if (action == "stop") {
      Stop();
    } else if (action == "kill") {
      _Stop();
    } else if (action == "play_stop") {
      PlayStop();
    } else if (action == "graph") {
      GraphPCMData("waveform");
    } else {
      std::cerr << "Unknown action \"" << action << "\"" << std::endl;
    }
  }
  
  /////////////////////////////////////////////////////////////////////////////
  ///                                                                          
  ///   Signals to signal Events                                                   
  ///                                                                          
  ///
  boost::signals2::scoped_connection system_update_connection;
  //boost::signals2::signal<void( uint32_t    )> sigPosition;
  //boost::signals2::signal<void( double      )> sigPercent;
  
  boost::signals2::signal<void(             )> sigLoaded;
  boost::signals2::signal<void(             )> sigEntered;
  boost::signals2::signal<void( bool        )> sigPlaying;
  boost::signals2::signal<void( bool        )> sigPaused;
  boost::signals2::signal<void(             )> sigExited;
  
  boost::signals2::signal<void( uint32_t    )> sigState;
  boost::signals2::signal<void( bool        )> sigStarving;
  
  boost::signals2::signal<void( std::string )> sigName;
  boost::signals2::signal<void( std::string )> sigSVGPath;
  boost::signals2::signal<void( uint32_t    )> sigLengthMs;
  boost::signals2::signal<void( double      )> sigActualVolume; // becasue python uses doubles
  
  // Signals used to calculate values
  boost::signals2::signal<float(uint32_t)> calculate_volume;
  
  Sound(System* sys, std::string u);
  void SignalInitialState();
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
  
  void GraphPCMData(std::string filename);
  uint32_t MemoryUsed();

public:
  /////////////////////////////////////////////////////////////////////////////
  ///                                                                          
  ///   Setters                                                   
  /// 
  void SetStartPosition(uint32_t ms) {
    // insert math here to ensure that the fades are not overlapping
    start = ms;
    fade_on_enter.start = ms;
  }
  void SetStopPosition(uint32_t ms) {
    // insert math here to ensure that the fades are not overlapping
    stop = ms;
    fade_on_exit.start = stop-fade_on_exit.length;
    //volume_manager.schedule(fade_on_exit);
  }
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
  void SetVolume(double v);
  void Play();
  void Pause();
  void Unpause();
  void Stop();
  void Exit();
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
  void _FadeExitCompleteCallback();
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
    fade_on_stop(-1.0, 1000 /*, Fader::LinearFade*/),
    fade_on_pause(-1.0, 1 /*, Fader::LinearFade*/),
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
  effect_volume = 0.0;
  actual_volume = 1.0;
  master_volume = max_volume;
  start = 0;
  stop = ~(uint32_t)0;
  
  Reset(); // the initial connection below is necessary for nonblocking loading status
  system_update_connection = system.update.connect(boost::bind(&Sound::SignalInitialState, this));
}

void Sound::SignalInitialState() {
  std::stringstream ss;
  //sigStateChange("file_path", url);
  ss.str(""); ss << length_ms;
  sigStateChange("length_ms", ss.str());
  
  //ss.str(""); ss << master_volume;
  //sigStateChange("master_volume", ss.str());
  //
  //ss.str(""); ss << start;
  //sigStateChange("start", ss.str());
  //
  //ss.str(""); ss << stop;
  //sigStateChange("stop", ss.str());
  //
  //ss.str(""); ss << fade_on_enter.length;
  //sigStateChange("fade_in", ss.str());
  //
  //ss.str(""); ss << fade_on_exit.length;
  //sigStateChange("fade_out", ss.str());
  //
  //ss.str(""); ss << fade_on_stop.length;
  //sigStateChange("fade_stop", ss.str());
  
  system.update.disconnect(&Sound::SignalInitialState);
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
  sigStateChange("name", name);
  sigLengthMs (length_ms);
  
  std::stringstream ss;
  ss << length_ms;
  sigStateChange("length_ms", ss.str());
  
  //Possibly why nothing works.  Patrick is bad at writing code.
  if (length_ms < fade_on_enter.length+fade_on_exit.length) {
    double ratio = (1.0*fade_on_enter.length)/fade_on_exit.length;
	if(ratio < 1)
	{
		fade_on_enter.length = ratio*(length_ms/2);
		fade_on_exit.length = (length_ms/2)*(1-ratio);
	}
	else
	{
		ratio = (1.0*fade_on_exit.length)/fade_on_enter.length;
		fade_on_enter.length = (1-ratio)*(length_ms/2);
		fade_on_exit.length = (length_ms/2)*ratio;
	}
  }
  fade_on_enter.start = start;
  volume_manager.schedule(fade_on_enter);
  fade_on_exit.start = length_ms-fade_on_exit.length-1;
  //volume_manager.schedule(fade_on_exit);
  fade_on_stop.on_stop(boost::bind(&Sound::_FadeStopCompleteCallback, this));
  fade_on_exit.on_stop(boost::bind(&Sound::_FadeExitCompleteCallback, this));
  fade_on_pause.on_stop(boost::bind(&Sound::_FadePauseCompleteCallback, this));
  
  uint32_t min = (length_ms/1000)/60;
  uint32_t sec = (length_ms/1000)%60;
  uint32_t dec = length_ms%1000;
  printf("Finished loading: \"%s\" - %02d:%02d.%03d\n", name.c_str(), min, sec, dec);
  sigLoaded();
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
    effect_volume = 0.0;
    _SetVolume(min_volume);
    volume_manager.reset();
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

void Sound::_FadeExitCompleteCallback() {
  volume_manager.bypass(false);
  volume_manager.clear_slots();
  _Stop();
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

void Sound::GraphPCMData(std::string filename) 
{
  static boost::uint64_t file_number = 0;
  //static std::stringstream old_file;
  file_number++;
  std::stringstream new_file;
  new_file << filename << file_number%2 << ".svg";
  //if (old_file.str() != "") {
  //  rename(old_file.str().c_str(), new_file.str().c_str()); // WARNING: possible security issue
  //}
  //old_file.str(new_file.str());
	std::stringstream left_speaker, right_speaker;
	boost::uint32_t  length;
	void *  ptr1;
	void *  ptr2;
	boost::uint32_t  len1;
	boost::uint32_t  len2;

	sound->lock(0, length_pcmbytes, &ptr1, &ptr2, &len1, &len2);
	if (ptr1 == NULL) {
    std::cout << "lock returned NULL" << std::endl;
    sigStateChange("svg_path", "waveform.svg");
    return;
	}
	//std::cout << len1 << " " << len2 << "fmt:" << bits << std::endl;
	std::ofstream svgfile;
	int height = 1 << bits;
	int width = len1+len2;
	int skip = 1;// = 50;
	
	//if (len1 > (1<<16)) 
	//{
  skip = (len1/(bits/8))/(1<<12);
  if (skip == 0) skip = 1;
	std::cout << "Graphing every " << skip << " samples." << std::endl;
	std::cout << "Length1: " << len1 << std::endl;
	std::cout << "Length2: " << len2 << std::endl;
	std::cout << "Data points: " << len1/skip << std::endl;
	std::cout << "Channels: " << channels << std::endl;
	//}
	
	size_t v = 0;
	//Get the speaker information.
	if (bits == 8) 
	{
		std::cout << "8 bit" << std::endl;
		for(size_t i=0; i < len1/(bits/8); i+=skip) 
		{
				if(channels == 1)
				{
					left_speaker << " " << v << "," << ((long)((boost::int8_t*)ptr1)[i]) - height/2 << "\n";
					//left_speaker << " " << v+1 << "," << ((long)((boost::int8_t*)ptr1)[i+1]) - height/2 << "\n";
					v+=1;
				}
				else if(channels == 2)
				{
					left_speaker << " " << v << "," << ((long)((boost::int8_t*)ptr1)[i]) - height/2 << "\n";
					if (i+1 < len1)
					  right_speaker << " " << v << "," << ((long)((boost::int8_t*)ptr1)[i+1]) + height/2 << "\n";
					v++;
				}
				else {
				    std::cerr << "channels > 2" << std::endl;
				}
		}
	} 
	else if (bits == 16) 
	{
		std::cout << "16 bit" << std::endl;
		for(size_t i=0; i < len1/(bits/8); i+=skip) 
		{
			if(channels == 1)
			{
				left_speaker << " " << v << "," << ((long)((boost::int16_t*)ptr1)[i]) - height/2 << "\n";
				//left_speaker << " " << v+1 << "," << ((long)((boost::int16_t*)ptr1)[i+1]) - height/2 << "\n";
				v+=1;
			}
			else if(channels == 2)
			{
				left_speaker << " " << v << "," << ((long)((boost::int16_t*)ptr1)[i]) - height/2 << "\n";
				if (i+1 < len1)
				  right_speaker << " " << v << "," << ((long)((boost::int16_t*)ptr1)[i+1]) + height/2 << "\n";
				v++;
			}
			else {
			    std::cout << "channels > 2" << std::endl;
			}
		}
	} 
	else if (bits == 24) {
		std::cerr << "ERROR: no 24 bit integer exists, add code that uses uint8_t to make int24_t respecting byte ordering." << std::endl;
	} 
	else if (bits == 32) 
	{
		std::cout << "32 bit" << std::endl;
		for(size_t i=0; i < len1/(bits/8); i+=skip) 
		{
				if(channels == 1)
				{
					left_speaker << " " << v << "," << ((long)((boost::int32_t*)ptr1)[4*i]) - height/2 << "\n";
					//left_speaker << " " << v+1 << "," << ((long)((boost::int32_t*)ptr1)[i+1]) - height/2 << "\n";
					v+=1;
				}
				else if(channels == 2)
				{
					left_speaker << " " << v << "," << ((long)((boost::int32_t*)ptr1)[4*i]) - height/2 << "\n";
					if (i+1 < len1)
					  right_speaker << " " << v << "," << ((long)((boost::int32_t*)ptr1)[4*(i+1)]) + height/2 << "\n";
					v++;
				}
				else {
				    std::cout << "channels > 2" << std::endl;
				}
				
    }
  }

	int line_width = (skip != 1 ? 2 : 3);
	svgfile.open(new_file.str().c_str());
	svgfile << "<?xml version=\"1.0\" standalone=\"yes\"?>\n"
	        << "<svg version=\"1.1\"\n"
          << "     viewBox=\"0 -" << height << " " << v << " " << height*2 <<"\"\n"
			    //<< "     viewBox=\"0 -" << 100 << " " << width << " " << 100 <<"\"\n"
			    << "     xmlns=\"http://www.w3.org/2000/svg\"\n" 
			    << "     xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
			    << "     preserveAspectRatio=\"none\">\n"
			    //<< "<path d=\"";
			    << "<polyline points=\""
			    << left_speaker.str()
			    << "\"\nstyle=\"fill:none;stroke:green;stroke-width:" << line_width << "\">\n"
	        << "</polyline>\n";
	if (channels == 2) {
			    svgfile << "<polyline points=\""
			    << right_speaker.str()
			    << "\"\nstyle=\"fill:none;stroke:green;stroke-width:" << line_width << "\">\n"
	        << "</polyline>\n";
	        
	}
	svgfile << "</svg>\n";
	svgfile.flush();
	svgfile.close();
  sound->unlock(ptr1, ptr2, len1, len2);
  sigStateChange("svg_path", new_file.str());
}

/////////////////////////////////////////////////////////////////////////////
///                                                                          
///   Setters                                                   
///
void Sound::SetFadeOnEnter(uint32_t len) {
  if (len == 0) len = 1;
  fade_on_enter.length = len;
}

void Sound::SetFadeOnExit(uint32_t len) {
  if (len == 0) len = 1;
  fade_on_exit.start = stop-len;
  fade_on_exit.length = len;
}

void Sound::SetFadeOnPause(uint32_t len) {
  if (len == 0) len = 1;
  fade_on_pause.length = len;
}

void Sound::SetFadeOnResume(uint32_t len) {
  if (len == 0) len = 1;
  fade_on_resume.length = len;
}

//void Sound::SetFadeOnPlay(uint32_t len) {
//  fade_on_enter.length = len;
//}

void Sound::SetFadeOnStop(uint32_t len) {
  //fade_on_stop.start = length_ms-len;
  if (len == 0) len = 1;
  fade_on_stop.length = len;
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
void Sound::SetVolume(double v) {
  if (v < min_volume) v = min_volume;
  if (v > max_volume) v = max_volume;
  master_volume = (float)v;
  //lazy_check(channel->setVolume((float)v), __LINE__);
}

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
  if (!paused && !fade_on_pause.is_connected()) {
    fade_on_pause.start = position;
    fade_on_pause.last_t = fade_on_pause.start-1;
    volume_manager.bypass(true);
    volume_manager.clear_slots();
    volume_manager.once(fade_on_pause);
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
  if (playing && !fade_on_stop.is_connected()) {
    fade_on_stop.start = position;
    fade_on_stop.last_t = fade_on_stop.start-1;
    volume_manager.bypass(true);
    volume_manager.clear_slots();
    volume_manager.once(fade_on_stop);
  }
}

void Sound::Exit() {
  if (playing && !fade_on_exit.is_connected()) {
    fade_on_exit.start = stop-fade_on_exit.length;
    fade_on_exit.last_t = fade_on_exit.start-1;
    volume_manager.bypass(true);
    volume_manager.clear_slots();
    volume_manager.once(fade_on_exit);
  }
}

///////////////////////////////////////////////////////////////////////////////
///
///   Update Function
///
void Sound::Update() {
  std::stringstream ss;
  // TODO: Define behavior when position is changed or song is paused during a fade.
  
  // for o in options
  //   set the corresponding variable to the parsed value
  
  uint32_t pos = position;
  float vol = actual_volume;
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
      //sigPosition(position);
      ss.str("");
      ss << position;
      sigStateChange("position", ss.str());
      percent = 100.0*position/length_ms;
      if (percent == percent) {
        //sigPercent(percent);
        ss.str("");
        ss << percent;
        sigStateChange("percent", ss.str());
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
    if (vol != actual_volume) {
      sigActualVolume(vol);
      ss.str("");
      ss << vol;
      sigStateChange("actual_volume", ss.str());
      actual_volume = vol;
    }
    double dv = 0;
    if (isplaying)
      dv = volume_manager(pos);
    if (!isplaying) {
      volume_manager.reset();
      effect_volume = 0;
    }
    effect_volume += dv;
    _SetVolume(effect_volume*master_volume);
    if (dv != 0) {
      std::cout << "New volume: " << effect_volume*master_volume << std::endl;
    }
    if (effect_volume*master_volume < -0.05) {
      _Stop();
    }
    
  } else if (!opening) {
    std::cout << "channel == null" << std::endl;
    paused = 0;
    playing = 0;
    position = 0;
    percent = 0;
    effect_volume = 0.0; 
    sigExited();
    std::cout << "finished" << std::endl;
    system_update_connection.disconnect();
    return;
  }
  
  if (position == 0 || position >= stop-fade_on_exit.length) {
    if (position != 0)
      Exit();
    //sigExited();
    //std::cout << "finished" << std::endl;
  }
}

};

#endif