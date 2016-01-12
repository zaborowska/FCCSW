#include "SimG4Common/ExitTrackerParticleInformation.h"

namespace sim {
ExitTrackerParticleInformation::ExitTrackerParticleInformation(double aEta, const CLHEP::Hep3Vector& aMom, const CLHEP::Hep3Vector& aExit):
  m_entranceEta(aEta), m_entranceMomentum(aMom), m_computedExitPosition(aExit), m_fullSimExitPosition(0) {}

ExitTrackerParticleInformation::~ExitTrackerParticleInformation() {}

void ExitTrackerParticleInformation::Print() const {}

double ExitTrackerParticleInformation::entranceEta() const {
  return m_entranceEta;
}

const CLHEP::Hep3Vector& ExitTrackerParticleInformation::entranceMomentum() const {
  return m_entranceMomentum;
}

const CLHEP::Hep3Vector& ExitTrackerParticleInformation::computedExitPosition() const {
  return m_computedExitPosition;
}

void ExitTrackerParticleInformation::setFullSimExitPosition(const CLHEP::Hep3Vector& aPos) {
  m_fullSimExitPosition = aPos;
}

const CLHEP::Hep3Vector& ExitTrackerParticleInformation::fullSimExitPosition() const {
  return m_fullSimExitPosition;
}

}
