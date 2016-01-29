from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, FCCDataSvc

albersevent = FCCDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import GeantSvc, GdmlDetector
det = GdmlDetector("GdmlDetector", gdml = "Sim/SimG4Common/gdml/example.xml")
geantservice = GeantSvc("GeantSvc", detector="GdmlDetector", physicslist="G4FTFP_BERT", actions="G4FullSimActions")
geantservice.addTool(det)

from Configurables import GeantFullSimAlg
geantsim = GeantFullSimAlg("GeantFullSimAlg")
geantsim.DataInputs.genParticles.Path="allGenParticles"
geantsim.DataOutputs.trackClusters.Path = "clusters"
geantsim.DataOutputs.trackHits.Path = "hits"
geantsim.DataOutputs.trackHitsClusters.Path = "hitClusterAssociation"

from Configurables import AlbersWrite, AlbersOutput
out = AlbersOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg = [reader, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [albersevent, geantservice], # order! geo needed by geant
                OutputLevel=DEBUG
 )
