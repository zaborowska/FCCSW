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
  /**  Initialize the Geant simulation service.
   *   Tools set in the configuration file need to specify the detector construction,
   *   physics list and user action initialization to initialize G4RunManager.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize the Geant simulation service.
   *   @return status code
   */
  virtual StatusCode finalize() final;
private:
   /// Pointer to the tool service
  SmartIF<IToolSvc> m_toolSvc;
  ToolHandle<IToolWithOutput> m_tool;
};

#endif
