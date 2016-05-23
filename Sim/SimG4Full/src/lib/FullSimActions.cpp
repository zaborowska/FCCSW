#include "SimG4Full/FullSimActions.h"
#include "SimG4Common/KillSecondariesAction.h"

namespace sim {
FullSimActions::FullSimActions():
  G4VUserActionInitialization() {}

FullSimActions::~FullSimActions() {}

void FullSimActions::Build() const {
  //SetUserAction(new KillSecondariesAction);
}
}
