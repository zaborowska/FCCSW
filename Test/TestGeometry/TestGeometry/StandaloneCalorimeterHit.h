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
// $Id: B5HadCalorimeterHit.hh 76474 2013-11-11 10:36:34Z gcosmo $
//
/// \file B5HadCalorimeterHit.hh
/// \brief Definition of the B5HadCalorimeterHit class

#ifndef StandaloneCalorimeterHit_h
#define StandaloneCalorimeterHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"

class G4AttDef;
class G4AttValue;

/// Hadron Calorimeter hit
///
/// It records:
/// - the cell x ID and y ID and z ID
/// - the energy deposit 
/// - the cell position and rotation

class StandaloneCalorimeterHit : public G4VHit
{
public:
    StandaloneCalorimeterHit();
    StandaloneCalorimeterHit(G4int iX,G4int iY,G4int iZ);
    StandaloneCalorimeterHit(const StandaloneCalorimeterHit &right);
    virtual ~StandaloneCalorimeterHit();

    const StandaloneCalorimeterHit& operator=(const StandaloneCalorimeterHit &right);
    int operator==(const StandaloneCalorimeterHit &right) const;
    
    inline void *operator new(size_t);
    inline void operator delete(void *aHit);
    
    virtual void Draw();
    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
    virtual std::vector<G4AttValue>* CreateAttValues() const;
    virtual void Print();
    
    void SetXid(G4int z) { fxID = z; }
    G4int GetXid() const { return fxID; }

    void SetYid(G4int z) { fyID = z; }
    G4int GetYid() const { return fyID; }
  
    void SetZid(G4int z) { fzID = z; }
    G4int GetZid() const { return fzID; }

    void SetEdep(G4double de) { fEdep = de; }
    void AddEdep(G4double de) { fEdep += de; }
    G4double GetEdep() const { return fEdep; }

    void SetPos(G4ThreeVector xyz) { fPos = xyz; }
    G4ThreeVector GetPos() const { return fPos; }

    void SetRot(G4RotationMatrix rmat) { fRot = rmat; }
    G4RotationMatrix GetRot() const { return fRot; }
    
private:
    G4int fxID;
    G4int fyID;
    G4int fzID;
    G4double fEdep;
    G4ThreeVector fPos;
    G4RotationMatrix fRot;
};

typedef G4THitsCollection<StandaloneCalorimeterHit> StandaloneCalorimeterHitsCollection;

extern G4ThreadLocal G4Allocator<StandaloneCalorimeterHit>* StandaloneCalorimeterHitAllocator;

inline void* StandaloneCalorimeterHit::operator new(size_t)
{
    if (!StandaloneCalorimeterHitAllocator)
        StandaloneCalorimeterHitAllocator = new G4Allocator<StandaloneCalorimeterHit>;
    return (void*)StandaloneCalorimeterHitAllocator->MallocSingle();
}

inline void StandaloneCalorimeterHit::operator delete(void* aHit)
{
    StandaloneCalorimeterHitAllocator->FreeSingle((StandaloneCalorimeterHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
