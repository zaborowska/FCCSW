#include "SimG4Full/FullSimActions.h"

// FCCSW
#include "SimG4Full/ExitTrackerSteppingAction.h"

namespace sim {
FullSimActions::FullSimActions(): G4VUserActionInitialization() {}

FullSimActions::~FullSimActions() {}

void FullSimActions::Build() const {
  SetUserAction(new ExitTrackerSteppingAction);
}
}
