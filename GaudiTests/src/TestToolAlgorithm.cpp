#include "TestToolAlgorithm.h"

DECLARE_COMPONENT(TestToolAlgorithm)

TestToolAlgorithm::TestToolAlgorithm(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc) {
  declareProperty("tool",m_tool);
  declarePrivateTool(m_tool);
}

StatusCode TestToolAlgorithm::initialize() {
if(GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_tool.retrieve();
  return StatusCode::SUCCESS;
}

StatusCode TestToolAlgorithm::execute() {
  m_tool->saveOutput();
  return StatusCode::SUCCESS;
}

StatusCode TestToolAlgorithm::finalize() {
  return GaudiAlgorithm::finalize();
}
