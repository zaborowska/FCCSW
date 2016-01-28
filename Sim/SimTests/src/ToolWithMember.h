#ifndef SIM_TOOLWITHMEMBER_H
#define SIM_TOOLWITHMEMBER_H

// GAUDI
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "IToolWithMember.h"

class ToolWithMember: public GaudiTool, public IToolWithMember {
public:
  /// Constructor.
  ToolWithMember(const std::string& aType , const std::string& aName,
                 const IInterface* aParent);
  /// Destructor.
  virtual ~ToolWithMember();
  /// Initialize.
  virtual StatusCode initialize() final;
  /// Finalize.
  virtual StatusCode finalize() final;

  virtual void setFilename(const std::string& filename);
  virtual void printFilename();
private:
  std::string m_filename;
};

#endif /* SIM_TOOLWITHMEMBER_H */
