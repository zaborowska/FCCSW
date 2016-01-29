
#ifndef SIM_TESTSUBALGORITHM_H
#define SIM_TESTSUBALGORITHM_H

#include "GaudiAlg/GaudiAlgorithm.h"

class TestSubAlgorithm: public  GaudiAlgorithm {
  public:
  TestSubAlgorithm(const std::string&, ISvcLocator*);
   ~TestSubAlgorithm();
  void print();
   StatusCode initialize() {return StatusCode::SUCCESS;};
   StatusCode execute(); //  {print(); return StatusCode::SUCCESS;};
   StatusCode finalize() { return StatusCode::SUCCESS;};

  private:
  Algorithm* m_pSubAlgorithm;   // Pointer to the sub algorithm
  std::string type;             // Type of sub algorithm
  std::string name;             // Name to be given to subAlgorithm
  StatusCode sc;                // Status code returned by the call

};
#endif
