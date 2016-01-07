#include "ToolWithOutput.h"

DECLARE_TOOL_FACTORY(ToolWithOutput)

ToolWithOutput::ToolWithOutput(const std::string& aType, const std::string& aName, const IInterface* aParent) :
GaudiTool(aType, aName, aParent) {
  declareInterface<IToolWithOutput>(this);
  declareOutput("testOutput",m_particles);
}

ToolWithOutput::~ToolWithOutput() {}

StatusCode ToolWithOutput::initialize() {
  if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode ToolWithOutput::finalize() {
  if(GaudiTool::finalize().isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
StatusCode ToolWithOutput::saveOutput() {
  ParticleCollection* particles = new ParticleCollection();
  ParticleHandle particle = particles->create();
  m_particles.put(particles);
  return StatusCode::SUCCESS;
}
