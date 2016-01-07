#include "TestSavingAlgorithm.h"

DECLARE_COMPONENT(TestSavingAlgorithm)

TestSavingAlgorithm::TestSavingAlgorithm(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc) {
  declareProperty("ToolWithOutput", m_toolWithOutput);
  declarePrivateTool(m_toolWithOutput, "IAlgTool/ToolWithOutput");
}

StatusCode TestSavingAlgorithm::initialize() {
  if ( ! (m_toolWithOutput.retrieve()) ) {
    error()<<"Unable to retrieve tool"<<endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode TestSavingAlgorithm::execute() {
  if ( ! (m_toolWithOutput->saveOutput()) ) {
    error()<<"Unable to save the output from a tool"<<endmsg;
    return StatusCode::FAILURE;
  }
   return StatusCode::SUCCESS;
}

StatusCode TestSavingAlgorithm::finalize() {
   return GaudiAlgorithm::finalize();
}
