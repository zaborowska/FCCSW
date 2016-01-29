#ifndef SIM_TOOLWITHVECTOR_H
#define SIM_TOOLWITHVECTOR_H

// GAUDI
#include "GaudiKernel/AlgTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "IOtherTool.h"

class ToolWithVector: public AlgTool, public IOtherTool {
public:
  /// Constructor.
  ToolWithVector(const std::string& aType , const std::string& aName,
                 const IInterface* aParent);
  /// Destructor.
  virtual ~ToolWithVector();
  /// Initialize.
  virtual StatusCode initialize() final;
  /// Finalize.
  virtual StatusCode finalize() final;
  std::vector<std::string> getString();
  std::vector<int> getInt();
  std::vector<double> getDouble();
private:
  std::vector<std::string> m_string;
  std::vector<int> m_int;
  std::vector<double> m_double;
};

#endif /* SIM_TOOLWITHVECTOR_H */
