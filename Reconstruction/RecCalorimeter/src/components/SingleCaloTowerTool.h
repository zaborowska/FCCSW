#ifndef RECCALORIMETER_SINGLECALOTOWERTOOL_H
#define RECCALORIMETER_SINGLECALOTOWERTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ITowerTool.h"
#include "DetSegmentation/GridPhiEta.h"
class IGeoSvc;

// datamodel
#include "datamodel/CaloHit.h"
namespace fcc {
class CaloHitCollection;
}

#include "DD4hep/Readout.h"
namespace DD4hep {
namespace DDSegmentation {
class Segmentation;
}
}

/** @class SingleCaloTowerTool Reconstruction/RecCalorimeter/src/components/SingleCaloTowerTool.h SingleCaloTowerTool.h
 *
 *  Tool building the calorimeter towers for the sliding window algorithm.
 *  Towers are built of cells in eta-phi, summed over all radial layers.
 *  This tool creates towers from a single cell collection (from one calorimeter).
 *
 *  For more explanation please [see reconstruction documentation](@ref md_reconstruction_doc_reccalorimeter).
 *
 *  @author Anna Zaborowska
 */

class SingleCaloTowerTool : public GaudiTool, virtual public ITowerTool {
public:
  SingleCaloTowerTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SingleCaloTowerTool() = default;
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;
  /**  Prepare calorimeter towers.
   *  Create empty towers for the calorimeter.
   */
  virtual std::array<uint,2> prepareTowers(uint m_nEtaFinal, uint m_nPhiFinal) final;
  /**  Build calorimeter towers.
   *  Tower is segmented in eta and phi, with the energy from all layers (no r segmentation).
   *  Cuurently the cells need to be included only in one tower.
   *  TODO: split cell energy into more towers if cell size is larger than the tower
   */
  virtual uint buildTowers(std::vector<std::vector<float>> & aTowers) final;
  /**  Get the tower IDs in eta.
   *   @param[in] aEta Position of the calorimeter cell in eta
   *   @return ID (eta) of a tower
   */
  virtual uint idEta(float aEta) const final;
  /**  Get the tower IDs in phi.
   *   @param[in] aPhi Position of the calorimeter cell in phi
   *   @return ID (phi) of a tower
   */
  virtual uint idPhi(float aPhi) const final;
  /**  Get the eta position of the centre of the tower.
   *   @param[in] aIdEta ID (eta) of a tower
   *   @return Position of the centre of the tower
   */
  virtual float eta(int aIdEta) const final;
  /**  Get the phi position of the centre of the tower.
   *   @param[in] aIdPhi ID (phi) of a tower
   *   @return Position of the centre of the tower
   */
  virtual float phi(int aIdPhi) const final;
  virtual void matchCells(float aEta, float aPhi, uint aHalfEtaFinal, uint aHalfPhiFinal, fcc::CaloCluster& aEdmCluster) final;
  virtual float radiusForPosition() const final;
private:
  /// Handle for calo cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_cells;
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Name of the detector readout
  std::string m_readoutName;
  /// Name of the fields describing the segmented calorimeter
  std::vector<std::string> m_fieldNames;
  /// Values of the fields describing the segmented calorimeter
  std::vector<int> m_fieldValues;
  /// PhiEta segmentation (owned by DD4hep)
  DD4hep::DDSegmentation::GridPhiEta* m_segmentation;
  /// Radius used to calculate cluster position from eta and phi. If not defined, radius from det::utils::tubeDimensions is used
  double m_innerRadius;
  /// Volume ID of the volume with cells to calculate
  uint64_t m_volumeId;
  /// Size of the tower in eta
  float m_deltaEtaTower;
  /// Size of the tower in phi
  float m_deltaPhiTower;
  /// number of towers in eta (calculated from m_deltaEtaTower and the eta size of the first layer)
  int m_nEtaTower;
  /// Number of towers in phi (calculated from m_deltaPhiTower)
  int m_nPhiTower;
};

#endif /* RECCALORIMETER_SINGLECALOTOWERTOOL_H */
