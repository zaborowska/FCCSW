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

private:
  /// DeltaEta size of the tower
  double m_deltaEtaTower;
  /// DeltePhi size of the tower
  double m_deltaPhiTower;

};

#endif /* RECCALORIMETER_BUILDCALOTOWERSTOOL_H */
