
#ifndef _TESTALG_H
#define _TESTALG_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "TestSubAlg.h"

class TestAlg: public  GaudiAlgorithm {
  public:
  TestAlg(const std::string&, ISvcLocator*);
   ~TestAlg();
  void print();
   StatusCode initialize() {print(); return StatusCode::SUCCESS;};
   StatusCode execute(); //  {print(); return StatusCode::SUCCESS;};
   StatusCode finalize() { return StatusCode::SUCCESS;};

  private:
  Algorithm* m_pSubAlgorithm;   // Pointer to the sub algorithm
  TestSubAlg* m_pTestSubAlg;
  // Must be a member variable of the parent class
  std::string type;             // Type of sub algorithm
  std::string name;             // Name to be given to subAlgorithm
  StatusCode sc;                // Status code returned by the call

};
#endif
