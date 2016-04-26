#include "DetSensitive/SimpleTrackerSD.h"

// FCCSW
#include "DetSensitive/SegmentationHelper.h"

// DD4hep
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

namespace det {
SimpleTrackerSD::SimpleTrackerSD(const std::string& aDetectorName,
  const std::string& aReadoutName,
  const DD4hep::Geometry::Segmentation& aSeg)
  : G4VSensitiveDetector(aDetectorName), m_seg(aSeg) {
  // add a name of the collection of hits
  collectionName.insert(aReadoutName);
  std::cout<<" Adding a collection with the name: "<<aReadoutName<<std::endl;
}

SimpleTrackerSD::~SimpleTrackerSD(){}

void SimpleTrackerSD::Initialize(G4HCofThisEvent* aHitsCollections)
{
  static int HCID = -1;
  // create a collection of hits and add it to G4HCofThisEvent
  // deleted in ~G4Event
  trackerCollection = new G4THitsCollection
    <DD4hep::Simulation::Geant4Hit>(SensitiveDetectorName,collectionName[0]);
  // get id for collection
  if(HCID<0)
    HCID = GetCollectionID(0);
  aHitsCollections->AddHitsCollection(HCID,trackerCollection);
}

bool SimpleTrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  // check if energy was deposited
  G4double edep = aStep->GetTotalEnergyDeposit();
  if(edep==0.)
    return false;

  // as in DD4hep::Simulation::Geant4GenericSD<Tracker>
  CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
  CLHEP::Hep3Vector postPos = aStep->GetPostStepPoint()->GetPosition();
  CLHEP::Hep3Vector medPos = 0.5*(prePos+postPos);
  DD4hep::Simulation::Position position(medPos.x(), medPos.y(), medPos.z());
  CLHEP::Hep3Vector direction = postPos - prePos;
  // create a hit and add it to collection
  const G4Track* track = aStep->GetTrack();
  // deleted in ~G4Event
  DD4hep::Simulation::Geant4TrackerHit* hit = new DD4hep::Simulation::Geant4TrackerHit(
    track->GetTrackID(), track->GetDefinition()->GetPDGEncoding(),edep, track->GetGlobalTime());
  if ( hit )  {
    hit->cellID  = segmentation::cellID(m_seg, *aStep);
    hit->energyDeposit = edep;
    hit->position = position;
    hit->momentum = direction;
    trackerCollection->insert(hit);
    return true;
  }
  return false;
}
}
