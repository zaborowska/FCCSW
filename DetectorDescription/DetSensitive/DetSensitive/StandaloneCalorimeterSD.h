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
// $Id: B5HadCalorimeterSD.hh 76474 2013-11-11 10:36:34Z gcosmo $
//
/// \file B5HadCalorimeterSD.hh
/// \brief Definition of the B5HadCalorimeterSD class

#ifndef StandaloneCalorimeterSD_h
#define StandaloneCalorimeterSD_h 1

#include "G4VSensitiveDetector.hh"
#include "G4VGFlashSensitiveDetector.hh"

#include "StandaloneCalorimeterHit.h"

class G4GFlashSpot;
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

/// Hadron calorimeter sensitive detector

class StandaloneCalorimeterSD : public G4VSensitiveDetector, 
  public G4VGFlashSensitiveDetector
{
public:
  StandaloneCalorimeterSD(G4String name);
  StandaloneCalorimeterSD(G4String name,G4int aCellNoInAxis);
  virtual ~StandaloneCalorimeterSD();
  virtual void Initialize(G4HCofThisEvent*HCE);
  virtual G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
  virtual G4bool ProcessHits(G4GFlashSpot*aSpot,G4TouchableHistory*);
private:
  StandaloneCalorimeterHitsCollection* fHitsCollection;
  G4int fHCID;
  G4int fCellNo;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
