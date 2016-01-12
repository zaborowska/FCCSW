#ifndef SIMG4FULL_EXITTRACKERSTEPPINGACTION_H
#define SIMG4FAST_EXITTRACKERSTEPPINGACTION_H

// Geant
#include "G4UserSteppingAction.hh"

/** @class ExitTrackerSteppingAction SimG4Full/SimG4Full/ExitTrackerSteppingAction.h ExitTrackerSteppingAction.h
 *
 *  Stepping action (invoked after Geant step processing).
 *  Stores the exit position from the Tracker volume and compares it with the computed value
 *  by the Coupled Transportation done at the entrance to that volume (as it is done by fast simulation).
 *  That enables extracting the position smearing parameters that rely only on physics processes
 *  in the tracker, not on the reconstruction process. Smeared position will be used further
 *  in fast simulation as the entrance to next volume.
 *
 *  @author Anna Zaborowska
*/

namespace sim {
class ExitTrackerSteppingAction : public G4UserSteppingAction {
public:
  ExitTrackerSteppingAction();
  virtual ~ExitTrackerSteppingAction();
  /* Defines the actions at the end of processing the step.
   * Computes the exit position from the tracker with the entrance-point momentum (using Coupled Transportation).
   * Compares it with the fully simulated passage of the particle through the detector. The exit point from
   * the full sim is compared with the previously computed one.
   * Difference is registered together with the pseudorapidity value and entrance-momentum for later creation of
   * the resolution files for the smearing procedure.
   * param[in] aStep Simulated step.
   */
  virtual void UserSteppingAction(const G4Step* aStep) final;
};
}

#endif /* SIMG4FULL_EXITTRACKERSTEPPINGACTION_H */


