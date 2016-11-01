#ifndef RECINTERFACE_IBUILDCALOTOWERSTOOL_H
#define RECINTERFACE_IBUILDCALOTOWERSTOOL_H

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"

#include <unordered_map>
#include <utility>  

/** @class IBuildCaloTowersTool
 *
 *  Abstract interface to build calorimeter towers
 *  Tower presented by a segment in eta, phi and energy (unordered_map)
 *     -> easy navigation from one tower to another
 *  @author Jana Faltova
 *  @date   2016-10
 */

class IBuildCaloTowersTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IBuildCaloTowersTool, 1, 0);

  /** @brief Tower presented by a segment in eta, phi (std::pair) and energy (float)
  */
  virtual std::unordered_map<int, float> buildTowers(std::vector<fcc::CaloHit*>& aCells) = 0;
};

#endif /* RECINTERFACE_IBUILDCALOTOWERSTOOL_H */
