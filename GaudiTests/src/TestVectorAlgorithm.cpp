#include "TestVectorAlgorithm.h"

DECLARE_COMPONENT(TestVectorAlgorithm)

TestVectorAlgorithm::TestVectorAlgorithm(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc) {
  declareProperty("tool",m_tool);
  declarePrivateTool(m_tool,"IOtherTool/ToolWithVector");
  declareProperty("string", m_string);
  declareProperty("int", m_int);
  declareProperty("double", m_double);
}

StatusCode TestVectorAlgorithm::initialize() {
if(GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_tool.retrieve();
  return StatusCode::SUCCESS;
}

StatusCode TestVectorAlgorithm::execute() {
  bool ifS = false, ifI = false, ifD = false;
  if(m_string != m_tool->getString()) {
    ifS = true;
    error() << "vectors of string are not identical: "<<endmsg;
    error() << "\talgorithm: "<<m_string<<endmsg;
    error() << "\ttool: "<<m_tool->getString()<<endmsg;
  }
  if(m_int != m_tool->getInt()) {
    ifI = true;
    error() << "vectors of int are not identical "<<endmsg;
    error() << "\talgorithm: "<<m_int<<endmsg;
    error() << "\ttool: "<<m_tool->getInt()<<endmsg;
  }
  if(m_double != m_tool->getDouble()) {
    ifD = true;
    error() << "vectors of double are not identical "<<endmsg;
    error() << "\talgorithm: "<<m_double<<endmsg;
    error() << "\ttool: "<<m_tool->getDouble()<<endmsg;
  }
  if(ifS || ifI || ifD)
    return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

StatusCode TestVectorAlgorithm::finalize() {
  return GaudiAlgorithm::finalize();
}
