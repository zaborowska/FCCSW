#ifndef SIM_TESTALGORITHM_H
#define SIM_TESTALGORITHM_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "ToolWithVector.h"

class TestVectorAlgorithm: public GaudiAlgorithm {
   friend class AlgFactory<TestVectorAlgorithm> ;
public:
   /// Constructor.
   TestVectorAlgorithm(const std::string& name, ISvcLocator* svcLoc);
   /// Initialize.
   virtual StatusCode initialize();
   /// Execute.
   virtual StatusCode execute();
   /// Finalize.
   virtual StatusCode finalize();
private:
  /// Handle for the particles to be written
  ToolHandle<ToolWithVector> m_tool;
  std::vector<std::string> m_string;
  std::vector<int> m_int;
  std::vector<double> m_double;
};

#endif
