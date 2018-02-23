#ifndef RECINTERFACE_INOISECORRECTIONTOOL_H
#define RECINTERFACE_INOISECORRECTIONTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// datamodel
#include "datamodel/CaloCluster.h"

struct tower {
  int eta;
  int phi;
};

/** @class INoiseCorrectionTool RecInterface/RecInterface/INoiseCorrectionTool.h INoiseCorrectionTool.h
 *
 *  Abstract interface to noise correction applied to clusters tool.
 *
 *  @author Anna Zaborowska
 */

class INoiseCorrectionTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(INoiseCorrectionTool, 1, 0);
  /**  Apply correction to the energy of a cluster acknowledging the correlation of pile-up noise.
   *   @param[out] aEdmCluster Cluster
   */
  virtual void correctEnergy(fcc::CaloCluster& aEdmCluster) = 0;
};

#endif /* RECINTERFACE_INOISECORRECTIONTOOL_H */
