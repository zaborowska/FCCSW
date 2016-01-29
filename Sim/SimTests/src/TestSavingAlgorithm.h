#ifndef SIM_TESTSAVINGALGORITHM_H
#define SIM_TESTSAVINGALGORITHM_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

//FCCSW
#include "IToolWithOutput.h"

class TestSavingAlgorithm: public GaudiAlgorithm {
   friend class AlgFactory<TestSavingAlgorithm> ;
public:
   /// Constructor.
   TestSavingAlgorithm(const std::string& name, ISvcLocator* svcLoc);
   /// Initialize.
   virtual StatusCode initialize();
   /// Execute.
   virtual StatusCode execute();
   /// Finalize.
   virtual StatusCode finalize();
private:
   ToolHandle<IToolWithOutput> m_toolWithOutput;
};

#endif
