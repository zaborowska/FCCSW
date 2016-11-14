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

// Cluster object
struct cluster {
  int ieta;
  int iphi;
  float transEnergy;
  float eta;
  float phi;
};

// Sort
struct compareCluster {
  inline bool operator() (const cluster& struct1, const cluster& struct2)
    {
        return (struct1.transEnergy > struct2.transEnergy);
    }
};


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
  /**  Prepare calorimeter towers.
   *  Create map with towers for the calorimeter.
   */
  void prepareTowers();
  /**  Build calorimeter towers.
   *  Tower is segmented in eta and phi, with the energy from all layers.
   *  It is represented by a pair of IDs (IDeta, IDphi) and the energy.
   *  Tower IDs are used to ease the navigation.
   *  Cuurently the cell size cannot be larger than the tower size.
   *  TODO: split cell energy into more towers if cell size is larger than the tower
   *       - problem with merging 2 cells (eta[0]=0.0)
   *       - probably have to shift the center of the cell, think about numbering of towers
   */
  void buildTowers();
  /**  Get the tower IDs in eta and phi.
   *   The position of a centre of the cell is checked and the ID is computed.
   *   @param[in] aCell Calorimeter cell
   *   @param[out] aTower ID (eta,phi) of a tower
   */
  void findTower(const fcc::CaloHit& aCell, std::pair<int, int>& aTower);

  void prepareTestTowers();

  void buildTestTowers();

  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Handle for calo cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_cells;
  /// Handle for calo clusters (output collection)
  DataHandle<fcc::CaloClusterCollection> m_clusters;
  /// PhiEta segmentation
  DD4hep::DDSegmentation::GridPhiEta* m_segmentation;
  // calorimeter towers
  /* std::unordered_map<std::pair<int,int>, float, boost::hash<std::pair<int, int>>> m_towers; */
  std::unordered_map<int, std::vector<float>> m_towers;
  /// Name of the detector readout
  std::string m_readoutName;
  /// Size of the tower in eta
  float m_deltaEtaTower;
  /// Size of the tower in phi
  float m_deltaPhiTower;
  /// number of towers in eta (calculated from m_deltaEtaTower and the eta size of the first layer)
  int m_nEtaTower;
  /// Number of towers in phi (calculated from m_deltaPhiTower)
  int m_nPhiTower;
  /// Size of the window in eta for pre-clusters (in units of tower size)
  int m_nEtaWindow;
  /// Size of the window in phi for pre-clusters (in units of tower size)
  int m_nPhiWindow;
  /// Size of the window in eta for position calculation (in units of tower size)
  int m_nEtaPosition;
  /// Size of the window in phi for position calculation (in units of tower size)
  int m_nPhiPosition;
 /// Size of the window in eta for the overlap removal (in units of tower size)
  int m_nEtaDuplicates;
  /// Size of the window in phi for the overlap removal (in units of tower size)
  int m_nPhiDuplicates;
 /// Size of the window in eta for the final object (in units of tower size)
  int m_nEtaObject;
  /// Size of the window in phi for the final object (in units of tower size)
  int m_nPhiObject;
  /// Energy threshold for cluster finding
  float m_energyThreshold;
  /// Energy threshold in the window for the position calculation
  float m_energyPosThreshold;
  /// Flag if a check on local maxima in phi should be done (temporary)
  bool m_checkPhiLocalMax;
  /// Flag if a check on local maxima in eta should be done (temporary)
  bool m_checkEtaLocalMax;
  /// Vector of clusters
  std::vector<cluster> m_preClusters;

};

#endif /* RECCALORIMETER_CREATECALOCLUSTERSSLIDINGWINDOW_H */
