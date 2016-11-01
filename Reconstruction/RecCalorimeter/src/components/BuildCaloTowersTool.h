#ifndef RECCALORIMETER_BUILDCALOTOWERSTOOL_H
#define RECCALORIMETER_BUILDCALOTOWERSTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "RecInterface/IBuildCaloTowersTool.h"

/** @class BuildCaloTowersTool
 *
 *  Tool for building calorimeter towers
 *  Tower presented by a segment in eta, phi and energy (unordered_map)
 *     -> easy navigation from one tower to another
 *
 *  @author Jana Faltova
 *  @date   2016-10
 */

class BuildCaloTowersTool : public GaudiTool, virtual public IBuildCaloTowersTool  {
public:
  BuildCaloTowersTool(const std::string& type,const std::string& name, const IInterface* parent);
  ~BuildCaloTowersTool();
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  /** @brief  Calibrate Geant4 hit energy to EM scale
   */
  virtual std::unordered_map<int, float> buildTowers(std::vector<fcc::CaloHit*>& aCells) final;

  /*
private:
  
 /// Handle for calo hits (input collection)
  DataHandle<fcc::CaloHitCollection> m_cells;
  /// Handle for calo cells (output collection)
  DataHandle<fcc::CaloClusterCollection> m_clusters;
  */

};

#endif /* RECCALORIMETER_BUILDCALOTOWERSTOOL_H */
