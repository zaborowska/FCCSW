#include "ToolWithMember.h"

DECLARE_TOOL_FACTORY(ToolWithMember)

ToolWithMember::ToolWithMember(const std::string& aType, const std::string& aName, const IInterface* aParent) :
GaudiTool(aType, aName, aParent) {
  declareInterface<IToolWithMember>(this);
}

ToolWithMember::~ToolWithMember() {}

StatusCode ToolWithMember::initialize() {
  return GaudiTool::initialize();
}


void ToolWithMember::setFilename(const std::string& filename) {
  m_filename = filename;
}

void ToolWithMember::printFilename() {
  info() << m_filename << endmsg;
}

StatusCode ToolWithMember::finalize() {
  return GaudiTool::finalize();
}
