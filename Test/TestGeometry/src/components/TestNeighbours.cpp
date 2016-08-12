#include "TestNeighbours.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/CaloHitCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"

DECLARE_ALGORITHM_FACTORY(TestNeighbours)

TestNeighbours::TestNeighbours(const std::string& aName, ISvcLocator* aSvcLoc):
  GaudiAlgorithm(aName, aSvcLoc) {
  declareProperty("readout",m_readoutName);
  declareInput("inhits", m_inHits,"hits/caloInHits");
}

TestNeighbours::~TestNeighbours() {}

StatusCode TestNeighbours::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_geoSvc = service ("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  m_decoder = m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation()->decoder();
  info() << "Bitfield: "<<m_decoder->fieldDescription() << endmsg;


  det::dummy();


  return StatusCode::SUCCESS;
}

StatusCode TestNeighbours::execute() {
  const fcc::CaloHitCollection* inHits = m_inHits.get();
  for(const auto& hit: *inHits) {
    debug() << "cell ID =" << hit.Core().Cellid << endmsg;
    debug() << "energy  =" << hit.Core().Energy << endmsg;

  }
  return StatusCode::SUCCESS;
}

StatusCode TestNeighbours::finalize() {
  return GaudiAlgorithm::finalize();
}
