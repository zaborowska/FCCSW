#include "CaloRecResolution.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

#include "TH1F.h"
#include "TVector3.h"
#include "GaudiKernel/ITHistSvc.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Readout.h"

DECLARE_ALGORITHM_FACTORY(CaloRecResolution)

CaloRecResolution::CaloRecResolution(const std::string& aName, ISvcLocator* aSvcLoc):
GaudiAlgorithm(aName, aSvcLoc), m_energyRaw(nullptr), m_energyCorrected(nullptr), m_diffEta(nullptr), m_diffPhi(nullptr), m_diffTheta(nullptr)
{
   declareProperty("cluster", m_cluster, "Reconstructed cluster in calorimeter (input)");
   declareProperty("cells", m_cells, "Cells in calorimeter (input)");
   declareProperty("particle", m_particle, "Generated single-particle event (input)");
}
CaloRecResolution::~CaloRecResolution() {}

StatusCode CaloRecResolution::initialize() {
   if (GaudiAlgorithm::initialize().isFailure())
      return StatusCode::FAILURE;
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
   m_histSvc = service("THistSvc");
   if (! m_histSvc) {
      error() << "Unable to locate Histogram Service" << endmsg;
      return StatusCode::FAILURE;
   }
   m_energyRaw = new TH1F("energy", "Energy; energy (GeV)", 1000, 0.5 * m_energy, 1.5 * m_energy);
   if (m_histSvc->regHist("/det/energy", m_energyRaw).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
   }
   m_energyCorrected = new TH1F("energyCorrected", "Energy corrected for material in front; energy (GeV)", 1000, 0.5 * m_energy, 1.5 * m_energy);
   if (m_histSvc->regHist("/det/energyCorrected", m_energyCorrected).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
   }
   m_diffEta = new TH1F("diffEta", "diff eta; #Delta#eta", 10 * ceil(2 * m_etaMax / m_dEta), - m_etaMax / 10., m_etaMax / 10.);
   if (m_histSvc->regHist("/det/diffEta", m_diffEta).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
   }
   m_diffTheta = new TH1F("diffTheta", "diff theta; #Delta#theta", 10 * ceil(2 * m_etaMax / m_dEta), - 0.25, 0.25);
   if (m_histSvc->regHist("/det/diffTheta", m_diffTheta).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
   }
   m_diffThetaCalc = new TH1F("diffThetaCalc", "diff theta calculated from 2 layers; #Delta#theta", 10 * ceil(2 * m_etaMax / m_dEta), - 0.25, 0.25);
   if (m_histSvc->regHist("/det/diffThetaCalc", m_diffThetaCalc).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
   }
   m_diffPhi = new TH1F("diffPhi", "diff phi; #Delta#varphi;", 10 * ceil(2 * m_phiMax / m_dPhi), - m_phiMax / 10., m_phiMax / 10.);
   if (m_histSvc->regHist("/det/diffPhi", m_diffPhi).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
   }
   for (uint i = 0; i < m_numLayers; i++) {
      m_enLayers.push_back(new TH1F(("enLayer" + std::to_string(i)).c_str(),
                                    ("Energy deposited in layer " + std::to_string(i) + ";energy (GeV)").c_str(), 1000, 0,
                                    1.2 * m_energy));
      if (m_histSvc->regHist("/det/enLayer" + std::to_string(i), m_enLayers.back()).isFailure()) {
         error() << "Couldn't register histogram" << endmsg;
         return StatusCode::FAILURE;
      }
      m_diffEtaLayers.push_back(new TH1F(("diffEtaLayer" + std::to_string(i)).c_str(),
                                         ("Diff eta of layer " + std::to_string(i) + "#Delta#eta").c_str(), 10 * ceil(2 * m_etaMax / m_dEta), - m_etaMax / 10., m_etaMax / 10.));
      if (m_histSvc->regHist("/det/diffEtaLayer" + std::to_string(i), m_diffEtaLayers.back()).isFailure()) {
         error() << "Couldn't register histogram" << endmsg;
         return StatusCode::FAILURE;
      }
      m_diffThetaLayers.push_back(new TH1F(("diffThetaLayer" + std::to_string(i)).c_str(),
                                           ("Diff theta of layer " + std::to_string(i) + "#Delta#theta").c_str(), 10 * ceil(2 * m_etaMax / m_dEta), - 0.25, 0.25));
      if (m_histSvc->regHist("/det/diffThetaLayer" + std::to_string(i), m_diffThetaLayers.back()).isFailure()) {
         error() << "Couldn't register histogram" << endmsg;
         return StatusCode::FAILURE;
      }
      m_diffPhiLayers.push_back(new TH1F(("diffPhiLayer" + std::to_string(i)).c_str(),
                                         ("Diff phi of layer " + std::to_string(i) + "#Delta#varphi").c_str(), 10 * ceil(2 * m_phiMax / m_dPhi), - m_phiMax / 10., m_phiMax / 10.));
      if (m_histSvc->regHist("/det/diffPhiLayer" + std::to_string(i), m_diffPhiLayers.back()).isFailure()) {
         error() << "Couldn't register histogram" << endmsg;
         return StatusCode::FAILURE;
      }
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

StatusCode CaloRecResolution::execute() {
   TVector3 momentum;
   double phiVertex = 0;
   double etaVertex = 0;
   double thetaVertex = 0;
   const auto particle = m_particle.get();
   if (particle->size() > 1) {
      error() << "This is not a single particle event! Number of particles: " << particle->size() << endmsg;
      return StatusCode::FAILURE;
   }
   for(const auto& part : *particle) {
      momentum = TVector3(part.core().p4.px, part.core().p4.py, part.core().p4.pz);
      etaVertex = momentum.Eta();
      phiVertex = momentum.Phi();
      thetaVertex = 2 * atan( exp( - etaVertex ) );
   }

   double enCluMax = 0;
   double etaCluMax = 0;
   double thetaCluMax = 0;
   double phiCluMax = 0;
   const auto cluster = m_cluster.get();
   for(const auto& clu : *cluster) {
      TVector3 pos (clu.core().position.x, clu.core().position.y, clu.core().position.z);
      if(enCluMax < clu.core().energy) {
        enCluMax = clu.core().energy;
        phiCluMax = pos.Phi();
        etaCluMax = pos.Eta();
        thetaCluMax = 2 * atan( exp( - etaCluMax ) );
      }
   }
   debug() << "eta particle: \t" << etaVertex << "\t eta cluster: \t" << etaCluMax << endmsg;
   double phiEntrance = phiVertex - 0.5 * asin( 0.3 * 4. * 1.92 / enCluMax );

   auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
   double sumEn = 0.;
   double sumEta = 0.;
   double sumPhi = 0.;
   std::vector<double> sumEnLayer;
   std::vector<double> sumPhiLayer;
   std::vector<double> sumEtaLayer;
   std::vector<double> sumThetaLayer;
   sumEnLayer.assign(m_numLayers, 0);
   sumPhiLayer.assign(m_numLayers, 0);
   sumEtaLayer.assign(m_numLayers, 0);
   sumThetaLayer.assign(m_numLayers, 0);
   double minEta = etaCluMax - m_dEta * m_nEtaWindow;
   double maxEta = etaCluMax + m_dEta * m_nEtaWindow;
   double minPhi = phiCluMax - m_dPhi * m_nPhiWindow;
   double maxPhi = phiCluMax + m_dPhi * m_nPhiWindow;

   const auto cells = m_cells.get();
   for(const auto& cell : *cells) {
      double phi = m_segmentation->phi(cell.core().cellId);
      double eta = m_segmentation->eta(cell.core().cellId);
      if(phi > minPhi && phi < maxPhi && eta > minEta && eta < maxEta) {
         decoder->setValue(cell.core().cellId);
         sumEn += cell.core().energy;
         sumEnLayer[(*decoder)[m_layerFieldName]] += cell.core().energy;
         sumEta += (cell.core().energy * eta);
         sumPhi += (cell.core().energy * phi);
         sumEtaLayer[(*decoder)[m_layerFieldName]] += (cell.core().energy * eta);
         sumThetaLayer[(*decoder)[m_layerFieldName]] += ( cell.core().energy * 2. * atan( exp( - eta) ) );
         sumPhiLayer[(*decoder)[m_layerFieldName]] += (cell.core().energy * phi);
      }
   }
   // normalize
   sumEta /= sumEn;
   sumPhi /= sumEn;
   for (uint iLayer = 0; iLayer < m_numLayers; iLayer++) {
      sumEtaLayer[iLayer] /= sumEnLayer[iLayer];
      sumPhiLayer[iLayer] /= sumEnLayer[iLayer];
   }
   // correct for presampler based on energy in layer 0:
   // check eta of the deposits (avg) and get correct parameters:
   double P00=0,P01=0,P10=0,P11=0;
   for(uint iEta = 0; iEta < m_etaBorders.size(); iEta++) {
      if(fabs( sumEtaLayer[0] ) < m_etaBorders[iEta]) {
         P00 = m_presamplerShiftP0[iEta];
         P01 = m_presamplerShiftP1[iEta];
         P10 = m_presamplerScaleP0[iEta];
         P11 = m_presamplerScaleP1[iEta];
         break;
      }
   }
   // if eta is larger than the last available eta values, take the last known parameters
   if(fabs( sumEtaLayer[0] ) > m_etaBorders.back()) {
      P00 = m_presamplerShiftP0[ m_presamplerShiftP0.size() - 1 ];
      P01 = m_presamplerShiftP1[ m_presamplerShiftP1.size() - 1 ];
      P10 = m_presamplerScaleP0[ m_presamplerScaleP0.size() - 1 ];
      P11 = m_presamplerScaleP1[ m_presamplerScaleP1.size() - 1 ];
      warning() << "average cell eta = " << sumEtaLayer[0] << " is larger than last defined eta values." << endmsg;
   }
   double presamplerShift = P00 + P01 * sumEn;
   double presamplerScale = P10 + P11 / sqrt( sumEn );
   double energyFront = presamplerShift + presamplerScale * sumEnLayer[0] * 0.12125;
   m_energyRaw->Fill(enCluMax);
   m_energyCorrected->Fill(sumEn + energyFront);
   m_diffEta->Fill(etaVertex - etaCluMax);
   m_diffPhi->Fill(phiEntrance - phiCluMax);
   std::vector<double> layerRadius = {1.92 + 0.0269 / 2., 1.9469 + 0.5 * 0.09, 1.9469 + 1.5 * 0.09, 1.9469 + 2.5 * 0.09, 1.9469 + 3.5 * 0.09, 1.9469 + 4.5 * 0.09, 1.9469 + 5.5 * 0.09, 1.9469 + 6.5 * 0.09};
   for(uint iLayer = 0; iLayer < m_numLayers; iLayer++) {
      m_enLayers[iLayer]->Fill(sumEnLayer[iLayer]);
      double phiTrueLayer = phiVertex - 0.5 * asin( 0.3 * 4. * layerRadius[iLayer] / (sumEn + energyFront) );
      m_diffPhiLayers[iLayer]->Fill(phiTrueLayer - sumPhiLayer[iLayer]);
      m_diffEtaLayers[iLayer]->Fill(etaVertex - sumEtaLayer[iLayer ]);
      m_diffThetaLayers[iLayer]->Fill(thetaVertex - sumThetaLayer[iLayer ]);
   }
   double z1 = layerRadius[0] / tan(sumThetaLayer[0]);
   double z2 = layerRadius[1] / tan(sumThetaLayer[1]);
   m_diffThetaCalc->Fill( atan( (z2 * tan(sumThetaLayer[1]) - z1 * tan(sumThetaLayer[0])) / (z2 - z1) )  );
   return StatusCode::SUCCESS;
}

StatusCode CaloRecResolution::finalize() {
   return GaudiAlgorithm::finalize();
}
