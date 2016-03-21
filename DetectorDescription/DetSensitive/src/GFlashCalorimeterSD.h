#ifndef DETSENSITIVE_GFLASHCALORIMETERSD_H
#define DETSENSITIVE_GFLASHCALORIMETERSD_H

#include "G4VSensitiveDetector.hh"
#include "G4VGFlashSensitiveDetector.hh"
#include "DDG4/Geant4Hits.h"
#include "G4THitsCollection.hh"

class G4Step;
class G4GFlashSpot;
class G4HCofThisEvent;
class G4TouchableHistory;
#include "DDSegmentation/Segmentation.h"

namespace det {
class GFlashCalorimeterSD : public G4VSensitiveDetector, public G4VGFlashSensitiveDetector {
public:
  GFlashCalorimeterSD(std::string aDetectorName, std::string aReadoutName, DD4hep::Geometry::Segmentation aSeg);
  ~GFlashCalorimeterSD();

  void Initialize(G4HCofThisEvent* aHCE);
  virtual bool ProcessHits(G4Step* aStep, G4TouchableHistory* aROhist) final;
  virtual bool ProcessHits(G4GFlashSpot* aSpot, G4TouchableHistory* aROhist) final;
  uint64_t getCellID(G4Step* aStep);
  uint64_t getCellID(G4GFlashSpot* aSpot);
private:
  G4THitsCollection<DD4hep::Simulation::Geant4CalorimeterHit> *calorimeterCollection;
  DD4hep::Geometry::Segmentation m_seg;
};
}

#endif /* DETSENSITIVE_GFLASHCALORIMETERSD_H */
