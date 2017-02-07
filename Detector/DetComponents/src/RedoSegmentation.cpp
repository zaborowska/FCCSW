#include "RedoSegmentation.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetSegmentation/GridPhiEta.h"

// datamodel
#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"
#include "datamodel/Point.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DDSegmentation/Segmentation.h"

DECLARE_ALGORITHM_FACTORY(RedoSegmentation)

RedoSegmentation::RedoSegmentation(const std::string& aName, ISvcLocator* aSvcLoc):
GaudiAlgorithm(aName, aSvcLoc)
{
  declareInput("inhits", m_inHits,"hits/positionedCaloHits");
  declareOutput("outhits", m_outHits,"hits/caloOutHits");
  declareProperty("oldReadoutName", m_oldReadoutName = "");
  declareProperty("oldSegmentationIds", m_oldIdentifiers);
  declareProperty("newReadoutName", m_newReadoutName = "");
  declareProperty("debugPrint", m_debugPrint = 10);
}

RedoSegmentation::~RedoSegmentation() {}

StatusCode RedoSegmentation::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;
  m_geoSvc = service ("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if(m_geoSvc->lcdd()->readouts().find(m_oldReadoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error()<<"Readout <<"<<m_oldReadoutName<<">> does not exist."<<endmsg;
    return StatusCode::FAILURE;
  }
  if(m_geoSvc->lcdd()->readouts().find(m_newReadoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error()<<"Readout <<"<<m_newReadoutName<<">> does not exist."<<endmsg;
    return StatusCode::FAILURE;
  }
  // Take readout, bitfield from GeoSvc
  m_oldDecoder = std::shared_ptr<DD4hep::DDSegmentation::BitField64>(
    m_geoSvc->lcdd()->readout(m_oldReadoutName).idSpec().decoder());
  // segmentation identifiers to be overwritten
  if(m_oldIdentifiers.size()==0) {
    // it is not an error, maybe no segmentation was used previously
    warning()<<"No previous segmentation identifiers. Volume ID may be recomputed incorrectly."<<endmsg;
  }
  // create detector identifiers (= all bitfield ids - segmentation ids)
  for(uint itField = 0; itField < m_oldDecoder->size(); itField++)
  {
    std::string field =  (*m_oldDecoder)[itField].name();
    auto iter = std::find( m_oldIdentifiers.begin(), m_oldIdentifiers.end(), field );
    if( iter == m_oldIdentifiers.end() )
    {
      m_detectorIdentifiers.push_back(field);
    }
  }
  // Take new segmentation from geometry service
  m_segmentation = m_geoSvc->lcdd()->readout(m_newReadoutName).segmentation().segmentation();
  // check if detector identifiers (old and new) agree
  std::vector<std::string> newFields;
  for(uint itField = 0; itField < m_segmentation->decoder()->size(); itField++) {
    newFields.push_back((*m_segmentation->decoder())[itField].name());
  }
  for(const auto& detectorField: m_detectorIdentifiers) {
    auto iter = std::find( newFields.begin(), newFields.end(), detectorField );
    if( iter == newFields.end() ) {
      error()<<"New readout does not contain field <<"<<detectorField<<">> that describes the detector ID."<<endmsg;
      return StatusCode::FAILURE;
       }
  }
  info()<<"Redoing the segmentation."<<endmsg;
  info()<<"Old bitfield:\t"<<m_oldDecoder->fieldDescription()<<endmsg;
  info()<<"New bitfield:\t"<<m_segmentation->decoder()->fieldDescription()<<endmsg;
  info()<<"New segmentation is of type:\t"<<m_segmentation->type()<<endmsg;

  return StatusCode::SUCCESS;
}

StatusCode RedoSegmentation::execute() {
  const auto inHits = m_inHits.get();
  auto outHits = m_outHits.createAndPut();
  // loop over positioned hits to get the energy deposits: position and cellID
  // cellID contains the volumeID that needs to be copied to the new id
  uint64_t oldid = 0;
  uint debugIter = 0;
  // if segmentation is of type eta-phi (used for sliding window reco), additionally fill info
  // about min/max existing fields
  DD4hep::DDSegmentation::GridPhiEta* phietasegm = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(m_segmentation);
  int minEtaId = 0;
  int maxEtaId = 0;
  int minPhiId = 0;
  int maxPhiId = 0;
  if(phietasegm != nullptr) {
    // set initial values to be cretainly larger/smaller than the first cell
    minPhiId = phietasegm->phiBins();
    maxPhiId = - phietasegm->phiBins();
    // take ridiculous large eta as the largest possible eta
    minEtaId = 100 / phietasegm->gridSizeEta();
    maxEtaId = - 100./ phietasegm->gridSizeEta();
  }
  for(const auto& hit: *inHits) {
    fcc::CaloHit newHit = outHits->create();
    newHit.energy(hit.energy());
    newHit.time(hit.time());
    m_oldDecoder->setValue(hit.cellId());
    if (debugIter < m_debugPrint) {
      debug() << "OLD: " << m_oldDecoder->valueString() << endmsg;
    }
    // factor 10 to convert mm to cm
    DD4hep::DDSegmentation::Vector3D position(hit.position().x / 10,
                                              hit.position().y / 10,
                                              hit.position().z / 10);
    // first calculate proper segmentation fields
    uint64_t newcellId = m_segmentation->cellID(position, position, volumeID(hit.cellId()));
    m_segmentation->decoder()->setValue(newcellId);
    // now rewrite all other fields (detector ID)
    for(const auto& detectorField: m_detectorIdentifiers) {
      oldid = (*m_oldDecoder)[detectorField];
      (*m_segmentation->decoder())[detectorField] = oldid;
    }
    newHit.cellId(m_segmentation->decoder()->getValue());
    if (debugIter < m_debugPrint) {
      debug() << "NEW: " << m_segmentation->decoder()->valueString() << endmsg;
      debugIter++;
    }
    // if that is et-phi grid, fill in data about existing min/max Ids
    if(phietasegm != nullptr) {
      int etaId = (*phietasegm->decoder())["eta"];
      if(etaId < minEtaId) {
        minEtaId = etaId;
      }
      if(etaId > maxEtaId) {
        maxEtaId = etaId;
      }
      int phiId = (*phietasegm->decoder())["phi"];
      if(phiId < minPhiId) {
        minPhiId = phiId;
      }
      if(phiId > maxPhiId) {
        maxPhiId = phiId;
      }
    }
  }
  // if that is et-phi grid, fill in data about existing min/max Ids
  if(inHits->size() > 0 && phietasegm != nullptr) {
    phietasegm->setMinEtaExisting(minEtaId);
    phietasegm->setMaxEtaExisting(maxEtaId);
    phietasegm->setMinPhiExisting(minPhiId);
    phietasegm->setMaxPhiExisting(maxPhiId);
    info() << "Filling additional information in eta-phi segmentation: eta  " << minEtaId << " to " << maxEtaId
           << "\tphi " << minPhiId << " to " << maxPhiId << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode RedoSegmentation::finalize() {
  return GaudiAlgorithm::finalize();
}

uint64_t RedoSegmentation::volumeID(uint64_t aCellId) const {
  m_oldDecoder->setValue(aCellId);
  for (const auto& identifier: m_oldIdentifiers) {
    (*m_oldDecoder)[identifier] = 0;
  }
  return m_oldDecoder->getValue();
}
