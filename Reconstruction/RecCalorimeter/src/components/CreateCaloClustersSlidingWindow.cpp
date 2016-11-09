#include "CreateCaloClustersSlidingWindow.h"

#include <unordered_map>
#include <utility>
#include <boost/functional/hash.hpp>

// FCCSW
#include "DetInterface/IGeoSvc.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// datamodel
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloClusterCollection.h"

// ROOT
#include "TMath.h"

// DD4hep
#include "DD4hep/LCDD.h"

DECLARE_ALGORITHM_FACTORY(CreateCaloClustersSlidingWindow)

CreateCaloClustersSlidingWindow::CreateCaloClustersSlidingWindow(const std::string& name, ISvcLocator* svcLoc)
  : GaudiAlgorithm(name, svcLoc) {
  declareInput("cells", m_cells,"calo/cells");
  declareOutput("clusters", m_clusters,"calo/clusters");
  declareProperty("readoutName", m_readoutName="ECalHitsPhiEta");
  // for building towers
  declareProperty("deltaEtaTower", m_deltaEtaTower=0.01);
  declareProperty("deltaPhiTower", m_deltaPhiTower=2*Gaudi::Units::pi/628);
}

CreateCaloClustersSlidingWindow::~CreateCaloClustersSlidingWindow() {}

StatusCode CreateCaloClustersSlidingWindow::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
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
  info() << "CreateCaloClustersSlidingWindow initialized" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClustersSlidingWindow::execute() {
  // Get the input collection with cells from simulation + digitisation (after calibration and with noise)
  const fcc::CaloHitCollection* cells = m_cells.get();
  debug() << "Input Hit collection size: " << cells->size() << endmsg;

  // 1. Get calorimeter towers (calorimeter grid in eta phi, all layers merged)
  auto caloTowers = buildTowers(*cells);

  // 2. Find local maxima with sliding window

  // 3. Calculate barycentre position

  // 4. Remove duplicates

  // 5. Create final clusters

  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClustersSlidingWindow::finalize() {
  return GaudiAlgorithm::finalize();
}

std::unordered_map<std::pair<int,int>, float, boost::hash<std::pair<int, int>>> CreateCaloClustersSlidingWindow::buildTowers(const fcc::CaloHitCollection& aCells) {
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

void CreateCaloClustersSlidingWindow::findTower(const fcc::CaloHit& aCell, std::pair<int, int>& aTower) {
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

  aTower.first = floor(eta/m_deltaEtaTower);
  aTower.second = floor(phi/m_deltaPhiTower);

  if (aCell.core().energy>100) {
    debug()<< "eta " << eta << " phi " << phi << " ibin "<< aTower.first << " , " << aTower.second << endmsg;
  }
}
