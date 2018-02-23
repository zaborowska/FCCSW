#include "NoiseCorrectionToClusterTool.h"

#include "GaudiKernel/ITHistSvc.h"

// Root
#include "TFile.h"
#include "TVector3.h"

DECLARE_TOOL_FACTORY(NoiseCorrectionToClusterTool)

NoiseCorrectionToClusterTool::NoiseCorrectionToClusterTool(const std::string& type, const std::string& name,
                                                           const IInterface* parent)
: GaudiTool(type, name, parent),
      m_histSvc("THistSvc", "NoiseCorrectionToClusterTool"),
      m_hEnergyPrePileupCorrection(nullptr),
      m_hEnergyPostPileupCorrection(nullptr),
      m_hPileupEnergy(nullptr) {
  declareInterface<INoiseCorrectionTool>(this);
}

StatusCode NoiseCorrectionToClusterTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  // Initialize random service
  if (service("RndmGenSvc", m_randSvc).isFailure()) {
    error() << "Couldn't get RndmGenSvc!!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  m_gauss.initialize(m_randSvc, Rndm::Gauss(0., 1.));

  // open and check file, read the histograms with noise constants
  if (initNoiseFromFile().isFailure()) {
    error() << "Couldn't open file with noise constants!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  // create control histograms
  m_hPileupEnergy = new TH1F("pileupCorrectionEnergy", "Energy added to a cluster as a correction for correlated noise", 1000, -10, 10);
  if (m_histSvc->regHist("/rec/pileupCorrectionEnergy", m_hPileupEnergy).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hEnergyPrePileupCorrection = new TH1F("energyPrePileupCorrection", "Energy of cluster before the correction", 1000, 0.8 * m_energy, 1.2 * m_energy);
  if (m_histSvc->regHist("/rec/energyPrePileupCorrection", m_hEnergyPrePileupCorrection).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hEnergyPostPileupCorrection = new TH1F("energyPostPileupCorrection", "Energy of cluster after the correction", 1000, 0.8 * m_energy, 1.2 * m_energy);
  if (m_histSvc->regHist("/rec/energyPostPileupCorrection", m_hEnergyPostPileupCorrection).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode NoiseCorrectionToClusterTool::finalize() { return GaudiTool::finalize(); }

void NoiseCorrectionToClusterTool::correctEnergy(fcc::CaloCluster& aEdmCluster) {
  TVector3 position(aEdmCluster.core().position.x, aEdmCluster.core().position.y, aEdmCluster.core().position.z);
  double eta = position.Eta();
  uint numCells = aEdmCluster.hits_size();
  double noise = getNoiseConstantPerCluster(eta, numCells) * m_gauss.shoot();
  m_hEnergyPrePileupCorrection->Fill(aEdmCluster.core().energy);
  aEdmCluster.core().energy += noise;
  m_hEnergyPostPileupCorrection->Fill(aEdmCluster.core().energy);
  m_hPileupEnergy->Fill(noise);
}

StatusCode NoiseCorrectionToClusterTool::initNoiseFromFile() {
  // check if file exists
  if (m_noiseFileName.empty()) {
    error() << "Name of the file with noise values not set" << endmsg;
    return StatusCode::FAILURE;
  }
  TFile file(m_noiseFileName.value().c_str(), "READ");
  if (file.IsZombie()) {
    error() << "Couldn't open the file with noise constants" << endmsg;
    return StatusCode::FAILURE;
  } else {
    info() << "Opening the file with noise constants: " << m_noiseFileName << endmsg;
  }

  std::string pileupParamHistoName;
  // Read the histograms with parameters for the pileup noise from the file
  for (unsigned i = 0; i < 2; i++) {
    pileupParamHistoName = m_pileupHistoName + std::to_string(i);
    debug() << "Getting histogram with a name " << pileupParamHistoName << endmsg;
    m_histoPileupConst.push_back(*dynamic_cast<TH1F*>(file.Get(pileupParamHistoName.c_str())));
    if (m_histoPileupConst.at(i).GetNbinsX() < 1) {
      error() << "Histogram  " << pileupParamHistoName
              << " has 0 bins! check the file with noise and the name of the histogram!" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // Check if we have same number of histograms (all layers) for pileup and electronics noise
  if (m_histoPileupConst.size() == 0) {
    error() << "No histograms with noise found!!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

double NoiseCorrectionToClusterTool::getNoiseConstantPerCluster(double aEta, uint aNumCells) {
  double param0 = 0.;
  double param1 = 0.;

  // All histograms have same binning, all bins with same size
  // Using the histogram of the first parameter to get the bin size
  unsigned index = 0;
  if (m_histoPileupConst.size() != 0) {
    int Nbins = m_histoPileupConst.at(index).GetNbinsX();
    double deltaEtaBin =
      (m_histoPileupConst.at(index).GetBinLowEdge(Nbins) + m_histoPileupConst.at(index).GetBinWidth(Nbins) -
       m_histoPileupConst.at(index).GetBinLowEdge(1)) /
      Nbins;
    double etaFirtsBin = m_histoPileupConst.at(index).GetBinLowEdge(1);
    // find the eta bin for the cell
    int ibin = floor((fabs(aEta) - etaFirtsBin) / deltaEtaBin) + 1;
    if (ibin > Nbins) {
      debug() << "eta outside range of the histograms! Cell eta: " << aEta << " Nbins in histogram: " << Nbins
              << endmsg;
      ibin = Nbins;
    }
    param0 = m_histoPileupConst.at(0).GetBinContent(ibin);
    param1 = m_histoPileupConst.at(1).GetBinContent(ibin);
  } else {
    debug() << "No histograms with noise constants!!!!! " << endmsg;
  }
  double pileupNoise = param0 * pow(aNumCells, param1);

  return pileupNoise;
}
