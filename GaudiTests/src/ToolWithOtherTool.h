#ifndef SIM_TOOLWITHOTHERTOOL_H
#define SIM_TOOLWITHOTHERTOOL_H

// GAUDI
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "IToolWithOutput.h"
#include "IOtherTool.h"

// albers
#include "datamodel/ParticleCollection.h"

class ToolWithOtherTool: public GaudiTool, public IToolWithOutput {
public:
  /// Constructor.
  ToolWithOtherTool(const std::string& aType , const std::string& aName,
                 const IInterface* aParent);
  /// Destructor.
  virtual ~ToolWithOtherTool();
  /// Initialize.
  virtual StatusCode initialize() final;
  /// Finalize.
  virtual StatusCode finalize() final;
  virtual StatusCode saveOutput() final;
private:
  /// Handle for the particles to be written
  ToolHandle<IOtherTool> m_tool;
};

#endif /* SIM_TOOLWITHTOOL_H */
