#ifndef RECCALORIMETER_NOISECORRECTIONTOCLUSTERTOOL_H
#define RECCALORIMETER_NOISECORRECTIONTOCLUSTERTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
class IRndmGenSvc;
class ITHistSvc;

// FCCSW
#include "RecInterface/INoiseCorrectionTool.h"

// ROOT
#include "TH1F.h"

/** @class NoiseCorrectionToClusterTool
 *
 *  Apply correction to the energy of a cluster taking into account the correlated pile-up noise.
 *  Pile-up noise per cluster can be parametrised as A*SIZE^B, where A,B - eta-dependent parameters,
 *  and SIZE is the number of cells contributing to a cluster.
 *  Histograms of parameters A and B, eta-dependent, are read from a ROOT file.
 *  Names of those histograms have common prefix (m_pileupHistoName) and appended 0,1 for A, B, respectively.
 *
 *  @author Anna Zaborowska
 *
 */

class NoiseCorrectionToClusterTool : public GaudiTool, virtual public INoiseCorrectionTool {
public:
  NoiseCorrectionToClusterTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~NoiseCorrectionToClusterTool() = default;
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  /**  Apply correction to the energy of a cluster acknowledging the correlation of pile-up noise.
   *   @param[out] aEdmCluster Cluster
   */
  virtual void correctEnergy(fcc::CaloCluster& aEdmCluster) final;
  /** Open file and read noise histograms in the memory
   *  @return Status code if retriving histograms was successful
   */
  StatusCode initNoiseFromFile();
  /** Find the appropriate noise constant from the histogram
   *  @param[in] aEta Pseudorapidity value of the centre of the cluster
   *  @param[in] aNumCells Number of cells in a cluster
   *  @return Width of the Gaussian distribution of noise per cluster
   */
  double getNoiseConstantPerCluster(double aEta, uint numCells);

private:
  /// Name of the file with noise constants
  Gaudi::Property<std::string> m_noiseFileName{this, "noiseFileName", "TestPileup_Cluster_mu200_100files.root", "Name of the file with noise constants"};
  /// Name of pileup histogram
  Gaudi::Property<std::string> m_pileupHistoName{this, "pileupHistoName", "histFitToClusterDependence_Measured_p", "Name of pileup histogram"};
  /// Histograms with pileup constants (index in array - radial layer)
  std::vector<TH1F> m_histoPileupConst;
  /// Random Number Service
  IRndmGenSvc* m_randSvc;
  /// Gaussian random number generator used for the generation of random noise hits
  Rndm::Numbers m_gauss;
  /// Pointer to the interface of histogram service
  ServiceHandle<ITHistSvc> m_histSvc;
  /// Histogram of energy before pileup correction
  TH1F* m_hEnergyPrePileupCorrection;
  /// Histogram of energy after pileup correction
  TH1F* m_hEnergyPostPileupCorrection;
  /// Histogram of pileup noise added to energy of clusters
  TH1F* m_hPileupEnergy;
  /// Energy of the centre of energy distribution histograms
  Gaudi::Property<double> m_energy{this, "energyAxis", 50, "Energy of the centre of energy distribution histograms"};
};

#endif /* RECCALORIMETER_NOISECORRECTIONTOCLUSTERTOOL_H */
