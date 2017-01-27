#include "CreateMidVolLocalPhiPositions.h"

//FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"
#include "DetSegmentation/GridLocalPhi.h"

// EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "TGeoManager.h"

DECLARE_ALGORITHM_FACTORY(CreateMidVolLocalPhiPositions)

CreateMidVolLocalPhiPositions::CreateMidVolLocalPhiPositions(const std::string& name, ISvcLocator* svcLoc)
  : GaudiAlgorithm(name, svcLoc) {
  declareInput("caloCells", m_caloCells);
  declareOutput("caloPositionedHits", m_caloPositionedHits);
  declareProperty("readoutName", m_readoutName = "ECalHitsNew");
}

StatusCode CreateMidVolLocalPhiPositions::initialize() {

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

  return sc;
}

StatusCode CreateMidVolLocalPhiPositions::execute() {

  //Get the input caloHits collection
  const fcc::CaloHitCollection* calocells = m_caloCells.get();
  debug() << "Input caloCells collection size: " << calocells->size() << endmsg;

  //Initialize output CaloClusterCollection
  auto edmPositionedHitCollection = m_caloPositionedHits.createAndPut();

  uint64_t cellid = 0;
  DD4hep::Geometry::VolumeManager volman = m_geoSvc->lcdd()->volumeManager();
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();

  // get local Phi segmentation
  DD4hep::DDSegmentation::GridLocalPhi* segmentation;
  segmentation = dynamic_cast<DD4hep::DDSegmentation::GridLocalPhi*>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if (segmentation == nullptr) {
    error() << "There is no phi-eta segmentation!!!!" << endmsg;
    return StatusCode::FAILURE;
  }

  // Loop though hits, retrieve volume position from cellID
  for (const auto& cell : *calocells) {
    cellid = cell.core().cellId;
    auto translation = volman.worldTransformation(cellid).GetTranslation(); // returns in cm

    // take into account segmentation
    auto numCells = det::utils::numberOfCells(cellid, { translation[0], translation[1], translation[2] }, *segmentation);
    auto volumePhiHalfWidth = det::utils::envelopePhiSize(cellid, { translation[0], translation[1], translation[2] }) / 2.;
    double xSegm = translation[0];
    double ySegm = translation[1];
    if( numCells > 0 ) {
      xSegm = translation[0] * cos(volumePhiHalfWidth / numCells) - translation[1] * sin(volumePhiHalfWidth / numCells);
      ySegm = translation[0] * sin(volumePhiHalfWidth / numCells) + translation[1] * cos(volumePhiHalfWidth / numCells);
    }
    auto edmPos = fcc::Point();
    edmPos.x = xSegm * 10.;
    edmPos.y = ySegm * 10.;
    edmPos.z = translation[2] * 10.;

    auto positionedHit = edmPositionedHitCollection->create(edmPos, cell.core());

    // Debug information about cells
    decoder->setValue(cellid);
    verbose() << decoder->valueString() << " \tenergy "<< cell.core().energy << "\ncellID " << cellid << endmsg;
    debug() << "translation of cell volume (mm) : \t" << translation[0] * 10. << "\t" << translation[1] * 10. << "\t" << translation[2] * 10. << endmsg;
    debug() << "additional shift in segmentation cell (mm) : \t" << xSegm * 10. << "\t" << ySegm * 10. << "\t" << translation[2] * 10. << endmsg;

  }
  debug() << "Output Hit Positions collection size: " << edmPositionedHitCollection->size() << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode CreateMidVolLocalPhiPositions::finalize() {
  return GaudiAlgorithm::finalize();
}
