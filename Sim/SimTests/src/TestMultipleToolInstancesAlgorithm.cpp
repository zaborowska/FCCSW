#include "TestMultipleToolInstancesAlgorithm.h"

DECLARE_COMPONENT(TestMultipleToolInstancesAlgorithm)

TestMultipleToolInstancesAlgorithm::TestMultipleToolInstancesAlgorithm(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc) {
  declareProperty("ToolName1", m_toolname1="ToolWithOutput", "");
  declareProperty("ToolName2", m_toolname2="ToolWithOutput", "");
}

StatusCode TestMultipleToolInstancesAlgorithm::initialize() {
if(GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
    m_tool1 =  tool< IToolWithOutput >( m_toolname1, this  ) ;
    m_tool2 =  tool< IToolWithOutput  >( m_toolname2, this  ) ;

  //m_tool1->retrieve();
  return StatusCode::SUCCESS;
}

StatusCode TestMultipleToolInstancesAlgorithm::execute() {
  m_tool1->setFilename("File1");
  m_tool2->setFilename("File2");
  m_tool1->printFilename(); //should print "File1", does print "File2"
  return StatusCode::SUCCESS;
}

StatusCode TestMultipleToolInstancesAlgorithm::finalize() {
  return GaudiAlgorithm::finalize();
}
