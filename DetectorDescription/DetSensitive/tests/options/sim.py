from Gaudi.Configuration import *

from Configurables import ParticleGunAlg, MomentumRangeParticleGun, Gaudi__ParticlePropertySvc
pgun = MomentumRangeParticleGun("PGun",
                                PdgCodes=[11], # electron
                                MomentumMin = os.environ['ENERGY'], # GeV
                                MomentumMax = os.environ['ENERGY'], # GeV
                                ThetaMin = 0., # rad
                                ThetaMax = 0., # rad
                                PhiMin = 0, # rad
                                PhiMax = 0) # rad
gen = ParticleGunAlg("ParticleGun", ParticleGunTool=pgun)
gen.DataOutputs.hepmc.Path = "hepmc"
ppservice = Gaudi__ParticlePropertySvc("ParticlePropertySvc", ParticlePropertiesFile="../../../Generation/data/ParticleTable.txt")

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import HepMCDumper
hepmc_dump = HepMCDumper("hepmc")
hepmc_dump.DataInputs.hepmc.Path="hepmc"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:compact/Box_gflashCaloSD.xml'], OutputLevel = DEBUG)

if os.environ['SIMTYPE']=='full':
    from Configurables import G4SimSvc
    geantservice = G4SimSvc("G4SimSvc", detector='G4DD4hepDetector', physicslist='G4FtfpBert', actions='G4FullSimActions')
elif os.environ['SIMTYPE']=='gflash':
    from Configurables import G4SimSvc, G4FastSimPhysicsList, G4FastSimActions, G4ParticleSmearSimple
    smeartool = G4ParticleSmearSimple("Smear", sigma = 0.15)
    actionstool = G4FastSimActions("Actions", smearing=smeartool)
    physicslisttool = G4FastSimPhysicsList("Physics", fullphysics="G4FtfpBert")
    geantservice = G4SimSvc("G4SimSvc", detector='G4DD4hepDetector', physicslist=physicslisttool, actions=actionstool)

from Configurables import G4SimAlg, G4SaveCalHits
savecaltool = G4SaveCalHits("saveECalHits", caloType = "ECal")
savecaltool.DataOutputs.caloClusters.Path = "caloClusters"
savecaltool.DataOutputs.caloHits.Path = "caloHits"
geantsim = G4SimAlg("G4SimAlg", outputs= ["G4SaveCalHits/saveECalHits"])
geantsim.DataInputs.genParticles.Path="allGenParticles"

from Configurables import FCCDataSvc, PodioOutput
podiosvc = FCCDataSvc("EventDataSvc")
out = PodioOutput("out", filename="output_"+os.environ['ENERGY']+"GeV_"+os.environ['SIMTYPE']+".root")
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [gen, hepmc_converter, hepmc_dump, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 2,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podiosvc, ppservice, geoservice, geantservice],
                OutputLevel=INFO
 )
