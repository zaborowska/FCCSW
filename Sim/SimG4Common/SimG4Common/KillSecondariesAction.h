#ifndef SIMG4COMMON_KILLSECONDARIESACTION_H
#define SIMG4COMMON_KILLSECONDARIESACTION_H

#include "G4UserStackingAction.hh"

class G4Track;

namespace sim {
class KillSecondariesAction : public G4UserStackingAction {
public:
  KillSecondariesAction();
  ~KillSecondariesAction();
  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* ) final;
};
}
#endif /* SIMG4COMMON_KILLSECONDARIESACTION_H */
