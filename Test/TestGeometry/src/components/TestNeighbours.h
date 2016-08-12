#ifndef TESTGEOMETRY_TESTNEIGHBOURS_H
#define TESTGEOMETRY_TESTNEIGHBOURS_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;

// DD4hep
namespace DD4hep {
namespace DDSegmentation {
  class BitField64;
}
}

// datamodel
namespace fcc {
class CaloHitCollection;
}

/** @class TestNeighbours TestGeometry/src/components/TestNeighbours.h TestNeighbours.h
 *
 *  Algorithm testing lookup of neighbours
 *
 *  @author Anna Zaborowska
 */

class TestNeighbours: public GaudiAlgorithm {
public:
  explicit TestNeighbours(const std::string&, ISvcLocator*);
  virtual ~TestNeighbours();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Execute.
   *   @return status code
   */
  virtual StatusCode execute() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;
private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Handle for the EDM Hits to be read
  DataHandle<fcc::CaloHitCollection> m_inHits;
  /// Name of the detector readout
  std::string m_readoutName;
  /// Pointer to the bitfield decoder
  DD4hep::DDSegmentation::BitField64* m_decoder;
};
#endif /* TESTGEOMETRY_TESTNEIGHBOURS_H */
