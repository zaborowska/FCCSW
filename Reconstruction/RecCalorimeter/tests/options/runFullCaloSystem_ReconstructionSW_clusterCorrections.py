# Setup
# Names of cells collections
ecalBarrelCellsName = "ECalBarrelCells"
ecalEndcapCellsName = "ECalEndcapCells"
ecalFwdCellsName = "ECalFwdCells"
hcalBarrelCellsName = "newHCalCells"
hcalExtBarrelCellsName = "newExtHCalCells"
hcalEndcapCellsName = "HCalEndcapCells"
hcalFwdCellsName = "HCalFwdCells"
# Readouts
ecalBarrelReadoutName = "ECalBarrelPhiEta"
ecalEndcapReadoutName = "EMECPhiEtaReco"
ecalFwdReadoutName = "EMFwdPhiEta"
hcalBarrelReadoutName = "BarHCal_Readout_phieta"
hcalExtBarrelReadoutName = "ExtBarHCal_Readout_phieta"
hcalEndcapReadoutName = "HECPhiEta"
hcalFwdReadoutName = "HFwdPhiEta"
# Number of events
num_events = 10

from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent = FCCDataSvc("EventDataSvc", input="output_fullCalo_SimAndDigi_e50GeV_"+str(num_events)+"events.root")
# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader",
                        collections = [ecalBarrelCellsName, ecalEndcapCellsName, ecalFwdCellsName,
                                       hcalBarrelCellsName, hcalExtBarrelCellsName, hcalEndcapCellsName, hcalFwdCellsName])

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           # 'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
                                           'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                           # 'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml',
                                           # 'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalExtendedBarrel_TileCal.xml',
                                           # 'file:Detector/DetFCChhCalDiscs/compact/Endcaps_coneCryo.xml',
                                           # 'file:Detector/DetFCChhCalDiscs/compact/Forward_coneCryo.xml'
],
                    OutputLevel = WARNING)


ecalBarrelNoisePath = "/eos/project/f/fccsw-web/testsamples/elecNoise_ecalBarrel_50Ohm_traces2_2shieldWidth.root"
ecalEndcapNoisePath = "/eos/project/f/fccsw-web/testsamples/elecNoise_emec_50Ohm_2shieldWidth_6layers.root"
ecalBarrelNoiseHistName = "h_elecNoise_fcc_"
ecalEndcapNoiseHistName = "h_elecNoise_fcc_"

# add noise, create all existing cells in detector
from Configurables import NoiseCaloCellsFromFileTool, TubeLayerPhiEtaCaloTool,CreateCaloCells
noiseBarrel = NoiseCaloCellsFromFileTool("NoiseBarrel",
                                         readoutName = ecalBarrelReadoutName,
                                         noiseFileName = ecalBarrelNoisePath,
                                         elecNoiseHistoName = ecalBarrelNoiseHistName,
                                         activeFieldName = "layer",
                                         addPileup = False,
                                         numRadialLayers = 8,
                                         noiseCells = "ECalBarrelNoiseOnlyCells")
barrelGeometry = TubeLayerPhiEtaCaloTool("EcalBarrelGeo",
                                         readoutName = ecalBarrelReadoutName,
                                         activeVolumeName = "LAr_sensitive",
                                         activeFieldName = "layer",
                                         fieldNames = ["system"],
                                         fieldValues = [5],
                                         activeVolumesNumber = 8)
createEcalBarrelCells = CreateCaloCells("CreateECalBarrelCells",
                                        geometryTool = barrelGeometry,
                                        doCellCalibration=False, # already calibrated
                                        addCellNoise=True, filterCellNoise=False,
                                        noiseTool = noiseBarrel,
                                        hits=ecalBarrelCellsName,
                                        cells=ecalBarrelCellsName+"Noise",
                                        OutputLevel=DEBUG)

# # add noise, create all existing cells in detector
# # currently only positive side!
# noiseEndcap = NoiseCaloCellsFromFileTool("NoiseEndcap",
#                                                  readoutName = ecalEndcapReadoutName,
#                                                  noiseFileName = ecalEndcapNoisePath,
#                                                  elecNoiseHistoName = ecalEndcapNoiseHistName,
#                                                  activeFieldName = "layer",
#                                                  addPileup = False,
#                                                  numRadialLayers = 6)
# endcapGeometry = TubeLayerPhiEtaCaloTool("EcalEndcapGeo",
#                                                  readoutName = ecalEndcapReadoutName,
#                                                  activeVolumeName = "layerEnvelope",
#                                                  activeFieldName = "layer",
#                                                  activeVolumesNumber = 6,
#                                                  fieldNames = ["system"],
#                                                  fieldValues = [6])
# createEcalEndcapCells = CreateCaloCells("CreateECalEndcapCells",
#                                                 geometryTool = endcapGeometry,
#                                                 doCellCalibration=False, # already calibrated
#                                                 addCellNoise=True, filterCellNoise=False,
#                                                 noiseTool = noiseEndcap,
#                                                 hits=ecalEndcapCellsName,
#                                                 cells=ecalEndcapCellsName+"Noise")

#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow, CaloTowerTool
from GaudiKernel.PhysicalConstants import pi

from Configurables import CreateEmptyCaloCellsCollection
createemptycells = CreateEmptyCaloCellsCollection("CreateEmptyCaloCells")
createemptycells.cells.Path = "emptyCaloCells"

towers = CaloTowerTool("towers",
                               deltaEtaTower = 0.01, deltaPhiTower = 2*pi/704.,
                               ecalBarrelReadoutName = ecalBarrelReadoutName,
                               # ecalEndcapReadoutName = ecalEndcapReadoutName,
                               # ecalFwdReadoutName = ecalFwdReadoutName,
                               # hcalBarrelReadoutName = hcalBarrelReadoutName,
                               # hcalExtBarrelReadoutName = hcalExtBarrelReadoutName,
                               # hcalEndcapReadoutName = hcalEndcapReadoutName,
                               # hcalFwdReadoutName = hcalFwdReadoutName,
                               OutputLevel = DEBUG)
towers.ecalBarrelCells.Path = ecalBarrelCellsName + "Noise"
# towers.ecalEndcapCells.Path = ecalEndcapCellsName + "Noise"
# towers.ecalFwdCells.Path = ecalFwdCellsName
# towers.hcalBarrelCells.Path = hcalBarrelCellsName
# towers.hcalExtBarrelCells.Path = hcalExtBarrelCellsName
# towers.hcalEndcapCells.Path = hcalEndcapCellsName
# towers.hcalFwdCells.Path = hcalFwdCellsName
towers.ecalEndcapCells.Path = "emptyCaloCells"
towers.ecalFwdCells.Path = "emptyCaloCells"
towers.hcalBarrelCells.Path = "emptyCaloCells"
towers.hcalExtBarrelCells.Path = "emptyCaloCells"
towers.hcalEndcapCells.Path = "emptyCaloCells"
towers.hcalFwdCells.Path = "emptyCaloCells"

# Cluster variables
windE = 9
windP = 17
posE = 5
posP = 11
dupE = 7
dupP = 13
finE = 9
finP = 17
threshold = 12

createClusters = CreateCaloClustersSlidingWindow("CreateClusters",
                                                 towerTool = towers,
                                                 nEtaWindow = windE, nPhiWindow = windP,
                                                 nEtaPosition = posE, nPhiPosition = posP,
                                                 nEtaDuplicates = dupE, nPhiDuplicates = dupP,
                                                 nEtaFinal = finE, nPhiFinal = finP,
                                                 energyThreshold = threshold, OutputLevel=DEBUG)
createClusters.clusters.Path = "CaloClusters"



from Configurables import CorrectCluster, NoiseCorrectionToCellsTool
cellPileupNoise = NoiseCorrectionToCellsTool("noise", OutputLevel = DEBUG)
cellPileupNoise.ecalBarrelNoiseCells.Path="ECalBarrelNoiseOnlyCells"
correctClusters = CorrectCluster("CorrectCluster",
                                 clusters = "CaloClusters",
                                 doNoiseCorrection = True,
                                 noiseTool = cellPileupNoise,
                                 etaRecalcWeights = [],
                                 numLayers = [0],
                                 upstreamParamsPath = "fake.root",
                                 etaValues = [0,1],
                                 presamplerShiftP0 = [0.03397, 0.03397],
                                 presamplerShiftP1 = [6.347e-5, 6.347e-5],
                                 presamplerScaleP0 = [0.119, 0.119],
                                 presamplerScaleP1 = [0.07005, 0.07005],
                                 # etaValues = [0],
                                 # presamplerShiftP0 = [0.03397],
                                 # presamplerShiftP1 = [6.347e-5],
                                 # presamplerScaleP0 = [0.119],
                                 # presamplerScaleP1 = [0.07005],
                                 OutputLevel=DEBUG)

out = PodioOutput("out", filename="output_allCalo_reco_noise.root")
out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
createClusters.AuditExecute = True
out.AuditExecute = True

THistSvc().Output = ["rec DATAFILE='clusterCorrections_histograms.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=False
THistSvc().OutputLevel=INFO

ApplicationMgr(
    TopAlg = [podioinput,
              createemptycells,
              createEcalBarrelCells,
              # createEcalEndcapCells,
              createClusters,
              correctClusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = num_events,
    ExtSvc = [podioevent, geoservice])
