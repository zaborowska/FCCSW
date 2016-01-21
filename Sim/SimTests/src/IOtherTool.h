#ifndef SIM_IOTHERTOOL_H
#define SIM_IOTHERTOOL_H

// GAUDI
#include "GaudiKernel/IAlgTool.h"

class IOtherTool: virtual public IAlgTool {
public:
  /// Retrieve interface ID
  DeclareInterfaceID(IOtherTool,1,0);
  /// Initialize.
  virtual StatusCode initialize() = 0;
  /// Finalize.
  virtual StatusCode finalize() = 0;
};

#endif /* SIM_ITOOLWITHOUTPUT_H */
