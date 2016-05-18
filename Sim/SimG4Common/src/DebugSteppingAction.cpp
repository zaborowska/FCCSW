#include "SimG4Common/DebugSteppingAction.h"

#include "G4StepPoint.hh"
#include "G4TouchableHandle.hh"
#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"

namespace sim {
DebugSteppingAction::DebugSteppingAction() : G4UserSteppingAction() {}

void DebugSteppingAction::UserSteppingAction(const G4Step* aStep) {
  G4StepPoint* point1 = aStep->GetPreStepPoint();
  G4ThreeVector pos1 = point1->GetPosition();
  std::cout<<pos1<<"\t"<<aStep->GetStepLength()<<std::endl;
  G4TouchableHandle touch1 = point1->GetTouchableHandle();
  if(touch1->GetVolume()->GetLogicalVolume()->GetSensitiveDetector())
    if(touch1->GetVolume()->GetLogicalVolume()->GetSensitiveDetector()->isActive()) {
    std::cout<<pos1<<std::endl;
  }
}
}
