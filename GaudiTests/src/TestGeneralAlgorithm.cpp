#include "TestGeneralAlgorithm.h"

DECLARE_COMPONENT(TestGeneralAlgorithm)

TestGeneralAlgorithm::TestGeneralAlgorithm(const std::string& name, ISvcLocator* svcLoc):
Algorithm(name, svcLoc) {
  declareProperty("tool",m_tool);
}

StatusCode TestGeneralAlgorithm::initialize() {
if(Algorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_tool.retrieve();
  return StatusCode::SUCCESS;
}

StatusCode TestGeneralAlgorithm::execute() {
  return StatusCode::SUCCESS;
}

StatusCode TestGeneralAlgorithm::finalize() {
  return Algorithm::finalize();
}
