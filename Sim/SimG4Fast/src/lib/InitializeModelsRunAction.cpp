#include "SimG4Fast/InitializeModelsRunAction.h"

// FCCSW
#include "SimG4Fast/FastSimModelTracker.h"

// Geant4
#include "G4RegionStore.hh"
#include "G4VFastSimulationModel.hh"
#include "G4TransportationManager.hh"
#include "GFlashHomoShowerParameterisation.hh"
#include "GFlashShowerModel.hh"
#include "GFlashHitMaker.hh"
#include "GFlashParticleBounds.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"

namespace sim {
InitializeModelsRunAction::InitializeModelsRunAction(const std::string& aSmearingToolName,
                                                     bool aModelTracker,
                                                     bool aModelECal,
                                                     bool aModelHCal) :
  G4UserRunAction(),
  m_msgSvc("MessageSvc","InitializeModelsRunAction"),
  m_log(&(*m_msgSvc),"InitializeModelsRunAction"),
  m_smearToolName(aSmearingToolName),
  m_modelTracker(aModelTracker),
  m_modelECal(aModelECal),
  m_modelHCal(aModelHCal) {}

InitializeModelsRunAction::~InitializeModelsRunAction() {}

void InitializeModelsRunAction::BeginOfRunAction(const G4Run* /*aRun*/) {
  G4LogicalVolume* world = (*G4TransportationManager::GetTransportationManager()->GetWorldsIterator())->GetLogicalVolume();
  for(int iter_region = 0; iter_region<world->GetNoDaughters(); ++iter_region) {
    if(m_modelTracker) {
      if(world->GetDaughter(iter_region)->GetName().find("Tracker") != std::string::npos) {
        /// all G4Region objects are deleted by the G4RegionStore
        m_g4regions.emplace_back(new G4Region(world->GetDaughter(iter_region)->GetLogicalVolume()->GetName()+"_fastsim"));
        m_g4regions.back()->AddRootLogicalVolume(world->GetDaughter(iter_region)->GetLogicalVolume());
        m_models.emplace_back(new FastSimModelTracker(m_g4regions.back()->GetName(),m_g4regions.back(),m_smearToolName));
        m_log<<MSG::INFO<<"Attaching a Fast Simulation Model to the region "<<m_g4regions.back()->GetName()<<endmsg;
      }
    }
    if(m_modelECal) {
      if(world->GetDaughter(iter_region)->GetName().find("ECal") != std::string::npos
         || world->GetDaughter(iter_region)->GetName().find("HCal") != std::string::npos
         || world->GetDaughter(iter_region)->GetName().find("EMCal") != std::string::npos) {
        /// all G4Region objects are deleted by the G4RegionStore
        m_g4regions.emplace_back(new G4Region(world->GetDaughter(iter_region)->GetLogicalVolume()->GetName()+"_fastsim"));
        m_g4regions.back()->AddRootLogicalVolume(world->GetDaughter(iter_region)->GetLogicalVolume());
        std::unique_ptr<GFlashShowerModel> model(new GFlashShowerModel(m_g4regions.back()->GetName(),m_g4regions.back()));
        // proper parametrisation with the material
        // TODO check if deleted
        GFlashHomoShowerParameterisation* fParameterisation = new GFlashHomoShowerParameterisation(
          world->GetDaughter(iter_region)->GetLogicalVolume()->GetMaterial());
        model->SetParameterisation(*fParameterisation);
        // Energy Cuts to kill particles:
        // TODO check if deleted
        GFlashParticleBounds* fParticleBounds = new GFlashParticleBounds();
        model->SetParticleBounds(*fParticleBounds);
        // Makes the EnergieSpots
        // TODO check if deleted
        GFlashHitMaker* fHitMaker = new GFlashHitMaker();
        model->SetHitMaker(*fHitMaker);
        m_models.push_back(std::move(model));
        m_log<<MSG::INFO<<"Attaching a Fast Simulation Model to the region "<<m_g4regions.back()->GetName()<<endmsg;
      }
    }
  }
  return;
}
}
