#include "CorrectCluster.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "DetSegmentation/FCCSWGridPhiEta.h"

// DD4hep
#include "DD4hep/Detector.h"

// our EDM
#include "datamodel/CaloCluster.h"
#include "datamodel/CaloClusterCollection.h"

DECLARE_ALGORITHM_FACTORY(CorrectCluster)

CorrectCluster::CorrectCluster(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc),
      m_histSvc("THistSvc", "CorrectCluster"),
      m_geoSvc("GeoSvc", "CorrectCluster"),
      m_hEnergyPreAnyCorrections(nullptr),
      m_hEnergyPostAllCorrections(nullptr)  {
  declareProperty("clusters", m_inClusters, "Input clusters (input)");
  declareProperty("correctedClusters", m_correctedClusters, "Corrected clusters (output)");

  declareProperty("noiseTool", m_noiseTool, "Handle for the noise correction tool");
}

StatusCode CorrectCluster::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  info() << "do correction for correlated pile-up noise : " << m_doNoiseCorrection << endmsg;
  if (m_etaRecalcLayerWeights.size() == 0) m_doEtaRecalc = false;
  info() << "do eta recalculation using log-weighting : " << m_doEtaRecalc << endmsg;
  if (m_upstreamMaterialParamsPath.empty()) m_doUpstreamCorrection = false;
  info() << "do correction for upstream material : " << m_doUpstreamCorrection << endmsg;

  // Initialization of tools
  if (m_doNoiseCorrection) {
    if (!m_noiseTool.retrieve()) {
      error() << "Unable to retrieve the noise correction tool!!!" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  if (m_systemId.size() != m_readoutName.size()) {
    error() << "systemId and readoutName have different size." << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_systemId.size() != m_numLayers.size()) {
    error() << "systemId and numLayers have different size." << endmsg;
    return StatusCode::FAILURE;
  }
  m_sumLayers = std::accumulate(m_numLayers.begin(), m_numLayers.end(), 0);
  if (m_etaRecalcLayerWeights.size() != m_sumLayers) {
    error() << "m_etaRecalcLayerWeights size and sum of numLayers are different." << endmsg;
    return StatusCode::FAILURE;
  }
  for(uint iSys = 0; iSys < m_systemId.size(); iSys++) {
    // check if readouts exist
    if (m_geoSvc->lcdd()->readouts().find(m_readoutName[iSys]) == m_geoSvc->lcdd()->readouts().end()) {
      error() << "Readout <<" << m_readoutName[iSys] << ">> does not exist." << endmsg;
      return StatusCode::FAILURE;
    }
    // retrieve PhiEta segmentation
    m_segmentation[m_systemId[iSys]] = dynamic_cast<dd4hep::DDSegmentation::FCCSWGridPhiEta*>(
      m_geoSvc->lcdd()->readout(m_readoutName[iSys]).segmentation().segmentation());
    if (m_segmentation[m_systemId[iSys]] == nullptr) {
      error() << "There is no phi-eta segmentation." << endmsg;
      return StatusCode::FAILURE;
    }
    m_decoder[m_systemId[iSys]] = m_geoSvc->lcdd()->readout(m_readoutName[iSys]).idSpec().decoder();
  }
  // create control histograms
  m_hEnergyPreAnyCorrections = new TH1F("energyPreAnyCorrections", "Energy of cluster before any correction", 1000, 0.8 * m_energy, 1.2 * m_energy);
  if (m_histSvc->regHist("/rec/energyPreAnyCorrections", m_hEnergyPreAnyCorrections).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hEnergyPostAllCorrections = new TH1F("energyPostAllCorrections", "Energy of cluster after all corrections", 1000, 0.8 * m_energy, 1.2 * m_energy);
  if (m_histSvc->regHist("/rec/energyPostAllCorrections", m_hEnergyPostAllCorrections).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hUpstreamEnergy = new TH1F("upstreamCorrectionEnergy", "Energy added to a cluster as a correction for upstream material", 1000, -10, 10);
  if (m_histSvc->regHist("/rec/upstreamCorrectionEnergy", m_hUpstreamEnergy).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
   // calculate borders of eta bins:
   if (m_etaValues.size() != m_presamplerShiftP0.size() &&
       m_etaValues.size() != m_presamplerShiftP1.size() &&
       m_etaValues.size() != m_presamplerScaleP0.size() &&
       m_etaValues.size() != m_presamplerScaleP1.size() ) {
      error() << "Sizes of parameter vectors should be the same" << endmsg;
      return StatusCode::FAILURE;
   }
   // if only one eta, leave border vector empty
   for(uint iEta = 1; iEta < m_etaValues.size(); iEta++) {
      m_etaBorders.push_back(m_etaValues[iEta-1] + 0.5 * (m_etaValues[iEta] - m_etaValues[iEta-1]) );
   }
   // push values for the last eta bin, width as the last one
   m_etaBorders.push_back(m_etaValues[m_etaValues.size() - 1] + 0.5 * (m_etaValues[m_etaValues.size() - 1] - m_etaValues[m_etaValues.size() - 2]) );
   for(auto eta: m_etaBorders)
      warning() << eta << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode CorrectCluster::execute() {
  // Get the input collection with clusters
  const fcc::CaloClusterCollection* inClusters = m_inClusters.get();
  debug() << "Input cluster collection size: " << inClusters->size() << endmsg;
  fcc::CaloClusterCollection* correctedClusters = m_correctedClusters.createAndPut();

  for (const auto& cluster: *inClusters) {
    // 0. Create new cluster, copy information from input
    fcc::CaloCluster newCluster = correctedClusters->create();
    newCluster.core().energy = cluster.core().energy;
    newCluster.core().position.x = cluster.core().position.x;
    newCluster.core().position.y = cluster.core().position.y;
    newCluster.core().position.z = cluster.core().position.z;
    for(auto cell = cluster.hits_begin(); cell != cluster.hits_end(); cell++) {
      newCluster.addhits(*cell);
    }

    // 1. Correct energy for the correlated noise
    if (m_doNoiseCorrection) {
      m_noiseTool->correctEnergy(newCluster);
    }

    double sumEnFirstLayer = 0;

    // 2. Correct eta position with log-weighting
    if (m_doEtaRecalc) { // based on information from ECal
      uint systemId = m_systemId[0]; // TODO fix that assumption, take sys Id from cells, but this requires new weights for other systems
      // get current pseudorapidity
      TVector3 pos (cluster.core().position.x, cluster.core().position.y, cluster.core().position.z);
      double oldEta = pos.Eta();
      double newEta = 0;
      std::vector<double> sumEnLayer;
      std::vector<double> sumEtaLayer;
      std::vector<double> sumWeightLayer;
      sumEnLayer.assign(m_sumLayers, 0);
      sumEtaLayer.assign(m_sumLayers, 0);
      sumWeightLayer.assign(m_sumLayers, 0);
      // first check the energy deposited in each layer
      for(auto cell = cluster.hits_begin(); cell != cluster.hits_end(); cell++) {
        m_decoder[systemId]->setValue(cell->core().cellId);
        uint layer = (*m_decoder[systemId])[m_layerFieldName] + m_firstLayerId; // TODO fix for hcals: take into account offset from ecal layers
        sumEnLayer[layer] += cell->core().energy;
      }
      sumEnFirstLayer = sumEnLayer[0];
      // repeat but calculating eta barycentre in each layer
      for(auto cell = cluster.hits_begin(); cell != cluster.hits_end(); cell++) {
        m_decoder[systemId]->setValue(cell->core().cellId);
        uint layer = (*m_decoder[systemId])[m_layerFieldName] + m_firstLayerId; // TODO fix for hcals: take into account offset from ecal layers
        double weightLog = std::max(0., m_etaRecalcLayerWeights[layer] + log(cell->core().energy / sumEnLayer[layer]));
        double eta = m_segmentation[systemId]->eta(cell->core().cellId);
        sumEtaLayer[layer] += (weightLog * eta);
        sumWeightLayer[layer] += weightLog;
      }
      // calculate eta position weighting with energy deposited in layer
      // this energy is a good estimator of 1/sigma^2 of (eta_barycentre-eta_MC) distribution
      for (uint iLayer = 0; iLayer < m_sumLayers; iLayer++) {
        if(sumWeightLayer[iLayer] != 0) {
          sumEtaLayer[iLayer] /= sumWeightLayer[iLayer];
        }
        newEta += sumEtaLayer[iLayer] * sumEnLayer[iLayer];
      }
      newEta /= std::accumulate(sumEnLayer.begin(), sumEnLayer.end(), 0);
      // alter Cartesian position of a cluster using new eta position
      double radius = pos.Perp();
      double phi = pos.Phi();
      newCluster.core().position.x = radius * cos(phi);
      newCluster.core().position.y = radius * sin(phi);
      newCluster.core().position.z = radius * sinh(newEta);
    }

    // 3. Correct energy for the energy deposited in the upstream material
    if (m_doUpstreamCorrection) {
      TVector3 pos (newCluster.core().position.x, newCluster.core().position.y, newCluster.core().position.z);
      double eta = pos.Eta();
      // correct for presampler based on energy in the first layer layer:
      // check eta of the cluster and get correction parameters:
      double P00=0,P01=0,P10=0,P11=0;
      for(uint iEta = 0; iEta < m_etaBorders.size(); iEta++) {
        if(fabs( eta ) < m_etaBorders[iEta]) {
          P00 = m_presamplerShiftP0[iEta];
          P01 = m_presamplerShiftP1[iEta];
          P10 = m_presamplerScaleP0[iEta];
          P11 = m_presamplerScaleP1[iEta];
          break;
        }
      }
      // if eta is larger than the last available eta values, take the last known parameters
      if(fabs( eta) > m_etaBorders.back()) {
        warning() << "cluster eta = " << eta << " is larger than last defined eta values." << endmsg;
      }
      if (sumEnFirstLayer == 0) {
        uint systemId = m_systemId[0]; // TODO fix that assumption, take sys Id from cells, but this requires new weights for other systems
        for(auto cell = cluster.hits_begin(); cell != cluster.hits_end(); cell++) {
          m_decoder[systemId]->setValue(cell->core().cellId);
          uint layer = (*m_decoder[systemId])[m_layerFieldName] + m_firstLayerId;
          if(layer == m_firstLayerId) {
            sumEnFirstLayer += cell->core().energy;
          }
        }
      }
      double presamplerShift = P00 + P01 * cluster.core().energy;
      double presamplerScale = P10 + P11 / sqrt( cluster.core().energy );
      double energyFront = presamplerShift + presamplerScale * sumEnFirstLayer ;
      debug() << "UPSTREAM corr: " << presamplerShift << "\t" << presamplerScale << "\t" << energyFront << endmsg;
      m_hUpstreamEnergy->Fill(energyFront);
      newCluster.core().energy += energyFront;
    }

    // Fill histograms
    m_hEnergyPreAnyCorrections->Fill(cluster.core().energy);
    m_hEnergyPostAllCorrections->Fill(newCluster.core().energy);
  }

  return StatusCode::SUCCESS;
}

StatusCode CorrectCluster::finalize() { return GaudiAlgorithm::finalize(); }
