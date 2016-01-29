#ifndef SIM_ITOOLWITHOUTPUT_H
#define SIM_ITOOLWITHOUTPUT_H

// GAUDI
#include "GaudiKernel/IAlgTool.h"

class IToolWithOutput: virtual public IAlgTool {
public:
  /// Retrieve interface ID
  DeclareInterfaceID(IToolWithOutput,1,0);
  /// Initialize.
  virtual StatusCode initialize() = 0;
  /// Finalize.
  virtual StatusCode finalize() = 0;
  /// Save the output.
  virtual StatusCode saveOutput() = 0;
};

#endif /* SIM_ITOOLWITHOUTPUT_H */
