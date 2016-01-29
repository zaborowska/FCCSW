#include "TestMultipleToolInstancesAlgorithm.h"

DECLARE_COMPONENT(TestMultipleToolInstancesAlgorithm)

TestMultipleToolInstancesAlgorithm::TestMultipleToolInstancesAlgorithm(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc) {
  declareProperty("tool1",m_tool1);
  declarePrivateTool(m_tool1, "ToolWithMember/tool1");
  declareProperty("tool2", m_tool2);
  declarePrivateTool(m_tool2, "ToolWithMember/tool2");
}

StatusCode TestMultipleToolInstancesAlgorithm::initialize() {
if(GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TestMultipleToolInstancesAlgorithm::execute() {
  m_tool1->setFilename("File1");
  m_tool2->setFilename("File2");
  m_tool1->printFilename();
  m_tool2->printFilename();
  return StatusCode::SUCCESS;
}

StatusCode TestMultipleToolInstancesAlgorithm::finalize() {
  return GaudiAlgorithm::finalize();
}
