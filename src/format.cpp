#include <string>

#include "format.h"

using std::string;


// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int hours = 0, minutes = 0, remain_sec = 0;
  hours = seconds / 3600;
  remain_sec = seconds % 3600;
  minutes = remain_sec / 60;
  seconds = remain_sec % 60;
  return Format::TimeString(hours) + ":" + Format::TimeString(minutes) + ":" + Format::TimeString(seconds); 
}

string Format::TimeString(int val){
  if (val < 10){
    return "0"+std::to_string(val);
  }else{
    return std::to_string(val);
  }
}