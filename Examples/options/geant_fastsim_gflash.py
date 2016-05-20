from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, FCCDataSvc

podioevent = FCCDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="/afs/cern.ch/exp/fcc/sw/0.6/testsamples/example_MyPythia.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detector='file:Test/TestDD4hep/compact/Box_simpleCaloSD.xml',
                    OutputLevel = DEBUG)

from Configurables import G4SimSvc, G4FastSimPhysicsList, G4FastSimActions, G4ParticleSmearSimple
smeartool = G4ParticleSmearSimple("Smear", sigma = 0.15)
actionstool = G4FastSimActions("Actions", smearing=smeartool)
physicslisttool = G4FastSimPhysicsList("Physics", fullphysics="G4FtfpBert")
geantservice = G4SimSvc("G4SimSvc", detector='G4DD4hepDetector', physicslist="G4FtfpBert", actions=actionstool)

from Configurables import G4SimAlg, G4SaveSmearedParticles, G4SaveCalHits
saveparticlestool = G4SaveSmearedParticles("G4SaveSmearedParticles")
saveparticlestool.DataOutputs.particles.Path = "smearedParticles"
saveparticlestool.DataOutputs.particlesMCparticles.Path = "particleMCparticleAssociation"
saveecaltool = G4SaveCalHits("G4SaveECalHits", caloType = "ECal")
saveecaltool.DataOutputs.caloClusters.Path = "caloClusters"
saveecaltool.DataOutputs.caloHits.Path = "caloHits"
geantsim = G4SimAlg("G4SimAlg", outputs = ["G4SaveSmearedParticles/G4SaveSmearedParticles",
                                           "G4SaveCalHits/G4SaveECalHits"])
geantsim.DataInputs.genParticles.Path="allGenParticles"


from Configurables import PodioWrite, PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg = [reader, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [podioevent, geoservice, geantservice], # order! geo needed by geant
                OutputLevel=DEBUG
 )