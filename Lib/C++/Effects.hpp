#ifndef __iSFX__Effects__hpp__
#define __iSFX__Effects__hpp__

#include <iostream>
#include <numeric>
#include <algorithm>
#include <sstream>
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
#include <boost/cstdint.hpp>

using namespace std;
using boost::int64_t;

struct Effect {
  boost::signals2::scoped_connection connection;
  int64_t start;
  int64_t length;
  int64_t last_t;
  Effect(int64_t start, int64_t length) : start(start), length(length), last_t(-1) {}
  void disconnect() { connection.disconnect(); }
  template<typename signal_t>
  void connect_to(signal_t& s) {
    connection = s.connect(boost::bind(&Effect::operator(), this, _1));
  }
  virtual double operator () (int64_t t) = 0;
};

struct Fader : public Effect {
  double change;
  boost::function<void()> _on_stop;
  Fader(double change, int64_t length) : Effect(0, length), change(change) {}
  void on_stop(boost::function<void()> f) { _on_stop = f; }
  double operator () (int64_t t) {
    int64_t prev_t = last_t;
    last_t = t;
    if (length == 0) {
      disconnect();
      return 0.0;
    }
    if (t < start || t >= start+length) {
      disconnect();
      if (t >= start+length && _on_stop) {
        _on_stop();
      }
    }
    if (t < prev_t) {
      last_t = -1;
      std::cout << toString() << " t: " << t << " prev_t: " << prev_t << std::endl;
      return -change/length*std::min<int64_t>(std::abs((long)prev_t-(long)t), prev_t-start+1);
    } else if (t > prev_t) {
      if (t > start+length) t = start+length;
      if (prev_t < start) prev_t = start;
      std::cout << toString() << " t: " << t << " prev_t: " << prev_t << std::endl;
      std::cout << toString() << " " << t-prev_t << std::endl;
      std::cout << toString() << " " << t-start << std::endl;
      return change/length*std::min<int64_t>(t-prev_t, t-start);
    }
  }
  std::string toString() {
    stringstream ss;
    ss << "{ ["<<start<<","<<start+length<<"), ";
    if (change > 0) {
      ss << "+" << change;
    } else {
      ss << change;
    }
    ss << " }";
    return ss.str();
  }
  bool is_connected() {
    return connection.connected();
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

template<typename T> // TODO use this instead of sum
struct product {
  typedef T result_type;
  template<typename InputIterator>
  T operator()(InputIterator first, InputIterator last) const {
    T sum (1.0);
    while (first != last)
      sum *= *first++;
    return sum;
  }
};

template<typename _T>
struct EffectManager {
  EffectManager(/*_T& t*/) : /*data(t),*/ last_t(-1), last(queue.end()), bypass_queue(false) {}
  //_T& data;
  boost::signals2::signal<double(int64_t), sum<double> > change;
  std::map<int64_t, std::list<Effect*> > queue;
  std::map<int64_t, std::list<Effect*> >::iterator last;
  int64_t last_t;
  double total_change;
  bool bypass_queue;
  _T operator () (int64_t t) { // currently not thread safe (queue)
    double c = 0.0;
    if (!bypass_queue) {
      if (t == last_t) { /*std::cout << "EffectManager: t == last_t" << std::endl;*/ return 0.0; }
      if (queue.empty()) { std::cout << "EffectManager: queue is empty " << __LINE__ << std::endl; return 0.0; }
      if (t < last_t) {
        last_t = -1;
        last = queue.end();
        c -= total_change;
        total_change = 0;
      }
      if (t > last_t) {
        std::map<int64_t, std::list<Effect*> >::iterator next = last;
        if (next != queue.end()) ++next;
        else next = queue.begin();
        while(next != queue.end() && next->first <= t) {
          std::cout << " >>> next->first: " << next->first << " <= t: " << t << std::endl;
          std::list<Effect*>& list = next->second;
          if (!list.empty()) {
            std::list<Effect*>::iterator ei;
            for(ei = list.begin(); ei != list.end(); ++ei) {
              if ((*ei)->length >= 0) {
                (*ei)->connect_to(change);
                (*ei)->last_t = last_t;
              }
            }
          }
          last = next;
          ++next;
        }
      }
    }
    c = change(t);
    total_change += c;
    last_t = t;
    return c;
  }
  void reset() {
    last_t = -1;
    last = queue.end();
    bypass_queue = false;
    change.disconnect_all_slots();
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