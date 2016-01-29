#include "ToolWithOtherTool.h"

DECLARE_TOOL_FACTORY(ToolWithOtherTool)

ToolWithOtherTool::ToolWithOtherTool(const std::string& aType, const std::string& aName, const IInterface* aParent) :
GaudiTool(aType, aName, aParent) {
  declareInterface<IToolWithOutput>(this);
  declareProperty("tool", m_tool);
  declarePrivateTool(m_tool);
}

ToolWithOtherTool::~ToolWithOtherTool() {}

StatusCode ToolWithOtherTool::initialize() {
if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_tool.retrieve();
  return StatusCode::SUCCESS;
}

StatusCode ToolWithOtherTool::finalize() {
  return GaudiTool::finalize();
}
StatusCode ToolWithOtherTool::saveOutput() {
  return StatusCode::SUCCESS;
}
