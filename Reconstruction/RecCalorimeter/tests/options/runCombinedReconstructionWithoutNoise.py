from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc", input="output_ecalSim_e50GeV_eta0_10events.root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=["ECalHits", "ECalPositionedHits"], OutputLevel=DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhECalSimple/compact/FCChh_ECalBarrel_Mockup.xml',
                                          'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'],
                    OutputLevel = INFO)

# common ECAL specific information
# readout name
ecalReadoutName = "ECalHitsPhiEta"
# active material identifier name
ecalIdentifierName = "active_layer"
# active material volume name
ecalVolumeName = "LAr_sensitive"
# ECAL bitfield names & values
ecalFieldNames=["system","ECAL_Cryo","bath","EM_barrel"]
ecalFieldValues=[5,1,1,1]
# common HCAL specific information
# readout name
hcalReadoutName = "BarHCal_Readout"
# active material identifier name
hcalIdentifierName = ["layer", "wedge","row","sub_module"]
 # active material volume name
hcalVolumeName = ["layer", "wedge","subWedge", "module_component"]
# ECAL bitfield names & values
hcalFieldNames=["system"]

#Configure tools for calo reconstruction
from Configurables import CalibrateCaloHitsTool
calibcells = CalibrateCaloHitsTool("CalibrateCaloHitsTool", invSamplingFraction="5.4")

from Configurables import CreateCaloCells
createEcells = CreateCaloCells("CreateECaloCells",
                              doCellCalibration=True,
                              calibTool=calibcells,
                              addCellNoise=False, filterCellNoise=False,
                              OutputLevel=DEBUG)
createEcells.DataInputs.hits.Path="ECalHits"
createEcells.DataOutputs.cells.Path="ECalCells"

createHcells = CreateCaloCells("CreateHCaloCells",
                              doCellCalibration = False,
                              addCellNoise = False, filterCellNoise = False,
                              OutputLevel = DEBUG)
createHcells.DataInputs.hits.Path="HCalHits"
createHcells.DataOutputs.cells.Path="HCalCells"

# additionally for HCal
from Configurables import CreateMidVolPositions
positions = CreateMidVolPositions("positions", readoutName = hcalReadoutName, OutputLevel = INFO)
positions.DataInputs.caloCells.Path = "HCalCells"
positions.DataOutputs.caloPositionedHits.Path = "HCalPositions"

from Configurables import RedoSegmentation
resegment = RedoSegmentation("ReSegmentation",
                             # old bitfield (readout)
                             oldReadoutName = hcalReadoutName,
                             # # specify which fields are going to be altered (deleted/rewritten)
                             oldSegmentationIds = [],
                             # new bitfield (readout), with new segmentation
                             newReadoutName = hcalReadoutName + "_phieta",
                             debugPrint = 10,
                             OutputLevel = DEBUG)
# clusters are needed, with deposit position and cellID in bits
resegment.DataInputs.inhits.Path = "HCalPositions"
resegment.DataOutputs.outhits.Path = "newHCalCells"

positions2 = CreateMidVolPositions("positions2", readoutName = hcalReadoutName + "_phieta", OutputLevel = INFO)
positions2.DataInputs.caloCells.Path = "newHCalCells"
positions2.DataOutputs.caloPositionedHits.Path = "newHCalPositions"
#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow, CombinedCaloTowerTool
from GaudiKernel.PhysicalConstants import pi
towers = CombinedCaloTowerTool("towers",
                             deltaEtaTower = 0.01, deltaPhiTower = 2*pi/629.,
                             ecalReadoutName = ecalReadoutName,
                             hcalReadoutName = hcalReadoutName + "_phieta",
                             OutputLevel=DEBUG)
towers.DataInputs.ecalCells.Path="ECalCells"
towers.DataInputs.hcalCells.Path="newHCalCells"
createclusters = CreateCaloClustersSlidingWindow("CreateCaloClusters",
                                                 towerTool = towers,
                                                 nEtaWindow = 5, nPhiWindow = 15,
                                                 nEtaPosition = 3, nPhiPosition = 3,
                                                 nEtaDuplicates = 5, nPhiDuplicates = 15,
                                                 nEtaFinal = 5, nPhiFinal = 15,
                                                 energyThreshold = 7,
                                                 OutputLevel = DEBUG)
createclusters.DataOutputs.clusters.Path="caloClusters"

out = PodioOutput("out", filename="output_ecalReco_noNoise_test.root",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
createclusters.AuditExecute = True
createcells.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              createcells,
              createclusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 10,
    ExtSvc = [podioevent, geoservice],
 )

