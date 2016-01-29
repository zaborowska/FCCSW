#include "TestToolService.h"

// Gaudi
#include "GaudiKernel/IToolSvc.h"

DECLARE_SERVICE_FACTORY(TestToolService)

TestToolService::TestToolService(const std::string& aName, ISvcLocator* aSL): base_class(aName, aSL) {
  declareProperty("tool", m_tool);
  declarePrivateTool(m_tool);
}

TestToolService::~TestToolService(){}

StatusCode TestToolService::initialize(){
  // Initialize necessary Gaudi components
  if (Service::initialize().isFailure()){
    error()<<"Unable to initialize Service()"<<endmsg;
    return StatusCode::FAILURE;
  }
  m_toolSvc = service("ToolSvc");
  if (!m_toolSvc) {
    error()<<"Unable to locate Tool Service"<<endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_tool.retrieve()) {
    error()<<"Unable to retrieve list of user actions"<<endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode TestToolService::finalize() {
  return Service::finalize();
}
