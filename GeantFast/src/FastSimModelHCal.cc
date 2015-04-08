#include "FastSimModelHCal.hh"
#include "PrimaryParticleInformation.hh"
#include "Smearer.hh"
#include "Output.hh"

#include "G4Track.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"

#include "G4SystemOfUnits.hh"

FastSimModelHCal::FastSimModelHCal(G4String aModelName, G4Region* aEnvelope, DetectorParametrisation::Parametrisation aType)
   : G4VFastSimulationModel(aModelName, aEnvelope), fCalculateParametrisation(), fParametrisation(aType)
{}

FastSimModelHCal::FastSimModelHCal(G4String aModelName, G4Region* aEnvelope)
   : G4VFastSimulationModel(aModelName, aEnvelope), fCalculateParametrisation(), fParametrisation(DetectorParametrisation::eCMS)
{}

FastSimModelHCal::FastSimModelHCal(G4String aModelName)
   : G4VFastSimulationModel(aModelName), fCalculateParametrisation(), fParametrisation(DetectorParametrisation::eCMS)
{}

FastSimModelHCal::~FastSimModelHCal()
{}

G4bool FastSimModelHCal::IsApplicable(const G4ParticleDefinition& aParticleType)
{
   return (aParticleType.GetQuarkContent(1) +
           aParticleType.GetQuarkContent(2) +
           aParticleType.GetQuarkContent(3) +
           aParticleType.GetQuarkContent(4) +
           aParticleType.GetQuarkContent(5) +
           aParticleType.GetQuarkContent(6) );
}

G4bool FastSimModelHCal::ModelTrigger(const G4FastTrack& /*aFastTrack*/)
{
   return true;
}

void FastSimModelHCal::DoIt(const G4FastTrack& aFastTrack,
                               G4FastStep& aFastStep)
{
   // Kill the parameterised particle:
   aFastStep.KillPrimaryTrack();
   aFastStep.ProposePrimaryTrackPathLength(0.0);
   G4double Edep = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

   // here smear according to tracker resolution
   G4ThreeVector Pos = aFastTrack.GetPrimaryTrack()->GetPosition();

   if ( !aFastTrack.GetPrimaryTrack()->GetParentID() )
   {
      G4ThreeVector Porg = aFastTrack.GetPrimaryTrack()->GetMomentum();
      G4double res = fCalculateParametrisation->GetResolution(DetectorParametrisation::eHCAL, fParametrisation, Porg.mag());
      G4double eff = fCalculateParametrisation->GetEfficiency(DetectorParametrisation::eHCAL, fParametrisation, Porg.mag());
      G4double Esm;
      Esm = abs(Smearer::Instance()->SmearEnergy(aFastTrack.GetPrimaryTrack(), res));
      Output::Instance()->FillHistogram(2, (Esm/MeV) / (Edep/MeV) );

      ((PrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                        (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetHCalPosition(Pos);
      ((PrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                        (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetHCalEnergy(Esm);
      ((PrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                        (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetHCalResolution(res);
      ((PrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                        (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetHCalEfficiency(eff);
      aFastStep.ProposeTotalEnergyDeposited(Esm);
   }
}