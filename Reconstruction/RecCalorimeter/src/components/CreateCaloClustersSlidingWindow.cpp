#include "CreateCaloClustersSlidingWindow.h"

#include <unordered_map>
#include <utility>
#include <boost/functional/hash.hpp>

// FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// Root
#include "TMath.h"

// datamodel
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloClusterCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"

DECLARE_ALGORITHM_FACTORY(CreateCaloClustersSlidingWindow)

CreateCaloClustersSlidingWindow::CreateCaloClustersSlidingWindow(const std::string& name, ISvcLocator* svcLoc)
: GaudiAlgorithm(name, svcLoc) {
  declareInput("cells", m_cells,"calo/cells");
  declareOutput("clusters", m_clusters,"calo/clusters");
  declareProperty("readoutName", m_readoutName="ECalHitsPhiEta");
  // size of tower
  declareProperty("deltaEtaTower", m_deltaEtaTower = 0.01);
  declareProperty("deltaPhiTower", m_deltaPhiTower = 2*Gaudi::Units::pi/628);
  // window size (in units of tower)
  declareProperty("nEtaWindow", m_nEtaWindow = 5);
  declareProperty("nPhiWindow", m_nPhiWindow = 5);
  declareProperty("nEtaPosition", m_nEtaPosition = 3);
  declareProperty("nPhiPosition", m_nPhiPosition = 3);
  declareProperty("nEtaDuplicates", m_nEtaDuplicates = 2);
  declareProperty("nPhiDuplicates", m_nPhiDuplicates = 2);
  declareProperty("nEtaObject", m_nEtaObject = 5);
  declareProperty("nPhiObject", m_nPhiObject = 5);
  declareProperty("energyThreshold", m_energyThreshold = 3);
  declareProperty("energyPosThreshold", m_energyPosThreshold = 0.1);
  declareProperty("checkPhiLocalMax", m_checkPhiLocalMax = true);
  declareProperty("checkEtaLocalMax", m_checkEtaLocalMax = true);
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

  // 2. Find local maxima with sliding window, build preclusters
  int halfEtaWin =  floor(m_nEtaWindow/2.);
  int halfPhiWin =  floor(m_nPhiWindow/2.);
  // calculate the sum of first m_nEtaWindow bins in eta, for each phi tower
  std::vector<float> sumOverEta (m_nPhiTower, 0);
  // seeds (preclusters)
  std::vector<cluster> m_preClusters;
  int halfEtaPos =  floor(m_nEtaPosition/2.);
  int halfPhiPos =  floor(m_nPhiPosition/2.);
  float posEta = 0;
  float posPhi = 0;
  float sumEnergyPos = 0;

  for(int iEta = 0; iEta < m_nEtaWindow; iEta++) {
    std::transform (sumOverEta.begin(), sumOverEta.end(), m_towers[iEta].begin(), sumOverEta.begin(), std::plus<float>());
  }

  // loop over all Eta slices starting at the half of the first window
  float sumWindow = 0;
  float sumPhiSlicePrevEtaWin = 0;
  float sumPhiSliceNextEtaWin = 0;
  float sumFirstPhiSlice = 0;
  float sumLastPhiSlice = 0;
  bool toRemove = false;
  // one slice in eta = window, now only sum over window in phi
  for(int iEta = halfEtaWin; iEta < m_nEtaTower - halfEtaWin; iEta++) {
    // sum first window in phi - full phi coverage so start with phi bins at the end...
    for(int iPhiWindow = m_nPhiTower - halfPhiWin; iPhiWindow < m_nPhiTower; iPhiWindow++) {
      if(iEta == halfEtaWin) info()<<" SUMMING BIN: "<<iPhiWindow<<endmsg;
      sumWindow += sumOverEta[iPhiWindow];
    }
    // ... and add first bins
    for(int iPhiWindow = 0; iPhiWindow <= halfPhiWin; iPhiWindow++) {
      if(iEta == halfEtaWin) info()<<" SUMMING BIN: "<<iPhiWindow<<endmsg;
      sumWindow += sumOverEta[iPhiWindow];
    }
    // loop over all the phi slices
    for(int iPhi = 0; iPhi < m_nPhiTower; iPhi++) {
      if(sumWindow > m_energyThreshold) {
        // test local maximum in phi
        if(m_checkPhiLocalMax) {
          // check closest neighbour on the right
          if(sumOverEta[iPhi-halfPhiWin] < sumOverEta[iPhi+halfPhiWin+1]) {
            toRemove = true;
          }
          // check closest neighbour on the left
          if(sumOverEta[iPhi+halfPhiWin] < sumOverEta[iPhi-halfPhiWin-1]) {
            toRemove = true;
          }
        }
        // test local maximum in eta (if it wasn't already marked as to be removed)
        if(m_checkEtaLocalMax && (!toRemove) ) {
          // check closest neighbour on the right
          for(int iPhiWindowLocalCheck = iPhi - halfPhiWin; iPhiWindowLocalCheck <= iPhi+halfPhiWin; iPhiWindowLocalCheck++) {
            sumPhiSlicePrevEtaWin += m_towers[iEta - halfEtaWin - 1][iPhiWindowLocalCheck];
            sumLastPhiSlice += m_towers[iEta + halfEtaWin][iPhiWindowLocalCheck];
          }
          if(sumPhiSlicePrevEtaWin > sumLastPhiSlice) {
            toRemove = true;
          }
          for(int iPhiWindowLocalCheck = iPhi - halfPhiWin; iPhiWindowLocalCheck <= iPhi+halfPhiWin; iPhiWindowLocalCheck++) {
            sumPhiSliceNextEtaWin += m_towers[iEta + halfEtaWin + 1][iPhiWindowLocalCheck];
            sumFirstPhiSlice += m_towers[iEta - halfEtaWin][iPhiWindowLocalCheck];
          }
          if(sumPhiSliceNextEtaWin > sumFirstPhiSlice) {
            toRemove = true;
          }
          sumFirstPhiSlice = 0;
          sumLastPhiSlice = 0;
          sumPhiSlicePrevEtaWin = 0;
          sumPhiSliceNextEtaWin = 0;
        }
        if (!toRemove) {
          // Build precluster
          // Calculate barycentre position (smaller window used to reduce noise influence)
          for(int ipEta = iEta-halfEtaPos; ipEta <= iEta+halfEtaPos; ipEta++) {
            for(int ipPhi = iPhi-halfPhiPos; ipPhi <= iPhi+halfPhiPos; ipPhi++) {
              posEta += (ipEta - m_nEtaTower/2. )* m_deltaEtaTower  * m_towers[ipEta][ipPhi];
              posPhi += (ipPhi - m_nPhiTower/2. )* m_deltaPhiTower * m_towers[ipEta][ipPhi];
              sumEnergyPos += m_towers[ipEta][ipPhi];
            }
          }
          // If non-zero energy in the cluster, add to pre-clusters (reduced size for pos. calculation -> energy in the core can be zero)
          if (sumEnergyPos>m_energyPosThreshold) {
            posEta /= sumEnergyPos;
            posPhi /= sumEnergyPos;
            cluster newPreCluster;
            newPreCluster.eta = posEta;
            newPreCluster.phi = posPhi;
            newPreCluster.transEnergy = sumWindow;
            newPreCluster.ieta = iEta;
            newPreCluster.iphi = iPhi;
            debug()<<"PRECLUSTERS: "<<newPreCluster.ieta << " " << newPreCluster.iphi<< " "<<newPreCluster.eta <<" "<<newPreCluster.phi<<" "<<newPreCluster.transEnergy<<endmsg;
            m_preClusters.push_back(newPreCluster);
          }
          posEta = 0;
          posPhi = 0;
          sumEnergyPos = 0;
        }
      }
      toRemove = false;
      if(iPhi > halfPhiWin + 1 && iPhi < m_nPhiTower - halfPhiWin - 1) {
        // finish processing that window, shift window to the next phi tower
        // substract first phi tower in current window
        sumWindow -= sumOverEta[iPhi-halfPhiWin];
        // add next phi tower to the window
        sumWindow += sumOverEta[iPhi+halfPhiWin+1];
      }  else if (iPhi < halfPhiWin) {
        // for windows at the lower edge of phi
        // substract first phi tower in current window
        sumWindow -= sumOverEta[iPhi-halfPhiWin+m_nPhiTower];
        // add next phi tower to the window
        sumWindow += sumOverEta[iPhi+halfPhiWin+1];
      } else {
        // for windows at the upper edge of phi
        // substract first phi tower in current window
        sumWindow -= sumOverEta[iPhi-halfPhiWin];
        // add next phi tower to the window
        sumWindow += sumOverEta[iPhi+halfPhiWin+1-m_nPhiTower];
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
  debug()<<"PRECLUSTERS size: "<<m_preClusters.size()<<endmsg;

  // 4. Sort the preclusters according to the energy
  std::sort(m_preClusters.begin(),m_preClusters.end(),compareCluster());
  debug()<<"PRECLUSTERS size after sort: "<<m_preClusters.size()<<endmsg;

  // 5. Remove duplicates
  for(auto it1 = m_preClusters.begin(); it1!=m_preClusters.end(); it1++) {
    debug()<<(*it1).eta << " "<<(*it1).phi<<" "<< (*it1).transEnergy <<endmsg;
    for(auto it2 = it1+1; it2!=m_preClusters.end();) {
      if ( (TMath::Abs((*it1).eta-(*it2).eta)<(m_nEtaDuplicates*m_deltaEtaTower)) && (TMath::Abs((*it1).phi-(*it2).phi)<(m_nPhiDuplicates*m_deltaPhiTower)) ) {
        it2 = m_preClusters.erase(it2);
      }
      else {
        //debug()<< (*it2).eta <<" "<<m_preClusters.size()<<endmsg;
        it2++;
      }
    }
  }
  debug()<<"PRECLUSTERS size after duplicates removal: "<<m_preClusters.size()<<endmsg;

  //TODO: If m_nEtaDuplicates < m_nEtaWindow, we have to divide energy between two clusters to avoid double counting.

  // 6. Create final clusters
  auto tubeSizes = det::utils::tubeDimensions(m_segmentation->volumeID(m_cells.get()->at(0).cellId()));
  double rDetector = tubeSizes.x();
  double theta = 0.;
  // TODO attach cells to built clusters
  auto edmClusters = m_clusters.createAndPut();
  for(const auto clu: m_preClusters) {
    auto edmCluster = edmClusters->create();
    auto& edmClusterCore = edmCluster.core();
    theta = 2.*atan(exp(-1.*clu.eta));
    edmClusterCore.position.x = rDetector * sin(theta) * cos(clu.phi);
    edmClusterCore.position.x = rDetector * sin(theta) * sin(clu.phi);
    edmClusterCore.position.x = rDetector * cos(theta);
    edmClusterCore.energy = clu.transEnergy; // saving transverse energy
  }

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
  //debug()<<"nEta "<< m_nEtaTower <<" nPhi " << m_nPhiTower <<endmsg;
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
    //save ET
    m_towers[ibin.first][ibin.second] += ecell.core().energy/TMath::CosH(m_segmentation->eta(ecell.core().cellId));
  }
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
