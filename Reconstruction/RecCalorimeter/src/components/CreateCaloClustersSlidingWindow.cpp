#include "CreateCaloClustersSlidingWindow.h"

#include <unordered_map>
#include <utility>
#include <boost/functional/hash.hpp>

// FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"

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
  declareProperty("energyThreshold", m_energyThreshold = 3);
  // size of tower
  declareProperty("deltaEtaTower", m_deltaEtaTower=0.01);
  declareProperty("deltaPhiTower", m_deltaPhiTower=2*Gaudi::Units::pi/628);
  // window size (in units of tower)
  declareProperty("nEtaWindow", m_nEtaWindow = 5);
  declareProperty("nPhiWindow", m_nPhiWindow = 5);
  declareProperty("nEtaDuplicate", m_nEtaDuplicate = 2);
  declareProperty("nPhiDuplicate", m_nPhiDuplicate = 2);
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

  // 1. Get calorimeter towers (calorimeter grid in eta phi, all layers merged)
  prepareTowers();
  buildTowers();

  warning()<<"TOWERS size: "<<m_towers.size()<<endmsg;
  warning()<<"TOWERS expected size: "<<(m_highEtaTower - m_lowEtaTower + 1)*(m_highPhiTower - m_lowPhiTower + 1) <<endmsg;

  // 2. Find local maxima with sliding window
  std::unordered_map<std::pair<int,int>, float, boost::hash<std::pair<int, int>>> seeds;
  double windowEnergy = 0;
  for(int iEta = m_lowEtaTower + floor(m_nEtaWindow/2.); iEta <= m_highEtaTower - floor(m_nEtaWindow/2.); iEta++) {
    for(int iPhi = m_lowPhiTower + floor(m_nPhiWindow/2.); iPhi <= m_highPhiTower - floor(m_nPhiWindow/2.); iPhi++) {
      // calculate energy of each window
      // TODO full phi coverage so it should start in low edge and end in high edge
      for(int iEtaWindow = -floor(m_nEtaWindow/2.); iEtaWindow <= floor(m_nEtaWindow/2.); iEtaWindow++) {
        for(int iPhiWindow = -floor(m_nPhiWindow/2.); iPhiWindow <= floor(m_nPhiWindow/2.); iPhiWindow++) {
          windowEnergy += m_towers.find(std::pair<int,int>(iEta+iEtaWindow, iPhi+iPhiWindow))->second;
        }
      }
      if(windowEnergy>3) {
        seeds[std::make_pair(iEta,iPhi)] = windowEnergy;
        //debug()<< "ieta,iphi " << iEta << ", "<< iPhi << " energy " << windowEnergy << endmsg;
      }
      windowEnergy = 0;
    }
  }
  warning()<<"SEEDs size: "<<seeds.size()<<endmsg;

  //              // TODO add caching of energy (not to repeat all sums)
  //              // TODO add check for local maxima (check closest neighbours)

  // 3. Calculate barycentre position
  std::unordered_map<std::pair<int,int>, float, boost::hash<std::pair<int, int>>> preclusters;
  float posEta = 0;
  float posPhi = 0;
  float sumEnergy = 0;
  for(const auto& seed: seeds) {
    for(int iEtaWindow = -floor(m_nEtaWindow/2.); iEtaWindow <= floor(m_nEtaWindow/2.); iEtaWindow++) {
      for(int iPhiWindow = -floor(m_nPhiWindow/2.); iPhiWindow <= floor(m_nPhiWindow/2.); iPhiWindow++) {
        posEta += (seed.first.first+iEtaWindow) * (m_towers.find(std::pair<int,int>(seed.first.first+iEtaWindow, seed.first.second+iPhiWindow))->second);
        //warning()<<" eta += "<< (seed.first.first+iEtaWindow) <<" * "<<(m_towers.find(std::pair<int,int>(seed.first.first+iEtaWindow, seed.first.second+iPhiWindow))->second)<<endmsg;
        //warning()<<" phi += "<< (seed.first.second+iPhiWindow) <<" * "<<(m_towers.find(std::pair<int,int>(seed.first.second+iPhiWindow, seed.first.second+iPhiWindow))->second)<<endmsg;
        posPhi += (seed.first.second+iPhiWindow) * (m_towers.find(std::pair<int,int>(seed.first.first+iEtaWindow, seed.first.second+iPhiWindow))->second);
        sumEnergy += m_towers.find(std::pair<int,int>(seed.first.first+iEtaWindow, seed.first.second+iPhiWindow))->second;
      }
    }
    posEta /= sumEnergy;
    posPhi /= sumEnergy;
    auto iter = preclusters.find(std::make_pair(int(posEta), int(posPhi)));
    if(iter == preclusters.end()) {
      preclusters[std::make_pair(int(posEta), int(posPhi))] = seed.second;
    } else if (iter->second < seed.second) {
      preclusters[std::make_pair(int(posEta), int(posPhi))] = seed.second;
    }
    posEta = 0;
    posPhi = 0;
    sumEnergy=0;
  }
  warning()<<"PRECLUSTERS size: "<<preclusters.size()<<endmsg;

  // 4. Remove duplicates

  // 5. Create final clusters

  // temporary: write ID of eta to position.x, ID of phi to position.y and energy to position.z
  auto edmClusters = m_clusters.createAndPut();
  for(const auto& clu: preclusters) {
    auto edmCluster = edmClusters->create();
    auto& edmClusterCore = edmCluster.core();
    edmClusterCore.position.x = clu.first.first;
    edmClusterCore.position.y = clu.first.second;
    edmClusterCore.position.z = clu.second;
  }
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClustersSlidingWindow::finalize() {
  return GaudiAlgorithm::finalize();
}

void CreateCaloClustersSlidingWindow::prepareTowers() {
  auto tubeSizes = det::utils::tubeDimensions(m_segmentation->volumeID(m_cells.get()->at(0).cellId()));
  double maxEta = CLHEP::Hep3Vector(tubeSizes.x(), 0 , tubeSizes.z()).eta();
  m_lowEtaTower = -1.*ceil((maxEta-m_deltaEtaTower/2.)/m_deltaEtaTower);
  m_highEtaTower = ceil((maxEta-m_deltaEtaTower/2.)/m_deltaEtaTower);
  m_lowPhiTower = -1.*ceil((2*Gaudi::Units::pi-m_deltaPhiTower/2.)/m_deltaPhiTower);
  m_highPhiTower = ceil((2*Gaudi::Units::pi-m_deltaPhiTower/2.)/m_deltaPhiTower);

  // warning()<<" cellID: "<<m_cells.get()->at(0).cellId()<<endmsg;
  // warning()<<" volID: "<<m_segmentation->volumeID(m_cells.get()->at(0).cellId())<<endmsg;
  // warning()<<"TUBE size: "<<tubeSizes<<endmsg;
  // warning()<<"Eta size: "<<maxEta<<endmsg;
  // warning()<<"Eta N bins : "<<m_highEtaTower - m_lowEtaTower + 1<<endmsg;
  // warning()<<"PHi N bins : "<<m_highPhiTower - m_lowPhiTower + 1<<endmsg;

  m_towers.reserve((m_highEtaTower - m_lowEtaTower)*(m_highPhiTower - m_lowPhiTower));
  for(int iEta = m_lowEtaTower; iEta <= m_highEtaTower; iEta++) {
    for(int iPhi = m_lowPhiTower; iPhi <= m_highPhiTower; iPhi++) {
      m_towers[std::make_pair(iEta,iPhi)] = 0;
    }
  }
}

void CreateCaloClustersSlidingWindow::buildTowers() {
  // Get the input collection with cells from simulation + digitisation (after calibration and with noise)
  const fcc::CaloHitCollection* cells = m_cells.get();
  debug() << "Input Hit collection size: " << cells->size() << endmsg;
  //Loop through a collection of calorimeter cells and build calo towers (unordered_map)
  float weight = 1.0;

  for (const auto& ecell : *cells) {
    //Find to which tower the cell belongs (defined by index ieta and iphi)
    std::pair<int, int> ibin;
    findTower(ecell, ibin);
    auto it = m_towers.find(ibin);
    //Weight cell energy (cell size bigger than tower size -> cell energy divided between more towers) -> TODO
    // if (ecell.core().energy>100) {
    //   debug()<< "ibin "<< ibin.first << " , " << ibin.second << " weight " << weight << endmsg;
    // }
    float cellEnergy = ecell.core().energy;
    //If the tower does not exist, add a new one
    if (it == m_towers.end()) {
      m_towers[ibin] = cellEnergy;
      warning()<<"creating new one!"<<endmsg;
    }
    //If the tower exists, add the energy in the tower
    else {
      it->second += cellEnergy;
    }
  }
  // for (auto it : m_towers) {
  //   if (it.second>500) {
  //     debug()<< "ieta,iphi " << it.first << " energy " << it.second << endmsg;
  //   }
  // }
  return;
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

  // if (aCell.core().energy>100) {
  //   debug()<< "eta " << eta << " phi " << phi << " ibin "<< aTower.first << " , " << aTower.second << endmsg;
  // }
}
