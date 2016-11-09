#ifndef RECCALORIMETER_CREATECALOCLUSTERSSLIDINGWINDOW_H
#define RECCALORIMETER_CREATECALOCLUSTERSSLIDINGWINDOW_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/IBuildCaloTowersTool.h"

// datamodel
namespace fcc {
class CaloHitCollection;
class CaloClusterCollection;
}

/** @class CreateCaloClustersSlidingWindow
 *
 *  Algorithm for creating calorimeter clusters from cells.
 *
 *  Sliding window algorithm:
 *  1. Create calorimeter towers;
 *  2. Find local maxima with sliding window of a fixed size in phi x eta;
 *  3. Build clusters of fixed size in phi x eta (can differ from sliding window size)
 *  Sliding window performs well for electrons/gamma reconstruction.
 *  Topological clusters should be better for jets.
 *
 *  @author Jana Faltova
 *  @author Anna Zaborowska
 *  @date   2016-11
 *
 */

class CreateCaloClustersSlidingWindow : public GaudiAlgorithm {
public:
  CreateCaloClustersSlidingWindow(const std::string& name, ISvcLocator* svcLoc);
  ~CreateCaloClustersSlidingWindow();
  /**  Initialize.
   *   @return status code
   */
  StatusCode initialize();
  /**  Execute.
   *   Perform the sliding window algorithm and build clusters.
   *   @return status code
   */
  StatusCode execute();
  /**  Finalize.
   *   @return status code
   */
  StatusCode finalize();

private:
  /// Handle to the tool building the calorimeter towers
  ToolHandle<IBuildCaloTowersTool> m_buildTowersTool;
  /// Handle for calo cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_cells;
  /// Handle for calo clusters (output collection)
  DataHandle<fcc::CaloClusterCollection> m_clusters;
};

#endif /* RECCALORIMETER_CREATECALOCLUSTERSSLIDINGWINDOW_H */
