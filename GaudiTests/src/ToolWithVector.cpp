#include "ToolWithVector.h"

DECLARE_TOOL_FACTORY(ToolWithVector)

ToolWithVector::ToolWithVector(const std::string& aType, const std::string& aName, const IInterface* aParent) :
AlgTool(aType, aName, aParent) {
  declareInterface<IOtherTool>(this);
  declareProperty("string", m_string);
  declareProperty("int", m_int);
  declareProperty("double", m_double);
  MsgStream log(msgSvc(), name());
  log << MSG::INFO
      << "Before Initialization and having read the JobOptions file..."
      << endmsg;
  log << MSG::INFO << "StringArray    = " << m_string << endmsg;
  log << MSG::INFO << "IntArray    = " << m_int << endmsg;
  log << MSG::INFO << "DoubleArray    = " << m_double << endmsg;
}

ToolWithVector::~ToolWithVector() {}

StatusCode ToolWithVector::initialize() {
  if(AlgTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  MsgStream log(msgSvc(), name());
  log << MSG::INFO
      << "After Initialization having read the JobOptions file..."
      << endmsg;
  log << MSG::INFO << "StringArray    = " << m_string << endmsg;
  log << MSG::INFO << "IntArray    = " << m_int << endmsg;
  log << MSG::INFO << "DoubleArray    = " << m_double << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode ToolWithVector::finalize() {
  return AlgTool::finalize();
}

std::vector<std::string> ToolWithVector::getString() {
  return m_string;
}
std::vector<int> ToolWithVector::getInt() {
  return m_int;
}
std::vector<double> ToolWithVector::getDouble() {
  return m_double;
}
