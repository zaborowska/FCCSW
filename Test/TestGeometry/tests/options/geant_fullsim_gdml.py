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

from Configurables import G4SimSvc, G4GdmlTestDetector
det = G4GdmlTestDetector("G4GdmlTestDetector", gdml = "../data/gflashDetector.xml")
geantservice = G4SimSvc("G4SimSvc", detector=det, physicslist='G4FtfpBert', actions='G4FullSimActions')

from Configurables import G4SimAlg, G4SaveTestCalHits
savecaltool = G4SaveTestCalHits("saveECalHits", caloType = "ECal", OutputLevel = DEBUG)
savecaltool.DataOutputs.caloClusters.Path = "caloClusters"
savecaltool.DataOutputs.caloHits.Path = "caloHits"
geantsim = G4SimAlg("G4SimAlg", outputs= ["G4SaveTestCalHits/saveECalHits",
                                          "InspectHitsCollectionsTool"])
geantsim.DataInputs.genParticles.Path="allGenParticles"

from Configurables import FCCDataSvc, PodioOutput
podiosvc = FCCDataSvc("EventDataSvc")
out = PodioOutput("out", filename="gdml_"+os.environ['ENERGY']+"GeV_full.root")
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [gen, hepmc_converter, hepmc_dump, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [podiosvc, ppservice, geantservice],
                OutputLevel=INFO
 )
