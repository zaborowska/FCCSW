
#include "TestSubAlgorithm/TestSubAlg.h"

DECLARE_ALGORITHM_FACTORY(TestSubAlg)

TestSubAlg::TestSubAlg(const std::string& aName, ISvcLocator* aSvcLoc):
GaudiAlgorithm(aName, aSvcLoc) {
}

TestSubAlg::~TestSubAlg() {}

void TestSubAlg::print() {
  std::cout<<"print function called, printing something..."<<std::endl;
}

StatusCode TestSubAlg::execute() {
  std::cout<<"TestSubAlg::execute called..."<<std::endl;
  return StatusCode::SUCCESS;
}
