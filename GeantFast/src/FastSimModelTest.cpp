#include "FastSimModelTest.h"
#include <memory>

// Geant4
#include "G4PathFinder.hh"
#include "G4SystemOfUnits.hh"

FastSimModelTest::FastSimModelTest(G4String aModelName, G4Region* aEnvelope, std::string aSmearToolName)
   : G4VFastSimulationModel(aModelName, aEnvelope), m_msgSvc("MessageSvc","FastSimModelTest"),
     log(&(*m_msgSvc), "FastSimModelTest"), m_toolSvc("ToolSvc","ToolSvc")
{
   log << MSG::INFO << "Initialising _______________________" << endmsg;
   if( m_toolSvc->retrieveTool(aSmearToolName, m_smearTool).isFailure())
        throw GaudiException("Smearing tool "+aSmearToolName+" not found",
                             "FastSimModelTest", StatusCode::FAILURE);
}

FastSimModelTest::FastSimModelTest(G4String aModelName)
   : G4VFastSimulationModel(aModelName), m_msgSvc("MessageSvc","FastSimModelTest"),
     log(&(*m_msgSvc), "FastSimModelTest"), m_toolSvc("ToolSvc","ToolSvc")
{}

FastSimModelTest::~FastSimModelTest()
{}

G4bool FastSimModelTest::IsApplicable(const G4ParticleDefinition& aParticleType)
{
   return aParticleType.GetPDGCharge() != 0;
}

G4bool FastSimModelTest::ModelTrigger(const G4FastTrack& /*aFastTrack*/)
{
   return true;
}

void FastSimModelTest::DoIt(const G4FastTrack& aFastTrack,
                            G4FastStep& aFastStep)
{
// Calculate the position of the particle at the end of volume
   G4ThreeVector spin = aFastTrack.GetPrimaryTrack()->GetPolarization() ;
   G4FieldTrack theFieldTrack = G4FieldTrack( aFastTrack.GetPrimaryTrack()->GetPosition(),
                                              aFastTrack.GetPrimaryTrack()->GetMomentumDirection(),
                                              0.0,
                                              aFastTrack.GetPrimaryTrack()->GetKineticEnergy(),
                                              aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetDefinition()->GetPDGMass(),
                                              0.0,
                                              aFastTrack.GetPrimaryTrack()->GetGlobalTime(), // Lab.
                                              aFastTrack.GetPrimaryTrack()->GetProperTime(), // Part.
                                              &spin) ;
   G4double retSafety= -1.0;
   ELimited retStepLimited;
   G4FieldTrack endTrack('a');
   G4double currentMinimumStep= 10*m; // TODO change that to sth connected to particle momentum and geometry
   G4PathFinder* fPathFinder = G4PathFinder::GetInstance();
   fPathFinder->ComputeStep( theFieldTrack,
                             currentMinimumStep,
                             0,
                             aFastTrack.GetPrimaryTrack()->GetCurrentStepNumber(),
                             retSafety,
                             retStepLimited,
                             endTrack,
                             aFastTrack.GetPrimaryTrack()->GetVolume() );
   aFastStep.ProposePrimaryTrackFinalPosition( endTrack.GetPosition() );

// Smear particle's momentum according to the tracker resolution
   G4ThreeVector Porg = aFastTrack.GetPrimaryTrack()->GetMomentum();
   // G4ThreeVector Psm = TOOOL->ReturnMyValue();

   G4double Ekinorg = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();
   // aFastStep.ProposePrimaryTrackFinalKineticEnergyAndDirection(Ekinorg+(Psm-Porg), Psm.unit());

   log << MSG::INFO << "P org = "<<Porg<<"  _______________________" << endmsg;
        m_smearTool->smearMomentum(Porg);
        log << MSG::INFO << "P sme = "<<Porg<<"  _______________________" << endmsg;
}