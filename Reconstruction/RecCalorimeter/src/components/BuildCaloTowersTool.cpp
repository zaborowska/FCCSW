#include "BuildCaloTowersTool.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// DD4hep
#include "DD4hep/LCDD.h"

DECLARE_TOOL_FACTORY(BuildCaloTowersTool)

BuildCaloTowersTool::BuildCaloTowersTool(const std::string& type,const std::string& name, const IInterface* parent) 
  : GaudiTool(type, name, parent)
{
  declareInterface<IBuildCaloTowersTool>(this);
  declareProperty("deltaEtaTower", m_deltaEtaTower=0.01);
  declareProperty("deltaPhiTower", m_deltaPhiTower=0.01);
  declareProperty("readoutName", m_readoutName="ECalHitsPhiEta");
}

BuildCaloTowersTool::~BuildCaloTowersTool()
{
}

StatusCode BuildCaloTowersTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
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
  m_segmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if(m_segmentation == nullptr) {
    error()<<"There is no phi-eta segmentation."<<endmsg;
    return StatusCode::FAILURE;
  }

  return sc;
}

std::unordered_map<int, float>  BuildCaloTowersTool::buildTowers(const fcc::CaloHitCollection& aCells) {
  //Loop through a vector with calorimeter cells and build calo towers

  std::unordered_map<int, float> caloTowers;

  float weight = 1.0;
  for (const auto& ecell : aCells) {
    //Find to which tower the cell belongs (defined by index ieta and iphi)
    std::pair<int, int> ibin = findBin(ecell, weight);
    //Find a tower with identifier (ieta, iphi)
    auto it = caloTowers.find(ibin.first);
    //Weight cell energy (cell size bigger than tower size -> cell energy divided between more towers)
    float cellEnergy = ecell.core().energy*weight;
    //If the tower does not exist, add a new one
    if (it == caloTowers.end()) {
      caloTowers[ibin.first] = cellEnergy;
    }
    //If the tower exists, add the energy in the tower
    else {
      it->second += cellEnergy;
    } 
  }

  for (auto it : caloTowers) { 
    if (it.second>1000) {
      debug()<< "ieta " << it.first << " energy " << it.second << endmsg;
    }
  }

  return caloTowers;

}

StatusCode BuildCaloTowersTool::finalize() {
  StatusCode sc = GaudiTool::finalize();
  return sc;
}

std::pair<int, int> BuildCaloTowersTool::findBin(const fcc::CaloHit& aCell, float& weight) {
  std::pair<int, int> ibin(0,0);
  
  float eta = m_segmentation->eta(aCell.core().cellId);
  float phi = m_segmentation->phi(aCell.core().cellId);
  float deltaEtaCell = m_segmentation->gridSizeEta();
  float deltaPhiCell = 2*Gaudi::Units::pi/(float)m_segmentation->phiBins();

  //debug() << " deltaEta " << deltaEtaCell <<" deltaPhi "<< deltaPhiCell << endmsg; 

  ibin.first = floor(eta/m_deltaEtaTower);
  ibin.second = floor(phi/m_deltaPhiTower);

  //if cell size bigger than tower size, fill only a fraction of energy in the tower (weight is the scale)
  weight = 1.0;
  if (deltaEtaCell>m_deltaEtaTower) {
    weight = m_deltaEtaTower/deltaEtaCell;
  }
  if (deltaPhiCell>m_deltaPhiTower) {
    weight = m_deltaPhiTower/deltaPhiCell;
  }

  return ibin;
}
