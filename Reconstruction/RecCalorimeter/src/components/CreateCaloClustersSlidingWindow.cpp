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
  // size of tower
  declareProperty("deltaEtaTower", m_deltaEtaTower=0.01);
  declareProperty("deltaPhiTower", m_deltaPhiTower=2*Gaudi::Units::pi/628);
  // window size (in units of tower)
  declareProperty("nEtaWindow", m_nEtaWindow = 5);
  declareProperty("nPhiWindow", m_nPhiWindow = 5);
  declareProperty("energyThreshold", m_energyThreshold = 3);
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

  // 2. Find local maxima with sliding window
  std::unordered_map<std::pair<int,int>, float, boost::hash<std::pair<int, int>>> seeds;
  int halfEtaWin =  floor(m_nEtaWindow/2.);
  int halfPhiWin =  floor(m_nPhiWindow/2.);
  // calculate the sum of first m_nEtaWindow bins in eta, for each phi tower
  std::vector<float> sumOverEta (m_nPhiTower, 0);
  for(int iEta = 0; iEta < m_nEtaWindow; iEta++) {
    std::transform (sumOverEta.begin(), sumOverEta.end(), m_towers[iEta].begin(), sumOverEta.begin(), std::plus<float>());
  }

  // loop over all Eta slices starting at the half of the first window
  float sumWindow = 0;
  warning()<<"RANGE: "<<halfEtaWin<<"; "<< m_nEtaTower - halfEtaWin - 1<<endmsg;
  for(int iEta = halfEtaWin; iEta < m_nEtaTower - halfEtaWin; iEta++) {
    info()<<"iEta = "<<iEta<<endmsg;
    // one slice in eta = window, now only sum over window in phi
    // TODO handle corner cases (full phi coverage)
    // sum first window in phi
    for(int iPhiWindow = 0; iPhiWindow <= 2*halfPhiWin; iPhiWindow++) {
      sumWindow += sumOverEta[iPhiWindow];
    }
    for(int iPhi = halfPhiWin; iPhi < m_nPhiTower - halfPhiWin ; iPhi++) {
      if(iEta == 2)
        info()<<"    iPhi = "<<iPhi<<endmsg;
      // if energy is over threshold, test if it is a local maximum
      if(sumWindow > m_energyThreshold) {
        //TODO test local maximum in phi
        //TODO test local maximum in eta
        seeds[std::make_pair(iEta,iPhi)] = sumWindow;
      }
      if(iPhi< m_nPhiTower - halfPhiWin - 1) {
        // finish processing that window, shift window to the next phi tower
        // substract first phi tower in current window
        sumWindow -= sumOverEta[iPhi-halfPhiWin];
        // add next phi tower to the window
        sumWindow += sumOverEta[iPhi+halfPhiWin+1];
      }
    }
    sumWindow = 0;
    // finish processing that slice, shift window to next eta tower
    if(iEta < m_nEtaTower - halfEtaWin - 1) {
      // substract first eta slice in current window
      std::transform (sumOverEta.begin(), sumOverEta.end(), m_towers[iEta-halfEtaWin].begin(), sumOverEta.begin(), std::minus<float>());
      // add next eta slice to the window
      std::transform (sumOverEta.begin(), sumOverEta.end(), m_towers[iEta+halfEtaWin+1].begin(), sumOverEta.begin(), std::plus<float>());
    }
  }
  debug()<<"SEEDS size: "<<seeds.size()<<endmsg;

  //              // TODO add caching of energy (not to repeat all sums)
  //              // TODO add check for local maxima (check closest neighbours)

  // 3. Calculate barycentre position
  std::unordered_map<std::pair<int,int>, float, boost::hash<std::pair<int, int>>> preclusters;
  float posEta = 0;
  float posPhi = 0;
  float sumEnergy = 0;
  float towEnergy = 0;
  for(const auto& seed: seeds) {
    for(int iEta = seed.first.first-halfEtaWin; iEta <= seed.first.first+halfEtaWin; iEta++) {
      for(int iPhi = seed.first.second-halfPhiWin; iPhi <= seed.first.second+halfPhiWin; iPhi++) {
        towEnergy = m_towers[iEta][iPhi];
        posEta += iEta * towEnergy;
        posPhi += iPhi * towEnergy;
        sumEnergy += towEnergy;
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
  debug()<<"PRECLUSTERS size: "<<preclusters.size()<<endmsg;

  // 4. Remove duplicates

  // 5. Create final clusters

  auto edmClusters = m_clusters.createAndPut();
  for(const auto& clu: preclusters) {
    auto edmCluster = edmClusters->create();
    auto& edmClusterCore = edmCluster.core();
    edmClusterCore.position.x = clu.first.first;
    edmClusterCore.position.y = clu.first.second;
    edmClusterCore.position.z = clu.second;
  }
  // for(const auto& clu: m_towers) {
  //   for(uint i=0; i<clu.second.size(); i++) {
  //     if(clu.second[i]>0) {
  //       auto edmCluster = edmClusters->create();
  //       auto& edmClusterCore = edmCluster.core();
  //       edmClusterCore.position.x = clu.first;
  //       edmClusterCore.position.y = i;
  //       edmClusterCore.position.z = clu.second[i];
  //     }
  //   }
  // }
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClustersSlidingWindow::finalize() {
  return GaudiAlgorithm::finalize();
}

void CreateCaloClustersSlidingWindow::prepareTowers() {
  auto tubeSizes = det::utils::tubeDimensions(m_segmentation->volumeID(m_cells.get()->at(0).cellId()));
  double maxEta = CLHEP::Hep3Vector(tubeSizes.x(), 0 , tubeSizes.z()).eta();
  m_nEtaTower = 2.*ceil((maxEta-m_deltaEtaTower/2.)/m_deltaEtaTower) + 1;
  m_nPhiTower = 2*ceil((2*Gaudi::Units::pi-m_deltaPhiTower/2.)/m_deltaPhiTower) + 1;

  for(int iEta = 0; iEta < m_nEtaTower; iEta++) {
    m_towers[iEta].assign(m_nPhiTower, 0);
  }
}

void CreateCaloClustersSlidingWindow::buildTowers() {
  // Get the input collection with cells from simulation + digitisation (after calibration and with noise)
  const fcc::CaloHitCollection* cells = m_cells.get();
  debug() << "Input Hit collection size: " << cells->size() << endmsg;
  //Loop through a collection of calorimeter cells and build calo towers (unordered_map)

  for (const auto& ecell : *cells) {
    //Find to which tower the cell belongs (defined by index ieta and iphi)
    std::pair<int, int> ibin;
    findTower(ecell, ibin);
    m_towers[ibin.first][ibin.second] += ecell.core().energy;
  }
  return;
}

void CreateCaloClustersSlidingWindow::prepareTestTowers() {
  m_nEtaTower = 20;
  m_nPhiTower = 20;

  for(int iEta = 0; iEta < m_nEtaTower; iEta++) {
    m_towers[iEta].assign(m_nPhiTower, 0);
  }
}

void CreateCaloClustersSlidingWindow::buildTestTowers() {
  m_towers[10][15] = 5000;
  m_towers[1][15] = 5000;
  m_towers[7][7] = 3000;
  m_towers[16][6] = 3000;
  m_towers[17][1] = 7000;
  return;
}


void CreateCaloClustersSlidingWindow::findTower(const fcc::CaloHit& aCell, std::pair<int, int>& aTower) {
  //cell coordinates
  float eta = m_segmentation->eta(aCell.core().cellId);
  float phi = m_segmentation->phi(aCell.core().cellId);

  // shift Ids so they start at 0 (segmentation returns IDs that may be from -N to N)
  aTower.first = floor(eta/m_deltaEtaTower) + (m_nEtaTower-1)/2;
  aTower.second = floor(phi/m_deltaPhiTower) + (m_nPhiTower-1)/2;
}
