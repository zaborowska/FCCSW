#ifndef SIM_TESTALGORITHM_H
#define SIM_TESTALGORITHM_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "IToolWithOutput.h"

// albers
#include "datamodel/ParticleCollection.h"

class TestToolAlgorithm: public GaudiAlgorithm {
   friend class AlgFactory<TestToolAlgorithm> ;
public:
   /// Constructor.
   TestToolAlgorithm(const std::string& name, ISvcLocator* svcLoc);
   /// Initialize.
   virtual StatusCode initialize();
   /// Execute.
   virtual StatusCode execute();
   /// Finalize.
   virtual StatusCode finalize();
private:
  /// Handle for the particles to be written
  ToolHandle<IToolWithOutput> m_tool;
};

#endif
