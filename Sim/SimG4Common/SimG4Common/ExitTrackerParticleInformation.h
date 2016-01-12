#ifndef SIMG4COMMON_EXITTRACKERPARTICLEINFORMATION_H
#define SIMG4COMMON_EXITTRACKERPARTICLEINFORMATION_H

// albers
#include "datamodel/MCParticleHandle.h"
#include "datamodel/ParticleHandle.h"

// Geant4
#include "G4VUserPrimaryParticleInformation.hh"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

/** @class ExitTrackerParticleInformation SimG4Common/SimG4Common/ExitTrackerParticleInformation.h ExitTrackerParticleInformation.h
 *
 *  Describes the information that can be assosiated with a G4PrimaryParticle class object.
 *  It contains the information about the entrance and exit parameters of the particle in the tracker volume.
 *  It is used in the full simulation in Geant to compute the smearing resolutions of the position at the exit
 *  of the tracker detector. Those resolution depend only on the physics effects, not on the reconstruction.
 *  They will be used in the fast sim to smear the exit position from the tracker that will be used as
 *  the entry position to the next volume.
 *  Information in this information is filled in sim::ExitTrackerSteppingAction::UserSteppingAction).
 *
 *  @author Anna Zaborowska
 */

namespace sim {
class ExitTrackerParticleInformation: public G4VUserPrimaryParticleInformation {
public:
  /** Constructor.
   *  @param[in] aEta  Pseudorapidity at the tracker entrance.
   *  @param[in] aMom  Momentum at the tracker entrance.
   *  @param[in] aExit Exit position computed at the tracker entrance.
   */
  ExitTrackerParticleInformation(double aEta, const CLHEP::Hep3Vector& aMom, const CLHEP::Hep3Vector& aExit);
  /// Destructor
  virtual ~ExitTrackerParticleInformation();
  /// Printing method
  virtual void Print() const final;
  /** Getter of the pseudorapidity at the entrance point.
   *  @returns Pseudorapidity.
   */
  double entranceEta() const;
  /** Getter of the particle momentum at the entrance point.
   *  @returns Particle momentum.
   */
  const CLHEP::Hep3Vector& entranceMomentum() const;
  /** Getter of the computed exit positon.
   *  @returns Computed exit positon.
   */
  const CLHEP::Hep3Vector& computedExitPosition() const;
  /** Setter of the exit positon from the full simulation.
   *  @returns Eexit positon.
   */
  void setFullSimExitPosition(const CLHEP::Hep3Vector& aPos);
  /** Getter of the exit positon from the full simulation.
   *  @returns Eexit positon.
   */
  const CLHEP::Hep3Vector& fullSimExitPosition() const;
private:
  /// Pseudorapidity at the particle entrance point to the tracker volume
  double m_entranceEta;
  /// Momentum of the particle at the entrance point to the tracker volume
  CLHEP::Hep3Vector m_entranceMomentum;
  /// Computed exit position from the tracker volume (done at the entrance point)
  CLHEP::Hep3Vector m_computedExitPosition;
  /// Fully simulated exit position from the tracker volume
  CLHEP::Hep3Vector m_fullSimExitPosition;
};
}

#endif /* SIMG4COMMON_EXITTRACKERPARTICLEINFORMATION_H */
