#ifndef SIM_TESTGENERALALGORITHM_H
#define SIM_TESTGENERALALGORITHM_H

// GAUDI
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"

class TestGeneralAlgorithm: public Algorithm {
   friend class AlgFactory<TestGeneralAlgorithm> ;
public:
   /// Constructor.
   TestGeneralAlgorithm(const std::string& name, ISvcLocator* svcLoc);
   /// Initialize.
   virtual StatusCode initialize();
   /// Execute.
   virtual StatusCode execute();
   /// Finalize.
   virtual StatusCode finalize();
private:
  /// Handle for the particles to be written
  ToolHandle<IAlgTool> m_tool;
};

#endif
