#include "CreatePositionedHit.h"

//FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"

DECLARE_ALGORITHM_FACTORY(CreatePositionedHit)

CreatePositionedHit::CreatePositionedHit(const std::string& name, ISvcLocator* svcLoc)
: GaudiAlgorithm(name, svcLoc) {
  declareInput("caloCells", m_caloCells,"caloCells");
  declareOutput("caloPositionedHits", m_caloPositionedHits,"caloPositionedHits");
  declareProperty("readoutName", m_readoutName="ECalHitsNew");
}

StatusCode CreatePositionedHit::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  m_geoSvc = service ("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if(m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error()<<"Readout <<"<<m_readoutName<<">> does not exist."<<endmsg;
    return StatusCode::FAILURE;
  }

  // Get PhiEta segmentation
  m_segmentation = dynamic_cast<DD4hep::DDSegmentation::GridRPhiEta*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if(m_segmentation == nullptr) {
    error()<<"There is no r-phi-eta segmentation."<<endmsg;
    return StatusCode::FAILURE;
  }

  return sc;
}

StatusCode CreatePositionedHit::execute() {

  //Get the input caloHits collection
  const fcc::CaloHitCollection* calocells = m_caloCells.get();
  debug() << "Input caloCells collection size: " << calocells->size() << endmsg;

  //Initialize output CaloClusterCollection
  auto edmPositionedHitCollection = m_caloPositionedHits.createAndPut();

  //Intialize value of the half size in r
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  //Loop though CaloHits, calculate position from cellID, create and fill information in a CaloCluster
  for (const auto& cell : *calocells) {
    uint64_t id = cell.core().cellId;

    decoder->setValue(id);
    //Global position of the cell
    auto position =  m_segmentation->positionFromREtaPhi( m_segmentation->r(id), m_segmentation->eta(id), m_segmentation->phi(id) );
    auto edmPos = fcc::Point();
    edmPos.x = position.x() * 10.;
    edmPos.y = position.y() * 10.;
    edmPos.z = position.z() * 10.;

    auto positionedHit = edmPositionedHitCollection->create(edmPos, cell.core());

    //Debug information about cells
    if ( msgLevel ( MSG::DEBUG ) ) {
      double phi = m_segmentation->phi(id);
      debug() << decoder->valueString()<< "    phi   " << m_segmentation->phi(id) << "cellID " << id
              <<" energy " << cell.core().energy << "    x    " << edmPos.x << "     y     "
              <<   edmPos.y << "     z     " <<  edmPos.z  << endmsg;
    }
  }
  debug() << "Output CaloCluster collection size: " << edmPositionedHitCollection->size() << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode CreatePositionedHit::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
