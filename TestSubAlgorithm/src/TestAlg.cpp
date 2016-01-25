
#include "TestSubAlgorithm/TestAlg.h"
#include "TestSubAlgorithm/TestSubAlg.h"

DECLARE_ALGORITHM_FACTORY(TestAlg)
TestAlg::TestAlg(const std::string& aName, ISvcLocator* aSvcLoc):
GaudiAlgorithm(aName, aSvcLoc) {
  name = "testsubalgname";
  type = "TestSubAlg";
  sc = createSubAlgorithm(type, name, m_pSubAlgorithm);
  m_pTestSubAlg = static_cast<TestSubAlg*>(m_pSubAlgorithm);
}
TestAlg::~TestAlg() {}
void TestAlg::print() {
  std::cout<<"TestAlg::print function called..."<<std::endl;
}

StatusCode TestAlg::execute() {
  std::cout<<"TestAlg::print function called..."<<std::endl;
  for (int i=0; i<10; ++i) {
    m_pTestSubAlg->execute();
  }
    return StatusCode::SUCCESS;
}
