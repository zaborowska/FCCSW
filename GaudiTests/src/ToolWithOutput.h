#ifndef SIM_TOOLWITHOUTPUT_H
#define SIM_TOOLWITHOUTPUT_H

// GAUDI
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "IToolWithOutput.h"

// albers
#include "datamodel/ParticleCollection.h"

class ToolWithOutput: public GaudiTool, public IToolWithOutput {
public:
  /// Constructor.
  ToolWithOutput(const std::string& aType , const std::string& aName,
                 const IInterface* aParent);
  /// Destructor.
  virtual ~ToolWithOutput();
  /// Initialize.
  virtual StatusCode initialize() final;
  /// Finalize.
  virtual StatusCode finalize() final;
  /// Save the output.
  virtual StatusCode saveOutput() final;
private:
  /// Handle for the particles to be written
  DataHandle<ParticleCollection> m_particles;
};

#endif /* SIM_TOOLWITHOUTPUT_H */
