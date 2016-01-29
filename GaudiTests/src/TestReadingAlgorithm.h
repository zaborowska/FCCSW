#ifndef SIM_TESTREADINGALGORITHM_H
#define SIM_TESTREADINGALGORITHM_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"

// albers
#include "datamodel/ParticleCollection.h"

class TestReadingAlgorithm: public GaudiAlgorithm {
   friend class AlgFactory<TestReadingAlgorithm> ;
public:
   /// Constructor.
   TestReadingAlgorithm(const std::string& name, ISvcLocator* svcLoc);
   /// Initialize.
   virtual StatusCode initialize();
   /// Execute.
   virtual StatusCode execute();
   /// Finalize.
   virtual StatusCode finalize();
private:
  /// Handle for the particles to be read
  DataHandle<ParticleCollection> m_particles;
};

#endif
