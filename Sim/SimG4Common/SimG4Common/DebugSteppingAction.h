#ifndef SIMG4COMMON_DEBUGSTEPPINGACTION_H
#define SIMG4COMMON_DEBUGSTEPPINGACTION_H

#include "G4UserSteppingAction.hh"

namespace sim {
class DebugSteppingAction : public G4UserSteppingAction {
public:
  DebugSteppingAction();
  ~DebugSteppingAction() {};
  virtual void UserSteppingAction(const G4Step*) final;
};
}
#endif /* SIMG4COMMON_DEBUGSTEPPINGACTION_H */
