#include "GFlashCalorimeterSD.h"

// DD4hep
#include "DDG4/Geant4Hits.h"
#include "DDG4/Geant4Data.h"
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

// Geant4
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4GFlashSpot.hh"
#include "G4TouchableHistory.hh"

namespace det {
GFlashCalorimeterSD::GFlashCalorimeterSD(std::string aDetectorName,
  std::string aReadoutName,
  DD4hep::Geometry::Segmentation aSeg)
  :G4VSensitiveDetector(aDetectorName), G4VGFlashSensitiveDetector(), m_seg(aSeg) {
  // add a name of the collection of hits
  collectionName.insert(aReadoutName);
  std::cout<<" Adding a collection with the name: "<<aReadoutName<<std::endl;
}

GFlashCalorimeterSD::~GFlashCalorimeterSD(){;}

void GFlashCalorimeterSD::Initialize(G4HCofThisEvent* aHitsCollections)
{
  // create a collection of hits and add it to G4HCofThisEvent
  // get id for collection
  static int HCID = -1;
  calorimeterCollection = new G4THitsCollection<DD4hep::Simulation::Geant4CalorimeterHit>(
    SensitiveDetectorName,collectionName[0]);
  if(HCID<0)
    HCID = GetCollectionID(0);
  aHitsCollections->AddHitsCollection(HCID,calorimeterCollection);
}

bool GFlashCalorimeterSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  // This method is called if full simulation is performed
  G4double edep = aStep->GetTotalEnergyDeposit();
  if(edep==0.) return false;

  // as in DD4hep::Simulation::Geant4GenericSD<Calorimeter>
  CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
  CLHEP::Hep3Vector postPos = aStep->GetPostStepPoint()->GetPosition();
  CLHEP::Hep3Vector midPos = 0.5*(postPos + prePos);
  DD4hep::Simulation::Position pos(midPos.x(), midPos.y(), midPos.z());
  CLHEP::Hep3Vector direction = postPos - prePos;
  double hit_len = direction.perp();
  // create a hit and add it to collection
  const G4Track* track = aStep->GetTrack();
  uint64_t id = getCellID(aStep);
  DD4hep::Simulation::Geant4CalorimeterHit* hit, *hitMatch = nullptr;
  for(int i=0; i<calorimeterCollection->entries(); i++) {
    hit = dynamic_cast<DD4hep::Simulation::Geant4CalorimeterHit*>(calorimeterCollection->GetHit(i));
    if(hit->cellID == id) {
      hitMatch = hit;
      continue;
    }
  }
  // create a new hit if it does not exist
  if ( !hitMatch )  {
    hitMatch = new  DD4hep::Simulation::Geant4CalorimeterHit(pos);
    hitMatch->cellID  = id;
    calorimeterCollection->insert(hitMatch);
  }
  hitMatch->energyDeposit += edep;
  return true;
}

bool GFlashCalorimeterSD::ProcessHits(G4GFlashSpot* aSpot, G4TouchableHistory*) {
  // This method will be called if gflash parametrisation is performed
  G4double edep = aSpot->GetEnergySpot()->GetEnergy();
  if(edep==0.) return false;
  uint64_t id = getCellID(aSpot);
  CLHEP::Hep3Vector geantPos = aSpot->GetEnergySpot()->GetPosition();
  DD4hep::Simulation::Position pos(geantPos.x(), geantPos.y(), geantPos.z());
  DD4hep::Simulation::Geant4CalorimeterHit* hit, *hitMatch = nullptr;
  for(int i=0; i<calorimeterCollection->entries(); i++) {
    hit = dynamic_cast<DD4hep::Simulation::Geant4CalorimeterHit*>(calorimeterCollection->GetHit(i));
    if(hit->cellID == id) {
      hitMatch = hit;
      continue;
    }
  }
  if ( !hitMatch )  {
    hitMatch = new  DD4hep::Simulation::Geant4CalorimeterHit(pos);
    hitMatch->cellID  = id;
    calorimeterCollection->insert(hitMatch);
  }
  hitMatch->energyDeposit += edep;
  return true;
}

uint64_t GFlashCalorimeterSD::getCellID(G4Step* aStep) {
  DD4hep::Simulation::Geant4VolumeManager volMgr = DD4hep::Simulation::Geant4Mapping::instance().volumeManager();
  DD4hep::Geometry::VolumeManager::VolumeID volID = volMgr.volumeID(aStep->GetPreStepPoint()->GetTouchable());
  if (m_seg.isValid() )  {
    G4ThreeVector global = 0.5 * (  aStep->GetPreStepPoint()->GetPosition()+
      aStep->GetPostStepPoint()->GetPosition());
    G4ThreeVector local  = aStep->GetPreStepPoint()->GetTouchable()->GetHistory()->GetTopTransform().TransformPoint(global);
    DD4hep::Simulation::Position loc(local.x()*MM_2_CM, local.y()*MM_2_CM, local.z()*MM_2_CM);
    DD4hep::Simulation::Position glob(global.x()*MM_2_CM, global.y()*MM_2_CM, global.z()*MM_2_CM);
    DD4hep::Geometry::VolumeManager::VolumeID cID = m_seg.cellID(loc,glob,volID);
    return cID;
  }
  return 0;
}

uint64_t GFlashCalorimeterSD::getCellID(G4GFlashSpot* aSpot) {
  DD4hep::Simulation::Geant4VolumeManager volMgr = DD4hep::Simulation::Geant4Mapping::instance().volumeManager();
  DD4hep::Geometry::VolumeManager::VolumeID volID = 0;//volMgr.volumeID(aSpot->GetTouchableHandle());
  if (m_seg.isValid() )  {
    G4ThreeVector global = 0.5 * (  aSpot->GetEnergySpot()->GetPosition());
    G4ThreeVector local  = aSpot->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(global);
    DD4hep::Simulation::Position loc(local.x()*MM_2_CM, local.y()*MM_2_CM, local.z()*MM_2_CM);
    DD4hep::Simulation::Position glob(global.x()*MM_2_CM, global.y()*MM_2_CM, global.z()*MM_2_CM);
    DD4hep::Geometry::VolumeManager::VolumeID cID = m_seg.cellID(loc,glob,volID);
    return cID;
  }
  return 0;
}

}
