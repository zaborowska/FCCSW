from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput
podioevent = FCCDataSvc("EventDataSvc", input="output_hcalSim_e50GeV_eta0_10events.root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=["HCalHits"], OutputLevel=DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'],
OutputLevel = DEBUG)

from Configurables import CreateCaloCells
createcells = CreateCaloCells("CreateCaloCells",
                              doCellCalibration = False,
                              addCellNoise = False, filterCellNoise = False,
                              OutputLevel = DEBUG)
createcells.DataInputs.hits.Path="HCalHits"
createcells.DataOutputs.cells.Path="HCalCells"

from Configurables import CreateMidVolPositions
positions = CreateMidVolPositions("positions", readoutName = "BarHCal_Readout", OutputLevel = INFO)
positions.DataInputs.caloCells.Path = "HCalCells"
positions.DataOutputs.caloPositionedHits.Path = "HCalPositions"

from Configurables import RedoSegmentation
resegment = RedoSegmentation("ReSegmentation",
                             # old bitfield (readout)
                             oldReadoutName = "BarHCal_Readout",
                             # # specify which fields are going to be altered (deleted/rewritten)
                             oldSegmentationIds = [],
                             # new bitfield (readout), with new segmentation
                             newReadoutName = "BarHCal_Readout_phieta",
                             debugPrint = 10,
                             OutputLevel = DEBUG)
# clusters are needed, with deposit position and cellID in bits
resegment.DataInputs.inhits.Path = "HCalPositions"
resegment.DataOutputs.outhits.Path = "newHCalCells"

positions2 = CreateMidVolPositions("positions2", readoutName = "BarHCal_Readout_phieta", OutputLevel = INFO)
positions2.DataInputs.caloCells.Path = "newHCalCells"
positions2.DataOutputs.caloPositionedHits.Path = "newHCalPositions"

# Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow, SingleCaloTowerTool
from GaudiKernel.PhysicalConstants import pi
towers = SingleCaloTowerTool("towers",
                             deltaEtaTower = 0.025, deltaPhiTower = 2*pi/129.,
                             readoutName = "BarHCal_Readout_phieta",
                             radiusForPosition = 2910,
                             OutputLevel = DEBUG)
towers.DataInputs.cells.Path="newHCalCells"
createclusters = CreateCaloClustersSlidingWindow("CreateCaloClusters",
                                                 towerTool = towers,
                                                 nEtaWindow = 9, nPhiWindow = 3,
                                                 nEtaPosition = 9, nPhiPosition = 3,
                                                 nEtaDuplicates = 9, nPhiDuplicates = 3,
                                                 nEtaFinal = 9, nPhiFinal = 3,
                                                 energyThreshold = 1,
                                                 saveCells = True,
                                                 OutputLevel = DEBUG)
createclusters.DataOutputs.clusters.Path="HCalClusters"

out = PodioOutput("out", filename="output_HCalReco_noNoise_e50GeV_eta0_10events.root",
                   OutputLevel = DEBUG)
out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
createcells.AuditExecute = True
positions.AuditExecute = True
resegment.AuditExecute = True
createclusters.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              createcells,
              positions,
              resegment,
              positions2,
              createclusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 5,
    ExtSvc = [podioevent, geoservice,],
    OutputLevel=DEBUG )

