#include "ToolWithTool.h"

DECLARE_TOOL_FACTORY(ToolWithTool)

ToolWithTool::ToolWithTool(const std::string& aType, const std::string& aName, const IInterface* aParent) :
GaudiTool(aType, aName, aParent) {
  declareInterface<IToolWithOutput>(this);
  declareProperty( "DataInputs", inputDataObjects());
  declareProperty( "DataOutputs", outputDataObjects());
  declareProperty("tool", m_tool);
  declarePrivateTool(m_tool);
}

ToolWithTool::~ToolWithTool() {}

StatusCode ToolWithTool::initialize() {
if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_tool.retrieve();
  return StatusCode::SUCCESS;
}

StatusCode ToolWithTool::finalize() {
  return GaudiTool::finalize();
}
StatusCode ToolWithTool::saveOutput() {
  warning()<<"ToolWithTool::saveOutput 1"<<endmsg;
  //m_tool->saveOutput();
  warning()<<"ToolWithTool::saveOutput 2"<<endmsg;
  return StatusCode::SUCCESS;
}
