#include "CreateCaloClustersSlidingWindow.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"

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
  declareProperty("energyThreshold", m_energyThreshold = 3);
  declareProperty("energyPosThreshold", m_energyPosThreshold = 0.00001);
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
      sumWindow += sumOverEta[iPhiWindow];
    }
    // ... and add first bins
    for(int iPhiWindow = 0; iPhiWindow <= halfPhiWin; iPhiWindow++) {
      sumWindow += sumOverEta[iPhiWindow];
    }
    // loop over all the phi slices
    for(int iPhi = 0; iPhi < m_nPhiTower; iPhi++) {
      if(sumWindow > m_energyThreshold) {
        // test local maximum in phi
        if(m_checkPhiLocalMax) {
          // check closest neighbour on the right
          if(iPhi > halfPhiWin - 1 && iPhi < m_nPhiTower - halfPhiWin - 1) {
            if(sumOverEta[iPhi-halfPhiWin] < sumOverEta[iPhi+halfPhiWin+1]) {
              toRemove = true;
            }
          } else if(iPhi < halfPhiWin ) {
            if(sumOverEta[iPhi-halfPhiWin+m_nPhiTower] < sumOverEta[iPhi+halfPhiWin+1]) {
              toRemove = true;
            }
          } else {
            if(sumOverEta[iPhi-halfPhiWin] < sumOverEta[iPhi+halfPhiWin+1-m_nPhiTower]) {
              toRemove = true;
            }
          }
          // check closest neighbour on the left
          if(iPhi > halfPhiWin && iPhi < m_nPhiTower - halfPhiWin) {
            if(sumOverEta[iPhi+halfPhiWin] < sumOverEta[iPhi-halfPhiWin-1]) {
              toRemove = true;
            }
          } else if(iPhi < halfPhiWin + 1 )  {
            if(sumOverEta[iPhi+halfPhiWin] < sumOverEta[iPhi-halfPhiWin-1+m_nPhiTower]) {
              toRemove = true;
            }
          } else {
            if(sumOverEta[iPhi+halfPhiWin-m_nPhiTower] < sumOverEta[iPhi-halfPhiWin-1]) {
              toRemove = true;
            }
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
            if(iPhi >= halfPhiWin && iPhi < m_nPhiTower - halfPhiWin - 1) {
              // most common case
              for(int ipPhi = iPhi-halfPhiPos; ipPhi <= iPhi+halfPhiPos; ipPhi++) {
                posEta += eta(ipEta) * m_towers[ipEta][ipPhi];
                posPhi += phi(ipPhi) * m_towers[ipEta][ipPhi];
                sumEnergyPos += m_towers[ipEta][ipPhi];
              }
            }  else if (iPhi < halfPhiWin) {
              // corner case: centre in phi is close to lower edge
              for(int ipPhi = iPhi-halfPhiPos+m_nPhiTower; ipPhi < m_nPhiTower; ipPhi++) {
                posEta += eta(ipEta) * m_towers[ipEta][ipPhi];
                posPhi += phi(ipPhi) * m_towers[ipEta][ipPhi];
                sumEnergyPos += m_towers[ipEta][ipPhi];
              }
              for(int ipPhi = 0; ipPhi <= iPhi+halfPhiPos; ipPhi++) {
                posEta += eta(ipEta) * m_towers[ipEta][ipPhi];
                posPhi += phi(ipPhi) * m_towers[ipEta][ipPhi];
                sumEnergyPos += m_towers[ipEta][ipPhi];
              }
            } else {
              // corner case: centre in phi is close to upper edge
              for(int ipPhi = iPhi-halfPhiPos; ipPhi < m_nPhiTower; ipPhi++) {
                posEta += eta(ipEta) * m_towers[ipEta][ipPhi];
                posPhi += phi(ipPhi) * m_towers[ipEta][ipPhi];
                sumEnergyPos += m_towers[ipEta][ipPhi];
              }
              for(int ipPhi = 0; ipPhi <= iPhi+halfPhiPos-m_nPhiTower; ipPhi++) {
                posEta += eta(ipEta) * m_towers[ipEta][ipPhi];
                posPhi += phi(ipPhi) * m_towers[ipEta][ipPhi];
                sumEnergyPos += m_towers[ipEta][ipPhi];
              }
            }
          }
          // If non-zero energy in the cluster, add to pre-clusters (reduced size for pos. calculation -> energy in the core can be zero)
          if (fabs(sumEnergyPos)>m_energyPosThreshold) {
            posEta /= sumEnergyPos;
            posPhi /= sumEnergyPos;
            cluster newPreCluster;
            newPreCluster.eta = posEta;
            newPreCluster.phi = posPhi;
            newPreCluster.transEnergy = sumWindow;
            debug()<<"PRECLUSTERS: "<<newPreCluster.eta <<" "<<newPreCluster.phi<<" "<<newPreCluster.transEnergy<<endmsg;
            m_preClusters.push_back(newPreCluster);
          }
          posEta = 0;
          posPhi = 0;
          sumEnergyPos = 0;
        }
      }
      toRemove = false;
      if(iPhi >= halfPhiWin && iPhi < m_nPhiTower - halfPhiWin - 1) {
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
      if ( (abs(idEta((*it1).eta)-idEta((*it2).eta)) < m_nEtaDuplicates)
        && ( (abs(idPhi((*it1).phi)-idPhi((*it2).phi)) < floor(m_nPhiDuplicates/2.))
          || (abs(idPhi((*it1).phi)-idPhi((*it2).phi)) > m_nPhiTower - floor(m_nPhiDuplicates/2.)) ) )  {
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
  auto edmClusters = m_clusters.createAndPut();
  const fcc::CaloHitCollection* cells = m_cells.get();
  float etaCell=0, phiCell=0;
  for(const auto clu: m_preClusters) {
    auto edmCluster = edmClusters->create();
    auto& edmClusterCore = edmCluster.core();
    edmClusterCore.position.x = rDetector * cos(clu.phi);
    edmClusterCore.position.y = rDetector * sin(clu.phi);
    edmClusterCore.position.z = rDetector * sinh(clu.eta);
    edmClusterCore.energy = clu.transEnergy * cosh(clu.eta);
    debug() << "Cluster x: " << edmClusterCore.position.x << " y " <<  edmClusterCore.position.y << " z "<<  edmClusterCore.position.z << " energy " << edmClusterCore.energy <<endmsg;
    // now go one by one through all cells and see to if they belong here
    // TODO if noise is added there will be lots of cells - make it faster?
    for (const auto& cell : *cells) {
      etaCell = m_segmentation->eta(cell.core().cellId);
      phiCell = m_segmentation->phi(cell.core().cellId);
      if( (abs(idEta(etaCell)-idEta(clu.eta)) <= halfEtaWin)
        && (abs(idPhi(phiCell)-idPhi(clu.phi)) <= halfPhiWin) ) {
        edmCluster.addhits(cell);
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClustersSlidingWindow::finalize() {
  return GaudiAlgorithm::finalize();
}

void CreateCaloClustersSlidingWindow::prepareTowers() {
   auto numOfCells = det::utils::numberOfCells(m_segmentation->volumeID(m_cells.get()->at(0).cellId()), *m_segmentation);
  m_nEtaTower = numOfCells[1];
  m_nPhiTower = numOfCells[0];
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

  int iPhi = 0, iEta = 0;
  float etaCell = 0;
  for (const auto& ecell : *cells) {
    // find to which tower the cell belongs (defined by index ieta and iphi)
    etaCell = m_segmentation->eta(ecell.core().cellId);
    iEta = idEta(etaCell);
    iPhi = idPhi(m_segmentation->phi(ecell.core().cellId));
    // debug()<<"\t-> eta = "<<etaCell<<" -> "<<iEta<<" -> "<<eta(iEta)<<"\t-> phi = "<<m_segmentation->phi(ecell.core().cellId)<<" -> "<<iPhi<<" -> "<<phi(iPhi)<<endmsg;
    // save transverse energy
    m_towers[iEta][iPhi] += ecell.core().energy/cosh(etaCell);
  }
  return;
}

int CreateCaloClustersSlidingWindow::idEta(float aEta) {
  // shift Ids so they start at 0 (segmentation returns IDs that may be from -N to N)
  // for segmentation in eta the middle cell has its centre at eta=0 (segmentation offset = 0)
  return floor((aEta+m_deltaEtaTower/2.)/m_deltaEtaTower) + (m_nEtaTower-1)/2;
}

int CreateCaloClustersSlidingWindow::idPhi(float aPhi) {
  // shift Ids so they start at 0 (segmentation returns IDs that may be from -N to N)
  // for segmentation in phi the middle cell has its centre at phi=0 (segmentation offset = 0)
  return floor((aPhi+m_deltaPhiTower/2.)/m_deltaPhiTower) + (m_nPhiTower-1)/2;
}

float CreateCaloClustersSlidingWindow::eta(int aIdEta) {
  // middle of the tower
  return (aIdEta - (m_nEtaTower-1)/2 ) * m_deltaEtaTower;
}

float CreateCaloClustersSlidingWindow::phi(int aIdPhi) {
  // middle of the tower
  return (aIdPhi - (m_nPhiTower-1)/2 ) * m_deltaPhiTower;
}
