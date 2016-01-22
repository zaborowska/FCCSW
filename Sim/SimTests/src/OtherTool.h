#ifndef SIM_OTHERTOOL_H
#define SIM_OTHERTOOL_H

// GAUDI
/* #include "GaudiAlg/GaudiTool.h" */
#include "GaudiKernel/AlgTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "IOtherTool.h"

// albers
#include "datamodel/ParticleCollection.h"

class OtherTool: public AlgTool, public IOtherTool {
public:
  /// Constructor.
  OtherTool(const std::string& aType , const std::string& aName,
                 const IInterface* aParent);
  /// Destructor.
  virtual ~OtherTool();
  /// Initialize.
  virtual StatusCode initialize() final;
  /// Finalize.
  virtual StatusCode finalize() final;
};

#endif /* SIM_TOOLWITHTOOL_H */
