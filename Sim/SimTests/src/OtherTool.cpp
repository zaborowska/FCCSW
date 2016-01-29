#include "OtherTool.h"

DECLARE_TOOL_FACTORY(OtherTool)

OtherTool::OtherTool(const std::string& aType, const std::string& aName, const IInterface* aParent) :
AlgTool(aType, aName, aParent) {
  declareInterface<IOtherTool>(this);
}

OtherTool::~OtherTool() {}

StatusCode OtherTool::initialize() {
if(AlgTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode OtherTool::finalize() {
  return AlgTool::finalize();
}
