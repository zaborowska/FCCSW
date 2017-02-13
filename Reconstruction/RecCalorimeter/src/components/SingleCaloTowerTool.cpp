#include "SingleCaloTowerTool.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"

// datamodel
#include "datamodel/CaloHitCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"

DECLARE_TOOL_FACTORY(SingleCaloTowerTool)

SingleCaloTowerTool::SingleCaloTowerTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ITowerTool>(this);
  declareInput("cells", m_cells, "calo/cells");
  declareProperty("readoutName", m_readoutName);
  declareProperty("fieldNames", m_fieldNames);
  declareProperty("fieldValues", m_fieldValues);
  declareProperty("radiusForPosition", m_innerRadius = 0.);
  declareProperty("deltaEtaTower", m_deltaEtaTower = 0.01);
  declareProperty("deltaPhiTower", m_deltaPhiTower = 0.01);
  // needed for AlgTool wit output/input until it appears in Gaudi AlgTool constructor
  declareProperty("DataInputs", inputDataObjects());
  declareProperty("DataOutputs", outputDataObjects());
}

StatusCode SingleCaloTowerTool::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if (m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_readoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  // retrieve PhiEta segmentation
  m_segmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if (m_segmentation == nullptr) {
    error() << "There is no phi-eta segmentation." << endmsg;
    return StatusCode::FAILURE;
  }
  // get the ID of the volume for which the cells are counted
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  if (m_fieldNames.size() != m_fieldValues.size()) {
    error() << "Volume readout field descriptors (names and values) have different size. " << endmsg;
    return StatusCode::FAILURE;
  }
  decoder->setValue(0);
  for (unsigned int it = 0; it < m_fieldNames.size(); it++) {
    (*decoder)[m_fieldNames[it]] = m_fieldValues[it];
  }
  m_volumeId = decoder->getValue();
  info() << "Reconstruction in volume with ID " << m_volumeId << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode SingleCaloTowerTool::finalize() { return GaudiTool::finalize(); }

std::array<uint, 2> SingleCaloTowerTool::prepareTowers(uint m_nEtaFinal, uint m_nPhiFinal) {
  // check if segmentation contains info about existing cells
  // this information is filled if RedoSegmentation algorithm was run
  int minEtaId = abs(m_segmentation->minEtaExisting());
  int maxEtaId = abs(m_segmentation->maxEtaExisting());
  if( minEtaId != 0 || maxEtaId != 0 ) {
    m_nEtaTower = (minEtaId > maxEtaId ? 2*minEtaId+1: 2*maxEtaId+1);
  } else {
    // if segmentation was already used in the simulation, take dimensions from geometry
    // true if sensitive volume is cylindrical
    auto numOfCells = det::utils::numberOfCells(m_volumeId, *m_segmentation);
    m_nEtaTower = numOfCells[1];
  }
  // number of phi bins take directly from segmentation
  m_nPhiTower = m_segmentation->phiBins();
  // set the minimal size to be of final window size
  if( m_nPhiTower < m_nPhiFinal ) {
    m_nPhiTower = m_nPhiFinal;
  }
  if( m_nEtaTower < m_nEtaFinal ) {
    m_nEtaTower = m_nEtaFinal;
  }
  if (m_nPhiTower % 2 == 0 || m_nEtaTower % 2 == 0) {
    error() << "Number of segmentation bins must be an odd number. See detector documentation for more details."
            << endmsg;
  }
  return {m_nEtaTower, m_nPhiTower};
}

uint SingleCaloTowerTool::buildTowers(std::vector<std::vector<float>>& aTowers) {
  // Get the input collection with cells from simulation + digitisation (after calibration and with noise)
  const fcc::CaloHitCollection* cells = m_cells.get();
  debug() << "Input Hit collection size: " << cells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  int iPhi = 0, iEta = 0;
  float etaCell = 0;
  for (const auto& ecell : *cells) {
    // find to which tower the cell belongs
    etaCell = m_segmentation->eta(ecell.core().cellId);
    iEta = idEta(etaCell);
    iPhi = idPhi(m_segmentation->phi(ecell.core().cellId));
    // save transverse energy
    aTowers[iEta][iPhi] += ecell.core().energy / cosh(etaCell);
  }
  return cells->size();
}

uint SingleCaloTowerTool::idEta(float aEta) const {
  // shift Ids so they start at 0 (segmentation returns IDs that may be from -N to N)
  // for segmentation in eta the middle cell has its centre at eta=0 (segmentation offset = 0)
  return floor((aEta + m_deltaEtaTower / 2.) / m_deltaEtaTower) + floor(m_nEtaTower / 2);
}

uint SingleCaloTowerTool::idPhi(float aPhi) const {
  // shift Ids so they start at 0 (segmentation returns IDs that may be from -N to N)
  // for segmentation in phi the middle cell has its centre at phi=0 (segmentation offset = 0)
  return floor((aPhi + m_deltaPhiTower / 2.) / m_deltaPhiTower) + floor(m_nPhiTower / 2);
}

float SingleCaloTowerTool::eta(int aIdEta) const {
  // middle of the tower
  return (aIdEta - (m_nEtaTower - 1) / 2) * m_deltaEtaTower;
}

float SingleCaloTowerTool::phi(int aIdPhi) const {
  // middle of the tower
  return (aIdPhi - (m_nPhiTower - 1) / 2) * m_deltaPhiTower;
}

void SingleCaloTowerTool::matchCells(float eta, float phi, uint halfEtaFin, uint halfPhiFin, fcc::CaloCluster& aEdmCluster) {
  const fcc::CaloHitCollection* cells = m_cells.get();
  for (const auto& cell : *cells) {
    float etaCell = m_segmentation->eta(cell.core().cellId);
    float phiCell = m_segmentation->phi(cell.core().cellId);
    if ((abs(idEta(etaCell) - idEta(eta)) <= halfEtaFin) &&
        (abs(idPhi(phiCell) - idPhi(phi)) <= halfPhiFin)) {
       aEdmCluster.addhits(cell);
    }
  }
  return;
}

float SingleCaloTowerTool::radiusForPosition() const {
   if( m_innerRadius ) {
      return m_innerRadius;
   }
   // if radius for position calculation is not defined bin properties, take inner radius from sensitive volume dimensions
   auto tubeSizes = det::utils::tubeDimensions(m_volumeId);
   return tubeSizes.x() * 10;// cm to mm
}
