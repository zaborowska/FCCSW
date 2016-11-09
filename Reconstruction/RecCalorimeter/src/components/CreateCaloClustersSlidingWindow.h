#ifndef RECCALORIMETER_CREATECALOCLUSTERSSLIDINGWINDOW_H
#define RECCALORIMETER_CREATECALOCLUSTERSSLIDINGWINDOW_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "DetSegmentation/GridPhiEta.h"
class IGeoSvc;

// datamodel
#include "datamodel/CaloHit.h"
namespace fcc {
class CaloHitCollection;
class CaloClusterCollection;
}

// DD4hep
#include "DD4hep/Readout.h"
namespace DD4hep {
namespace DDSegmentation {
  class Segmentation;
}
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
  /**  Build calorimeter towers.
   *  Tower is segmented in eta and phi, with the energy from all layers.
   *  It is represented by a pair of IDs (IDeta, IDphi) and the energy.
   *  Tower IDs are used to ease the navigation.
   *  Cuurently the cell size cannot be larger than the tower size.
   *  TODO: split cell energy into more towers if cell size is larger than the tower
   *       - problem with merging 2 cells (eta[0]=0.0)
   *       - probably have to shift the center of the cell, think about numbering of towers
   *  @param[in] aCells Input collection of cells
   *  @return Towers
   */
  std::unordered_map<std::pair<int,int>, float, boost::hash<std::pair<int, int>>> buildTowers(const fcc::CaloHitCollection& aCells);
  /**  Get the tower IDs in eta and phi.
   *   The position of a centre of the cell is checked and the ID is computed.
   *   @param[in] aCell Calorimeter cell
   *   @param[out] aTower ID (eta,phi) of a tower
   */
  void findTower(const fcc::CaloHit& aCell, std::pair<int, int>& aTower);
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Handle for calo cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_cells;
  /// Handle for calo clusters (output collection)
  DataHandle<fcc::CaloClusterCollection> m_clusters;
  /// PhiEta segmentation
  DD4hep::DDSegmentation::GridPhiEta* m_segmentation;
  /// Name of the detector readout
  std::string m_readoutName;
  /// Size of the tower in eta
  float m_deltaEtaTower;
  /// Size of the tower in phi
  float m_deltaPhiTower;
};

#endif /* RECCALORIMETER_CREATECALOCLUSTERSSLIDINGWINDOW_H */
