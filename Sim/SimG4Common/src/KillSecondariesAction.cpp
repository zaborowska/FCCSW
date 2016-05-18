#include "SimG4Common/KillSecondariesAction.h"

#include "G4Track.hh"
#include "G4TrackStatus.hh"

namespace sim {
KillSecondariesAction::KillSecondariesAction() : G4UserStackingAction() {}

KillSecondariesAction::~KillSecondariesAction() {}

G4ClassificationOfNewTrack KillSecondariesAction::ClassifyNewTrack(const G4Track* aTrack)
{
  G4ClassificationOfNewTrack     classification = fUrgent;
  if(aTrack->GetParentID() != 0) classification = fKill;
  return classification;
}
}
