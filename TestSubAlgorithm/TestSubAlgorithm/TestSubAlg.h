#ifndef _TESTSUBALG_H
#define _TESTSUBALG_H 

#include<iostream>
#include<vector>

#include "GaudiAlg/GaudiAlgorithm.h"



class TestSubAlg: public  GaudiAlgorithm {
  public:
  TestSubAlg(const std::string&, ISvcLocator*);
   ~TestSubAlg();
  void print();
   StatusCode initialize() {print(); return StatusCode::SUCCESS;};
   StatusCode execute(); //  {print(); return StatusCode::SUCCESS;};
   StatusCode finalize() { return StatusCode::SUCCESS;};

   private:
   int m_someInteger;

};
#endif
