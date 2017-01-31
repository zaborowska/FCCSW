from Gaudi.Configuration import *

from Configurables import HepMCReader
reader = HepMCReader("Reader", Filename="Test/TestGeometry/data/testHepMCrandom_calo.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import HepMCDumper
hepmc_dump = HepMCDumper("hepmc")
hepmc_dump.DataInputs.hepmc.Path="hepmc"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Test/TestGeometry/data/BarrelPhiZRowLayer_volumes.xml'])
readout = "ECalHits"

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist='SimG4TestPhysicsList', actions='SimG4FullSimActions', OutputLevel = VERBOSE)

from Configurables import SimG4Alg, SimG4SaveCalHits
savecaltool = SimG4SaveCalHits("saveCalHits", readoutNames = [readout])
savecaltool.DataOutputs.positionedCaloHits.Path = "positionedCaloHits"
savecaltool.DataOutputs.caloHits.Path = "caloHits"
geantsim = SimG4Alg("SimG4Alg", outputs= ["SimG4SaveCalHits/saveCalHits"], OutputLevel = VERBOSE)

from Configurables import CreateMidVolPositions
positions = CreateMidVolPositions("positions", readoutName = readout, OutputLevel = VERBOSE)
positions.DataInputs.caloCells.Path = "caloHits"
positions.DataOutputs.caloPositionedHits.Path = "caloPositions"

from Configurables import FCCDataSvc, PodioOutput
podiosvc = FCCDataSvc("EventDataSvc")
out = PodioOutput("out", filename="test_position_hits_volumes.root")
out.outputCommands = ["keep *"]

ApplicationMgr(
    TopAlg = [reader, hepmc_converter, hepmc_dump, geantsim, positions, out],
    EvtSel = 'NONE',
    EvtMax   = 10,
    ExtSvc = [geoservice, geantservice, podiosvc],
 )

