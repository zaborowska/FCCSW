#ifndef SIM_TOOLWITTOOL_H
#define SIM_TOOLWITTOOL_H

// GAUDI
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "IToolWithOutput.h"

// albers
#include "datamodel/ParticleCollection.h"

class ToolWithTool: public GaudiTool, public IToolWithOutput {
public:
  /// Constructor.
  ToolWithTool(const std::string& aType , const std::string& aName,
                 const IInterface* aParent);
  /// Destructor.
  virtual ~ToolWithTool();
  /// Initialize.
  virtual StatusCode initialize() final;
  /// Finalize.
  virtual StatusCode finalize() final;
  /// Save the output.
  virtual StatusCode saveOutput() final;
private:
  /// Handle for the particles to be written
  ToolHandle<IToolWithOutput> m_tool;
};

#endif /* SIM_TOOLWITHTOOL_H */
