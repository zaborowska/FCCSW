from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc", input="output_ecalSim_e50GeV_eta0_10events.root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=["ECalHits", "ECalPositionedHits"], OutputLevel=DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhECalSimple/compact/FCChh_ECalBarrel_Mockup.xml'],
                    OutputLevel = INFO)

from Configurables import MergeLayers
merge = MergeLayers("mergeLayers",
                   # take the bitfield description from the geometry service
                   readout ="ECalHitsPhiEta",
                   # cells in which field should be merged
                   identifier = "active_layer",
                   # how many cells to merge
                   # merge first 20 into new cell (id=0), next 50 into second cell (id=1), ...
                   merge = [22,83,11],
                   volumeName = "LAr_sensitive",
                   OutputLevel = INFO)
merge.DataInputs.inhits.Path = "ECalHits"
merge.DataOutputs.outhits.Path = "newECalHits"

#Configure tools for calo reconstruction
#Create calo cells
from Configurables import CalibrateCaloHitsTool
calibcells = CalibrateCaloHitsTool("CalibrateCaloHitsTool",invSamplingFraction="5.4")

from Configurables import CreateCaloCells
createcells = CreateCaloCells("CreateCaloCells",
                              calibTool=calibcells, doCellCalibration=True,
                              addCellNoise=False,filterCellNoise=False,
                              readoutName="ECalHitsPhiEta",
                              fieldNames=["system","ECAL_Cryo","bath","EM_barrel"],
                              fieldValues=[5,1,1,1],
                              OutputLevel=INFO)
createcells.DataInputs.hits.Path="newECalHits"
createcells.DataOutputs.cells.Path="caloCells"

#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow
from GaudiKernel.PhysicalConstants import pi
createclusters = CreateCaloClustersSlidingWindow("CreateCaloClusters",
                                                 readoutName="ECalHitsPhiEta",
                                                 fieldNames=["system","ECAL_Cryo","bath","EM_barrel"],
                                                 fieldValues=[5,1,1,1],
                                                 deltaEtaTower=0.01, deltaPhiTower=2*pi/629.,
                                                 nEtaWindow = 9, nPhiWindow = 9,
                                                 nEtaPosition =7, nPhiPosition = 7,
                                                 nEtaDuplicates = 9, nPhiDuplicates = 9,
                                                 energyThreshold = 3,
                                                 OutputLevel=DEBUG)
createclusters.DataInputs.cells.Path="caloCells"
createclusters.DataOutputs.clusters.Path="caloClusters"

from Configurables import CreatePositionedHit
positionhit = CreatePositionedHit("CreatePositionedHit", readoutName = "ECalHitsPhiEta",activeFieldName = "active_layer",activeVolumeName="LAr_sensitive", OutputLevel=INFO)
positionhit.DataInputs.caloCells.Path="caloCells"
positionhit.DataOutputs.caloPositionedHits.Path="caloCellsPositions"


out = PodioOutput("out", filename="new_output_ecalReco_10events.root",
                   OutputLevel=INFO)
out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
createclusters.AuditExecute = True
merge.AuditExecute = True
createcells.AuditExecute = True
positionhit.AuditExecute = True
out.AuditExecute = True


ApplicationMgr(
    TopAlg = [podioinput,
              merge,
              createcells,
              createclusters,
              positionhit,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 10,
    ExtSvc = [podioevent, geoservice],
 )

