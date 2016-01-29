#include "ToolWithOutput.h"

DECLARE_TOOL_FACTORY(ToolWithOutput)

ToolWithOutput::ToolWithOutput(const std::string& aType, const std::string& aName, const IInterface* aParent) :
GaudiTool(aType, aName, aParent) {
  declareInterface<IToolWithOutput>(this);
  declareOutput("testOutput",m_particles);
  declareProperty( "DataInputs", inputDataObjects());
  declareProperty( "DataOutputs", outputDataObjects());
}

ToolWithOutput::~ToolWithOutput() {}

StatusCode ToolWithOutput::initialize() {
  return GaudiTool::initialize();
}

StatusCode ToolWithOutput::finalize() {
  return GaudiTool::finalize();
}
StatusCode ToolWithOutput::saveOutput() {
  warning()<<"ToolWithOutput::saveOutput 1"<<endmsg;
  ParticleCollection* particles = new ParticleCollection();
  warning()<<"ToolWithOutput::saveOutput 2"<<endmsg;
  ParticleHandle particle = particles->create();
  warning()<<"ToolWithOutput::saveOutput 3"<<endmsg;
  m_particles.put(particles);
  warning()<<"ToolWithOutput::saveOutput 4"<<endmsg;
  return StatusCode::SUCCESS;
}
