#ifndef SIMG4TEST_SVC_H
#define SIMG4TEST_SVC_H

// FCCSW
#include "IToolWithOutput.h"

//Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ToolHandle.h"

class TestToolService: public extends1<Service, IService> {
public:
  /// Standard constructor
  TestToolService(const std::string& aName, ISvcLocator* aSL);
  /// Standard destructor
  virtual ~TestToolService();
  /**  Initialize
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize
   *   @return status code
   */
  virtual StatusCode finalize() final;
private:
   /// Pointer to the tool service
  SmartIF<IToolSvc> m_toolSvc;
  ToolHandle<IToolWithOutput> m_tool;
};

#endif
