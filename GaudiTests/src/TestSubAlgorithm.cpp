
#include "TestSubAlgorithm.h"

DECLARE_ALGORITHM_FACTORY(TestSubAlgorithm)
TestSubAlgorithm::TestSubAlgorithm(const std::string& aName, ISvcLocator* aSvcLoc):
GaudiAlgorithm(aName, aSvcLoc) {
  name = "testsubalgname";
  type = "TestMultipleToolInstancesAlgorithm";
  sc = createSubAlgorithm(type, name, m_pSubAlgorithm);
  // if anything other methods than initialize, execute, finalize are 
  // needed, the algorithm pointer needs to be cast to the correct type here.
}
TestSubAlgorithm::~TestSubAlgorithm() {}

StatusCode TestSubAlgorithm::execute() {
  info() << "calling execute() ..." << endmsg;
    m_pSubAlgorithm->execute();
    return StatusCode::SUCCESS;
}
