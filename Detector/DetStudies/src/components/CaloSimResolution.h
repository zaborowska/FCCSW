#ifndef DETSTUDIES_CALOSIMRESOLUTION_H
#define DETSTUDIES_CALOSIMRESOLUTION_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "DetSegmentation/GridPhiEta.h"
class IGeoSvc;

// datamodel
#include "datamodel/CaloHitCollection.h"
#include "datamodel/MCParticleCollection.h"


class TH1F;
class ITHistSvc;

/** @class CaloSimResolution CaloSimResolution.h
 *
 *  @author Anna Zaborowska
 */

class CaloSimResolution: public GaudiAlgorithm {
public:
  explicit CaloSimResolution(const std::string&, ISvcLocator*);
  virtual ~CaloSimResolution();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Fills the histograms.
   *   @return status code
   */
  virtual StatusCode execute() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;
  private:
  Gaudi::Property<double> m_energy{this, "energyAxis", 500, "Maximum energy for axis range"};
  /// Pointer to the interface of histogram service
  SmartIF<ITHistSvc> m_histSvc;
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  TH1F* m_energyRaw;
  TH1F* m_energyCorrected;
  TH1F* m_diffEta;
  TH1F* m_diffPhi;
  std::vector<TH1F*> m_enLayers;
  std::vector<TH1F*> m_diffEtaLayers;
  std::vector<TH1F*> m_diffPhiLayers;
  /// Name of the layer/cell field
  Gaudi::Property<std::string> m_layerFieldName{this, "layerFieldName", "", "Identifier of layers"};
  /// Number of layers/cells
  Gaudi::Property<uint> m_numLayers{this, "numLayers", 8, "Number of layers"};
  /// Id of the first layer
  Gaudi::Property<uint> m_firstLayerId{this, "firstLayerId", 0, "ID of first layer"};
  /// Name of the detector readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "", "Name of the detector readout"};
  Gaudi::Property<double> m_numPhi{this, "numPhi", 704, "Phi number"};
  Gaudi::Property<double> m_numEta{this, "dEta", 0.01, "dEta"};
  DataHandle<fcc::CaloHitCollection> m_cells{"rec/caloCells", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::MCParticleCollection> m_particle{"gen/particles", Gaudi::DataHandle::Reader, this};
  ///
  Gaudi::Property<std::vector<double>> m_etaValues{this, "etaValues", {}, "Eta values"};
  std::vector<double> m_etaBorders;
  Gaudi::Property<std::vector<double>> m_presamplerShiftP0{this, "presamplerShiftP0", {}, "Upstream material param 00 as fnc of eta"};
  ///
  Gaudi::Property<std::vector<double>> m_presamplerScaleP0{this, "presamplerScaleP0", {}, "Upstream material param 10 as fnc of eta"};
  ///
  Gaudi::Property<std::vector<double>> m_presamplerShiftP1{this, "presamplerShiftP1", {}, "Upstream material param 01 as fnc of eta"};
  ///
Gaudi::Property<std::vector<double>> m_presamplerScaleP1{this, "presamplerScaleP1", {}, "Upstream material param 11 as fnc of eta"};
  double m_phiMax = M_PI;
  double m_etaMax = 1.6805;
  double m_dPhi = 2*M_PI/704;
  double m_dEta = 0.01;
  DD4hep::DDSegmentation::GridPhiEta* m_segmentation;
};
#endif /* DETSTUDIES_CALOSIMRESOLUTION_H */
