#ifndef SIM_G4FASTSIMPHYSICSLIST_H
#define SIM_G4FASTSIMPHYSICSLIST_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimG4Interface/IG4PhysicsList.h"

class G4FastSimPhysicsList: public GaudiTool, virtual public IG4PhysicsList {
public:
  G4FastSimPhysicsList(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~G4FastSimPhysicsList();

  virtual StatusCode initialize();

  virtual G4VModularPhysicsList* getPhysicsList();

private:
   /// Handle for the full physics list tool
   IG4PhysicsList* m_physicsListTool;
   /// Name of the full physics list tool
   std::string m_physicsListToolName;

};

#endif /* SIM_G4FASTSIMPHYSICSLIST_H */
