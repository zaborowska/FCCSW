//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B5HadCalorimeterSD.cc 76474 2013-11-11 10:36:34Z gcosmo $
//
/// \file B5HadCalorimeterSD.cc
/// \brief Implementation of the B5HadCalorimeterSD class

#include "DetSensitive/StandaloneCalorimeterSD.h"
#include "DetSensitive/StandaloneCalorimeterHit.h"

#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4VTouchable.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4GFlashSpot.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StandaloneCalorimeterSD::StandaloneCalorimeterSD(G4String name)
  : G4VSensitiveDetector(name),G4VGFlashSensitiveDetector(), fHitsCollection(0), fHCID(-1), fCellNo(1)
{
  collectionName.insert("ECalorimeterColl");
}

StandaloneCalorimeterSD::StandaloneCalorimeterSD(G4String name, G4int aCellNoInAxis)
  : G4VSensitiveDetector(name), fHitsCollection(0), fHCID(-1), fCellNo(aCellNoInAxis)
{
  collectionName.insert("ECalorimeterColl");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StandaloneCalorimeterSD::~StandaloneCalorimeterSD()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StandaloneCalorimeterSD::Initialize(G4HCofThisEvent* hce)
{
  fHitsCollection
    = new StandaloneCalorimeterHitsCollection(SensitiveDetectorName,collectionName[0]);
  if (fHCID<0)
  { fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection); }
  hce->AddHitsCollection(fHCID,fHitsCollection);

  // fill calorimeter hits with zero energy deposition
  for (G4int ix=0;ix<fCellNo;ix++)
    for (G4int iy=0;iy<fCellNo;iy++)
      for (G4int iz=0;iz<fCellNo;iz++)
      {
        StandaloneCalorimeterHit* hit = new StandaloneCalorimeterHit();
        fHitsCollection->insert(hit);
      }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool StandaloneCalorimeterSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  G4double edep = step->GetTotalEnergyDeposit();
  if (edep==0.) return true;

  G4TouchableHistory* touchable
    = (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());

  G4int yNo = touchable->GetCopyNumber(1);
  G4int xNo = touchable->GetCopyNumber(2);
  G4int zNo = touchable->GetCopyNumber(0);

  G4int hitID = fCellNo*fCellNo*xNo+fCellNo*yNo+zNo;
  StandaloneCalorimeterHit* hit = (*fHitsCollection)[hitID];
  G4StepPoint* preStepPoint = step->GetPreStepPoint(); 
  G4StepPoint* postStepPoint = step->GetPostStepPoint(); 

  if(hit->GetXid()<0)
  {
    /*
      std::cout<<"volume 4 "<<touchable->GetVolume(4)->GetName()<<"\t"
      <<touchable->GetVolume(4)->GetCopyNo()<<std::endl
      <<"volume 3 "<<touchable->GetVolume(3)->GetName()<<"\t"
      <<touchable->GetVolume(3)->GetCopyNo()<<std::endl
      <<"volume 2 "<<touchable->GetVolume(2)->GetName()<<"\t"
      <<touchable->GetVolume(2)->GetCopyNo()<<std::endl
      <<"volume 1 "<<touchable->GetVolume(1)->GetName()<<"\t"
      <<touchable->GetVolume(1)->GetCopyNo()<<std::endl
      <<"volume 0 "<<touchable->GetVolume(0)->GetName()<<"\t"
      <<touchable->GetVolume(0)->GetCopyNo()<<std::endl;
    */
//             <<"volume 3 "<<touchable->GetVolume(3)->GetCopyNo()<<G4endl
//          <<"volume 4 "<<touchable->GetVolume(4)->GetCopyNo()<<G4endl;
    //            <<"volume 5 "<<touchable->GetVolume(5)->GetCopyNo()<<G4endl;
    //G4cout<<"position in vol: "<<touchable->GetHistory()->GetTransform(
    //touchable->GetHistory()->GetDepth()-1).NetTranslation()<<G4endl;
// std::cout<<xNo<<"\t"/be
    //          <<yNo<<"\t"
    //          <<zNo<<"\t"
    //          <<edep<<std::endl;
    /*       std::cout<<"preStepPoint->GetPosition() = "<<preStepPoint->GetPosition()/CLHEP::cm<<" cm\n"
             <<"step->GetTotalEnergyDeposit() = "<< edep<<std::endl;
    */
    // std::cout<<postStepPoint->GetPosition().x()/CLHEP::cm<<"\t"
    //      <<postStepPoint->GetPosition().y()/CLHEP::cm<<"\t"
    //      <<postStepPoint->GetPosition().z()/CLHEP::cm<<"\t"
    //          <<edep<<std::endl;

    // check if it is first touch
//    if (hit->GetXid()<0)
//    {
    hit->SetXid(xNo);
    hit->SetYid(yNo);
    hit->SetZid(zNo);
    G4int depth = touchable->GetHistory()->GetDepth();
    G4AffineTransform transform = touchable->GetHistory()->GetTransform(depth);
    transform.Invert();
    hit->SetRot(transform.NetRotation());
    hit->SetPos(transform.NetTranslation());
    // hit->SetPos(preStepPoint->GetPosition());
    // G4cout<<"hit: "<<xNo<<" , "<<yNo<<", "<<zNo<<G4endl;
    /*    std::cout<<"cell-centre position: "<<hit->GetPos()/CLHEP::cm<<" cm"<<std::endl;
          std::cout<<"==========================================="<<std::endl;*/
  }
  // add energy deposition
  hit->AddEdep(edep);
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool StandaloneCalorimeterSD::ProcessHits(G4GFlashSpot* spot, G4TouchableHistory*)
{
  G4double edep=spot->GetEnergySpot()->GetEnergy();
  if (edep==0.) return true;

  G4VTouchable* touchable = (spot->GetTouchableHandle())();

  G4int yNo = touchable->GetCopyNumber(1);
  G4int xNo = touchable->GetCopyNumber(2);
  G4int zNo = touchable->GetCopyNumber(0);

  G4int hitID = fCellNo*fCellNo*xNo+fCellNo*yNo+zNo;
  StandaloneCalorimeterHit* hit = (*fHitsCollection)[hitID];
  G4ThreeVector position = spot->GetEnergySpot()->GetPosition();

  if(hit->GetXid()<0)
  {
    hit->SetXid(xNo);
    hit->SetYid(yNo);
    hit->SetZid(zNo);
    G4int depth = touchable->GetHistory()->GetDepth();
    G4AffineTransform transform = touchable->GetHistory()->GetTransform(depth);
    transform.Invert();
    hit->SetRot(transform.NetRotation());
    hit->SetPos(transform.NetTranslation());
  }
  // add energy deposition
  hit->AddEdep(edep);
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
