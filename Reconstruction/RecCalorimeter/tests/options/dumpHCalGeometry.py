from Gaudi.Configuration import *

from Configurables import ApplicationMgr

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'],
                    OutputLevel = INFO)

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc",
                        detector='SimG4DD4hepDetector',
                        physicslist="SimG4FtfpBert",
                        actions="SimG4FullSimActions")

export_fname = "HCal_barrel.gdml"
# check if file exists and delete it:
if os.path.isfile(export_fname):
    os.remove(export_fname)

from Configurables import GeoToGdmlDumpSvc
geodumpservice = GeoToGdmlDumpSvc("GeoDump", gdml=export_fname)

ApplicationMgr(
    TopAlg = [],
    EvtSel = 'NONE',
    EvtMax   = 1,
    ExtSvc = [ geoservice, geantservice, geodumpservice],
 )

