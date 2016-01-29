#ifndef SIM_TESTALGORITHM_H
#define SIM_TESTALGORITHM_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "IToolWithMember.h"

// albers
#include "datamodel/ParticleCollection.h"

class TestMultipleToolInstancesAlgorithm: public GaudiAlgorithm {
   friend class AlgFactory<TestMultipleToolInstancesAlgorithm> ;
public:
   /// Constructor.
   TestMultipleToolInstancesAlgorithm(const std::string& name, ISvcLocator* svcLoc);
   /// Initialize.
   virtual StatusCode initialize();
   /// Execute.
   virtual StatusCode execute();
   /// Finalize.
   virtual StatusCode finalize();
private:
  /// Handle for the particles to be written
  //ToolHandle<IToolWithOutput> m_tool1;
  //ToolHandle<IToolWithOutput> m_tool2;
  ToolHandle<IToolWithMember> m_tool1;
  ToolHandle<IToolWithMember> m_tool2;
  std::string m_toolname2 = "tool2";
  std::string m_toolname1 = "tool1";
};

#endif
