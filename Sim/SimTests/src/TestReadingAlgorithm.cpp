#include "TestReadingAlgorithm.h"

DECLARE_COMPONENT(TestReadingAlgorithm)

TestReadingAlgorithm::TestReadingAlgorithm(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc) {
  declareInput("testInput",m_particles);
}

StatusCode TestReadingAlgorithm::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode TestReadingAlgorithm::execute() {
  const ParticleCollection* particles = m_particles.get();
  for(const auto& part : *particles) {
    const BareParticle& core = part.read().Core;
    info()<<"\t particle momentum = ("<<core.P4.Px<<"\t"<<core.P4.Py<<"\t"<<core.P4.Pz<<")\tGeV"<<endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode TestReadingAlgorithm::finalize() {
  return GaudiAlgorithm::finalize();
}
