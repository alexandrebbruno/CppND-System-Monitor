#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization(); 
  
 private:
  long idle_;
  long active_;
  long total_;
};

#endif