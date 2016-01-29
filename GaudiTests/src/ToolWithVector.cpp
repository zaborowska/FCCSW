#include "ToolWithVector.h"

DECLARE_TOOL_FACTORY(ToolWithVector)

ToolWithVector::ToolWithVector(const std::string& aType, const std::string& aName, const IInterface* aParent) :
AlgTool(aType, aName, aParent) {
  declareInterface<IOtherTool>(this);
  declareProperty("string", m_string);
  declareProperty("int", m_int);
  declareProperty("double", m_double);
}

ToolWithVector::~ToolWithVector() {}

StatusCode ToolWithVector::initialize() {
if(AlgTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
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
