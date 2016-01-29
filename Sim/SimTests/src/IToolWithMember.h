#ifndef SIM_ITOOLWITHMEMBER_H
#define SIM_ITOOLWITHMEMBER_H

// GAUDI
#include "GaudiKernel/IAlgTool.h"

class IToolWithMember: virtual public IAlgTool {
public:
  /// Retrieve interface ID
  DeclareInterfaceID(IToolWithMember,1,0);
  /// Initialize.
  virtual StatusCode initialize() = 0;
  /// Finalize.
  virtual StatusCode finalize() = 0;
  /// Save the output.
  /// sample member routines
  virtual void setFilename(const std::string& filename) = 0;
  virtual void printFilename() = 0;
};

#endif /* SIM_ITOOLWITHMEMBER_H */
