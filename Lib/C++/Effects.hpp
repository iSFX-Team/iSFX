#ifndef __iSFX__Effects__hpp__
#define __iSFX__Effects__hpp__

#include <iostream>
#include <numeric>
#include <algorithm>
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
#include <boost/cstdint.hpp>

using namespace std;
using boost::uint32_t;
using boost::int64_t;

struct Effect {
  boost::signals2::scoped_connection connection;
  uint32_t start;
  uint32_t length;
  int64_t last_t;
  Effect(uint32_t start, uint32_t length) : start(start), length(length), last_t((int64_t)start-1) {}
  void disconnect() {
    //std::cout << "disconnect: \""<<start<<"-"<<start+length<<"\"" << std::endl;
    connection.disconnect();
  }
  template<typename signal_t>
  void connect_to(signal_t& s) {
    connection = s.connect(boost::bind(&Effect::operator(), this, _1));
  }
  virtual double operator () (uint32_t t) = 0;
};

struct Fader : public Effect {
  double change;
  boost::function<void()> _on_stop;
  Fader(double change, uint32_t start, uint32_t length) : Effect(start, length), change(change) {}
  void on_stop(boost::function<void()> f) { _on_stop = f; }
  double operator () (uint32_t t) {
    int64_t prev_t = last_t;
    last_t = t;
    if (t < start || t >= start+length) {
      disconnect();
      if (t >= start+length && _on_stop) {
        _on_stop();
      }
    }
    if (t < prev_t) {
      last_t = start-1;
      return -change/length*std::min<int64_t>(std::abs((long)prev_t-(long)t), prev_t-start+1);
    } else {
      return change/length*std::min<int64_t>(std::min<int64_t>(std::min<int64_t>((int64_t)t-start+1, t-prev_t), start+length-prev_t-1),(int64_t)length);
    }
  }
};

// combiner which returns the maximum value returned by all slots
template<typename T>
struct sum {
  typedef T result_type;
  template<typename InputIterator>
  T operator()(InputIterator first, InputIterator last) const {
    T sum (0.0);
    while (first != last)
      sum += *first++;
    return sum;
  }
};

template<typename _T>
struct EffectManager {
  EffectManager(/*_T& t*/) : /*data(t),*/ last_t(-1), last(queue.end()), bypass_queue(false) {}
  //_T& data;
  boost::signals2::signal<double(uint32_t), sum<double> > change;
  std::map<uint32_t, std::list<Effect*> > queue;
  std::map<uint32_t, std::list<Effect*> >::iterator last;
  int64_t last_t;
  double total_change;
  bool bypass_queue;
  _T operator () (uint32_t t) { // currently not thread safe (queue)
    double c = 0.0;
    if (!bypass_queue) {
      if (t == last_t) { /*std::cout << "EffectManager: t == last_t" << std::endl;*/ return 0.0; }
      if (queue.empty()) { std::cout << "EffectManager: queue is empty " << __LINE__ << std::endl; return 0.0; }
      if (t < last_t) { std::cout << "t < last_t" << std::endl; last_t = -1; last = queue.end(); c -= total_change; total_change = 0; }
      if (t > last_t) {
        std::map<uint32_t, std::list<Effect*> >::iterator next = last;
        if (next != queue.end()) ++next;
        else next = queue.begin();
        while(next != queue.end() && next->first <= t) {
          //std::cout << "EffectManager: checking " << next->first << std::endl;
          std::list<Effect*>& list = next->second;
          if (!list.empty()) {
            std::list<Effect*>::iterator ei;
            for(ei = list.begin(); ei != list.end(); ++ei) {
              // it reconnects the same ones a bunch of times
              (*ei)->connect_to(change);
              //std::cout << "connect: \"" << (*ei)->start << "-" << (*ei)->start+(*ei)->length << "\" as slot #: " << change.num_slots() << std::endl;
              (*ei)->last_t = last_t;
            }
          }
          last = next; // what if next == end()
          ++next;
        }
      } else {
        std::cout << "EffectManager: t > last_t at line: " << __LINE__ << std::endl;
        return 0.0; 
      }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The section below is currently commented out because an Effect can be any length
    // So when you are skipping backwards, you do not know how many effects exist
    // Possible solution: add a reverse_queue
    //
    // else /* t < last_t */ {
    //  if (last == queue.end()) { std::cout << "last == queue.end() " << __LINE__ << std::endl; return; }
    //  std::map<uint32_t, std::list<Effect*> >::iterator prev = last;
    //  while(prev != queue.begin() && prev->first->start+prev->first->length > t) {
    //    std::list<Effect*>& list = prev->second;
    //    if (!list.empty()) {
    //      std::list<Effect*>::iterator ei;
    //      for(ei = list.begin(); ei != list.end(); ++ei) {
    //        (*ei)->connect_to(change);
    //        (*ei)->last_t = last_t;
    //      }
    //    }
    //    --prev;
    //  }
    //  last = prev;
    //}
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    c = change(t);
    total_change += c;
    //data += c;
    last_t = t;
    return c;
  }
  void clear_slots() {
    change.disconnect_all_slots();
  }
  void bypass(bool b = true) {
    bypass_queue = b;
  }
  void schedule(Effect& e) {
    queue[e.start].push_back(&e);
  }
  void once(Effect& e) {
    e.connect_to(change);
  }
};

#endif