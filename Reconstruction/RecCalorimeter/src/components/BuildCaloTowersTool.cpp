#include "BuildCaloTowersTool.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// DD4hep
#include "DD4hep/LCDD.h"

#include "TMath.h"

DECLARE_TOOL_FACTORY(BuildCaloTowersTool)

BuildCaloTowersTool::BuildCaloTowersTool(const std::string& type,const std::string& name, const IInterface* parent) 
  : GaudiTool(type, name, parent)
{
  declareInterface<IBuildCaloTowersTool>(this);
  declareProperty("deltaEtaTower", m_deltaEtaTower=0.01);
  declareProperty("deltaPhiTower", m_deltaPhiTower=2*Gaudi::Units::pi/628);
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

std::unordered_map<std::pair<int,int>, float, boost::hash<std::pair<int, int>>> BuildCaloTowersTool::buildTowers(const fcc::CaloHitCollection& aCells) {

  //Loop through a collection of calorimeter cells and build calo towers (unordered_map)
  std::unordered_map<std::pair<int,int>, float, boost::hash<std::pair<int, int>>> caloTowers;
  float weight = 1.0;

  for (const auto& ecell : aCells) {
    //Find to which tower the cell belongs (defined by index ieta and iphi)
    std::pair<int, int> ibin;
    findTower(ecell, ibin);
    auto it = caloTowers.find(ibin);
    //Weight cell energy (cell size bigger than tower size -> cell energy divided between more towers) -> TODO
    if (ecell.core().energy>100) {
      debug()<< "ibin "<< ibin.first << " , " << ibin.second << " weight " << weight << endmsg; 
    }
    float cellEnergy = ecell.core().energy;
    //If the tower does not exist, add a new one
    if (it == caloTowers.end()) {
      caloTowers[ibin] = cellEnergy;
    }
    //If the tower exists, add the energy in the tower
    else {
      it->second += cellEnergy;
    } 
  }
  for (auto it : caloTowers) { 
    if (it.second>500) {
      debug()<< "ieta,iphi " << it.first << " energy " << it.second << endmsg;
    }
  }

  return caloTowers;

}


StatusCode BuildCaloTowersTool::finalize() {
  StatusCode sc = GaudiTool::finalize();
  return sc;
}

void BuildCaloTowersTool::findTower(const fcc::CaloHit& aCell, std::pair<int, int>& itower) {
  //cell coordinates
  float eta = m_segmentation->eta(aCell.core().cellId);
  float phi = m_segmentation->phi(aCell.core().cellId);
  //TODO: use cell sizes to split cell to more towers if size bigger than tower segmentation
  /*
  //cell sizes
  float deltaEtaCell = m_segmentation->gridSizeEta();
  float deltaPhiCell = 2*Gaudi::Units::pi/(float)m_segmentation->phiBins();
  //cell edges
  float etaMinCell = eta-deltaEtaCell/2.;
  float etaMaxCell = eta+deltaEtaCell/2.;
  float phiMinCell = phi-deltaPhiCell/2.;
  float phiMaxCell = phi+deltaPhiCell/2.;
  */

  itower.first = floor(eta/m_deltaEtaTower);
  itower.second = floor(phi/m_deltaPhiTower);

  if (aCell.core().energy>100) {
    debug()<< "eta " << eta << " phi " << phi << " ibin "<< itower.first << " , " << itower.second << endmsg; 
  }

}
