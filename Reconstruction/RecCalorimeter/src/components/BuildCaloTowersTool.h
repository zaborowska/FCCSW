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
 *  The cell size cannot be larger than the tower size at the moment
 *  TODO: split cell energy into more towers if cell size is larger than the tower!!!!!
 *       - problem with merging 2 cells (eta[0]=0.0)
 *       - probably have to shift the center of the cell, think about numbering of towers
 *  @author Jana Faltova
 *  @date   2016-11
 */

class BuildCaloTowersTool : public GaudiTool, virtual public IBuildCaloTowersTool  {
public:
  BuildCaloTowersTool(const std::string& type,const std::string& name, const IInterface* parent);
  ~BuildCaloTowersTool();
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  /** @brief 
   */
  virtual std::unordered_map<std::pair<int,int>, float, boost::hash<std::pair<int, int>>> buildTowers(const fcc::CaloHitCollection& aCells) final;

  void findTower(const fcc::CaloHit& aCell, std::pair<int, int>& itower);



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
