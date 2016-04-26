#include "DetSensitive/GFlashCalorimeterSD.h"

// FCCSW
#include "DetSensitive/SegmentationHelper.h"

// DD4hep
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

namespace det {
GFlashCalorimeterSD::GFlashCalorimeterSD(const std::string& aDetectorName,
  const std::string& aReadoutName,
  const DD4hep::Geometry::Segmentation& aSeg)
  : G4VSensitiveDetector(aDetectorName), G4VGFlashSensitiveDetector(), m_seg(aSeg) {
  // add a name of the collection of hits
  collectionName.insert(aReadoutName);
}

GFlashCalorimeterSD::~GFlashCalorimeterSD(){}

void GFlashCalorimeterSD::Initialize(G4HCofThisEvent* aHitsCollections)
{
  static int HCID = -1;
  // create a collection of hits and add it to G4HCofThisEvent
  // deleted in ~G4Event
  calorimeterCollection = new G4THitsCollection<DD4hep::Simulation::Geant4CalorimeterHit>(
    SensitiveDetectorName,collectionName[0]);
  // get id for collection
  if(HCID<0)
    HCID = GetCollectionID(0);
  aHitsCollections->AddHitsCollection(HCID,calorimeterCollection);
}

bool GFlashCalorimeterSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  // This method is called if full simulation is performed
  // check if energy was deposited
  G4double edep = aStep->GetTotalEnergyDeposit();
  if(edep==0.) return false;

  // as in DD4hep::Simulation::Geant4GenericSD<Calorimeter>
  CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
  CLHEP::Hep3Vector postPos = aStep->GetPostStepPoint()->GetPosition();
  CLHEP::Hep3Vector midPos = 0.5*(postPos + prePos);
  DD4hep::Simulation::Position pos(midPos.x(), midPos.y(), midPos.z());
  // check the cell ID
  uint64_t id = segmentation::cellID(m_seg, *aStep);
  DD4hep::Simulation::Geant4CalorimeterHit* hit = nullptr;
  for(int i=0; i<calorimeterCollection->entries(); i++) {
    hit = dynamic_cast<DD4hep::Simulation::Geant4CalorimeterHit*>(calorimeterCollection->GetHit(i));
    if(hit->cellID == id) {
      hit->energyDeposit += edep;
      return true;
    }
  }
  // create a new hit if it does not exist
  // deleted in ~G4Event
  hit = new DD4hep::Simulation::Geant4CalorimeterHit(pos);
  hit->cellID = id;
  hit->energyDeposit = edep;
  calorimeterCollection->insert(hit);
  return true;
}

bool GFlashCalorimeterSD::ProcessHits(G4GFlashSpot* aSpot, G4TouchableHistory*) {
  // This method will be called if gflash parametrisation is performed
  G4double edep = aSpot->GetEnergySpot()->GetEnergy();
  // check if energy was deposited
  if(edep==0.) return false;
  // check the cell ID
  uint64_t id = cellID(*aSpot);
  CLHEP::Hep3Vector geantPos = aSpot->GetEnergySpot()->GetPosition();
  DD4hep::Simulation::Position pos(geantPos.x(), geantPos.y(), geantPos.z());
  DD4hep::Simulation::Geant4CalorimeterHit* hit = nullptr;
  for(int i=0; i<calorimeterCollection->entries(); i++) {
    hit = dynamic_cast<DD4hep::Simulation::Geant4CalorimeterHit*>(calorimeterCollection->GetHit(i));
    if(hit->cellID == id) {
      hit->energyDeposit += edep;
      return true;
    }
  }
  // create a new hit if it does not exist
  // deleted in ~G4Event
  hit = new DD4hep::Simulation::Geant4CalorimeterHit(pos);
  hit->cellID = id;
  hit->energyDeposit = edep;
  calorimeterCollection->insert(hit);
  return true;
}

uint64_t GFlashCalorimeterSD::cellID(const G4GFlashSpot& aSpot) {
  DD4hep::Simulation::Geant4VolumeManager volMgr = DD4hep::Simulation::Geant4Mapping::instance().volumeManager();
  // TODO fix that
  DD4hep::Geometry::VolumeManager::VolumeID volID = 0;//volMgr.volumeID(aSpot->GetTouchableHandle());
  if (m_seg.isValid() )  {
    G4ThreeVector global = 0.5*(aSpot.GetEnergySpot()->GetPosition());
    G4ThreeVector local = aSpot.GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(global);
    DD4hep::Simulation::Position loc(local.x()*MM_2_CM, local.y()*MM_2_CM, local.z()*MM_2_CM);
    DD4hep::Simulation::Position glob(global.x()*MM_2_CM, global.y()*MM_2_CM, global.z()*MM_2_CM);
    DD4hep::Geometry::VolumeManager::VolumeID cID = m_seg.cellID(loc,glob,volID);
    return cID;
  }
  return 0;
}

}
