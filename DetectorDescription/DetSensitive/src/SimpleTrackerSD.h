#ifndef DETSENSITIVE_SIMPLETRACKERSD_H
#define DETSENSITIVE_SIMPLETRACKERSD_H

#include "G4VSensitiveDetector.hh"
#include "DDG4/Geant4Hits.h"
#include "G4THitsCollection.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;
namespace det {
class SimpleTrackerSD : public G4VSensitiveDetector
{

  public:
      SimpleTrackerSD(std::string aDetectorName, std::string aReadoutName);
      ~SimpleTrackerSD();

      void Initialize(G4HCofThisEvent*HCE);
      G4bool ProcessHits(G4Step*aStep, G4TouchableHistory*ROhist);
      void EndOfEvent(G4HCofThisEvent*HCE);
      void clear();
      void DrawAll();
      void PrintAll();

  private:
      G4THitsCollection<DD4hep::Simulation::Geant4Hit> *trackerCollection;

  public:
};
}

#endif /* DETSENSITIVE_SIMPLETRACKERSD_H */