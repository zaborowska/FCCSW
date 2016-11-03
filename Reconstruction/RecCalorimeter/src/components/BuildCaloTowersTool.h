#ifndef RECCALORIMETER_BUILDCALOTOWERSTOOL_H
#define RECCALORIMETER_BUILDCALOTOWERSTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "RecInterface/IBuildCaloTowersTool.h"

// our EDM
#include "datamodel/CaloHit.h"

// FCCSW
#include "DetSegmentation/GridPhiEta.h"
class IGeoSvc;

// DD4hep
#include "DD4hep/Readout.h"
namespace DD4hep {
namespace DDSegmentation {
  class Segmentation;
}
}
// C++
//#include <utility>  

/** @class BuildCaloTowersTool
 *
 *  Tool for building calorimeter towers
 *  Tower presented by a segment in eta, phi and energy (unordered_map)
 *     -> easy navigation from one tower to another
 *
 *  @author Jana Faltova
 *  @date   2016-11
 */

class BuildCaloTowersTool : public GaudiTool, virtual public IBuildCaloTowersTool  {
public:
  BuildCaloTowersTool(const std::string& type,const std::string& name, const IInterface* parent);
  ~BuildCaloTowersTool();
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  /** @brief  Calibrate Geant4 hit energy to EM scale
   */
  virtual std::unordered_map<int, float> buildTowers(const fcc::CaloHitCollection& aCells) final;

  std::pair<int, int> findBin(const fcc::CaloHit& aCell, float& weight );

private:
  /// DeltaEta size of the tower
  float m_deltaEtaTower;
  /// DeltePhi size of the tower
  float m_deltaPhiTower;
  /// Name of the detector readout
  std::string m_readoutName;
  
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// PhiEta segmentation
  DD4hep::DDSegmentation::GridPhiEta* m_segmentation;

};

#endif /* RECCALORIMETER_BUILDCALOTOWERSTOOL_H */
