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
// $Id: B5HadCalorimeterHit.cc 76474 2013-11-11 10:36:34Z gcosmo $
//
/// \file B5HadCalorimeterHit.cc
/// \brief Implementation of the B5HadCalorimeterHit class

#include "DetSensitive/StandaloneCalorimeterHit.h"

#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal G4Allocator<StandaloneCalorimeterHit>* StandaloneCalorimeterHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StandaloneCalorimeterHit::StandaloneCalorimeterHit()
: G4VHit(), fxID(-1), fyID(-1), fzID(-1), fEdep(0.), fPos(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StandaloneCalorimeterHit::StandaloneCalorimeterHit(G4int iX,G4int iY,G4int iZ)
: G4VHit(), fxID(iX), fyID(iY), fzID(iZ), fEdep(0.), fPos(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StandaloneCalorimeterHit::~StandaloneCalorimeterHit()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StandaloneCalorimeterHit::StandaloneCalorimeterHit(const StandaloneCalorimeterHit &right)
: G4VHit() {
    fxID = right.fxID;
    fyID = right.fyID;
    fzID = right.fzID;
    fEdep = right.fEdep;
    fPos = right.fPos;
    fRot = right.fRot;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const StandaloneCalorimeterHit& StandaloneCalorimeterHit::operator=(const
                                    StandaloneCalorimeterHit &right)
{
    fxID = right.fxID;
    fyID = right.fyID;
    fzID = right.fzID;
    fEdep = right.fEdep;
    fPos = right.fPos;
    fRot = right.fRot;
    return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int StandaloneCalorimeterHit::operator==(const StandaloneCalorimeterHit &right) const
{
    return (fxID==right.fxID&&fyID==right.fyID&&fzID==right.fzID);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StandaloneCalorimeterHit::Draw()
{
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if (pVVisManager&&(fEdep>0.))
    {
      G4Transform3D trans(fRot,fPos);
        std::cout<<"drawing point at: "<<fPos<<std::endl;
        G4VisAttributes attribs;
        G4Colour colour(1.,0.,0.);
        attribs.SetColour(colour);
        attribs.SetForceSolid(true);
        G4Box box("dummy",1.*cm/2.,1.*cm/2.,1*cm/2.);
        pVVisManager->Draw(box,attribs,trans);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const std::map<G4String,G4AttDef>* StandaloneCalorimeterHit::GetAttDefs() const
{
    G4bool isNew;
    std::map<G4String,G4AttDef>* store
    = G4AttDefStore::GetInstance("StandaloneCalorimeterHit",isNew);

    if (isNew) {
        (*store)["HitType"] 
          = G4AttDef("HitType","Hit Type","Physics","","G4String");
        
        (*store)["X"] 
          = G4AttDef("X","x ID","Physics","","G4int");
        
        (*store)["Y"] 
          = G4AttDef("Y","y ID","Physics","","G4int");
        
        (*store)["Z"] 
          = G4AttDef("Z","z ID","Physics","","G4int");
        
        (*store)["Energy"] 
          = G4AttDef("Energy","Energy Deposited","Physics","G4BestUnit",
                     "G4double");
        
        (*store)["Pos"] 
          = G4AttDef("Pos", "Position", "Physics","G4BestUnit",
                     "G4ThreeVector");
    }
    return store;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<G4AttValue>* StandaloneCalorimeterHit::CreateAttValues() const
{
    std::vector<G4AttValue>* values = new std::vector<G4AttValue>;
    
    values
      ->push_back(G4AttValue("HitType","HadCalorimeterHit",""));
    values
      ->push_back(G4AttValue("x",G4UIcommand::ConvertToString(fxID), ""));
    values
      ->push_back(G4AttValue("y",G4UIcommand::ConvertToString(fyID), ""));
    values
      ->push_back(G4AttValue("y",G4UIcommand::ConvertToString(fzID), ""));
    values
      ->push_back(G4AttValue("Energy",G4BestUnit(fEdep,"Energy"),""));
    values
      ->push_back(G4AttValue("Pos",G4BestUnit(fPos,"Length"),""));
    
    return values;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StandaloneCalorimeterHit::Print()
{
    G4cout << "  Cell[" << fxID << ", " << fyID << ", " << fzID << "] "
           << fEdep/MeV << " (MeV) " << fPos/cm << " cm"<<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
