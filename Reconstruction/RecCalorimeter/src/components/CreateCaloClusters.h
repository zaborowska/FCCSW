#ifndef RECCALORIMETER_CREATECALOCLUSTERS_H
#define RECCALORIMETER_CREATECALOCLUSTERS_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "FWCore/DataHandle.h"

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloClusterCollection.h"

#include "RecInterface/IBuildCaloTowersTool.h"

/** @class CreateCaloClusters
 *
 *  Algorithm for creating calorimeter clusters from cells. 
 * 
 *  Sliding window algorithm: Create calorimeter towers, find local maxima with sliding window of a fixed size in phi x eta, build clusters of fixed size in phi x eta (different from sliding window size)
 *  Sliding window good for electrons/gamma reconstruction.
 *  Topological clusters should be better for jets - not implemented yet
 *     
 *  @author Jana Faltova
 *  @date   2016-11
 *
 */

class CreateCaloClusters : public GaudiAlgorithm 
{
public:
  CreateCaloClusters(const std::string& name, ISvcLocator* svcLoc);
 
  ~CreateCaloClusters();

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:

  /// Handle for building calorimeter towers (needed for sliding window)
  ToolHandle<IBuildCaloTowersTool> m_buildTowersTool;
 
  /// Handle for calo cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_cells;
  /// Handle for calo clusters (output collection)
  DataHandle<fcc::CaloClusterCollection> m_clusters;

};

#endif /* RECCALORIMETER_CREATECALOCLUSTERS_H */
