#include "SimG4Full/ExitTrackerSteppingAction.h"

#include<iostream>
#include<string>

// FCCSW
#include "SimG4Common/ExitTrackerParticleInformation.h"

// Geant4
#include "G4Step.hh"
#include "G4PrimaryParticle.hh"
#include "G4PathFinder.hh"
#include "G4SystemOfUnits.hh"

namespace sim {
ExitTrackerSteppingAction::ExitTrackerSteppingAction(): G4UserSteppingAction() {}

ExitTrackerSteppingAction::~ExitTrackerSteppingAction() {}

void ExitTrackerSteppingAction::UserSteppingAction(const G4Step* aStep) {
  if(aStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) {
    std::cout<<"=======\t change from: "<<aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()<<"  to  "<<aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName()<<std::endl;

    // entrance to the tracker volume:
    if(aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName().find("Tracker")==std::string::npos
       && aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName().find("Tracker")!=std::string::npos
       && aStep->GetTrack()->GetDynamicParticle()->GetPrimaryParticle()) {
      // compute the exit position assuming current momentum and no physics processes
      const G4Track* track = aStep->GetTrack();
      CLHEP::Hep3Vector spin = track->GetPolarization() ;
      G4FieldTrack theFieldTrack = G4FieldTrack( track->GetPosition(),
                                             track->GetMomentumDirection(),
                                             0.0,
                                             track->GetKineticEnergy(),
                                             track->GetDynamicParticle()->GetDefinition()->GetPDGMass(),
                                             0.0,
                                             track->GetGlobalTime(), // Lab.
                                             track->GetProperTime(), // Part.
                                             &spin) ;
      double retSafety= -1.0;
      ELimited retStepLimited;
      G4FieldTrack endTrack('a');
      double currentMinimumStep= 10*m;
      G4PathFinder* fPathFinder = G4PathFinder::GetInstance();
      fPathFinder->ComputeStep( theFieldTrack,
                                currentMinimumStep,
                                0,
                                track->GetCurrentStepNumber(),
                                retSafety,
                                retStepLimited,
                                endTrack,
                                track->GetVolume() );

      // associate tracker-entrance information with particles in the event
      track->GetDynamicParticle()->GetPrimaryParticle()->SetUserInformation(
        new ExitTrackerParticleInformation(aStep->GetPostStepPoint()->GetPosition().eta(),
                                           aStep->GetTrack()->GetMomentum(),
                                           endTrack.GetPosition()));




    }
  }
}
}
