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
  ParticleCollection* particles = new ParticleCollection();
  ParticleHandle particle = particles->create();
  m_particles.put(particles);
  return StatusCode::SUCCESS;
}
