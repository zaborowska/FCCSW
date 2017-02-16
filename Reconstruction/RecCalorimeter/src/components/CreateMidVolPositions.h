#ifndef RECCALORIMETER_CREATEMIDVOLPOSITIONS_H
#define RECCALORIMETER_CREATEMIDVOLPOSITIONS_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;

namespace fcc {
class CaloHitCollection;
class PositionedCaloHitCollection;
}

// DD4hep
#include "DD4hep/Readout.h"

/** @class CreateMidVolPositions
 *
 *  Retrieve positions of the cells.
 *  This algorithm saves the centre posiiton of the volume. No segmentation is taken into account
 *
 *  @author Anna Zaborowska
 *
 */


class CreateMidVolPositions : public GaudiAlgorithm
{
public:
  CreateMidVolPositions(const std::string& name, ISvcLocator* svcLoc);

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /// Handle for calo cells (input collection with cellID)
  DataHandle<fcc::CaloHitCollection> m_caloCells;
  /// Handle for positioned hits (output collection)
  DataHandle<fcc::PositionedCaloHitCollection> m_caloPositionedHits;
  /// Name of the detector readout
  std::string m_readoutName;

  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
};

#endif /* RECCALORIMETER_CREATEMIDVOLPOSITIONS_H */
