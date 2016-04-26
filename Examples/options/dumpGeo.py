from Gaudi.Configuration import *

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:../DetectorDescription/DetSensitive/tests/compact/Box_simpleTrackerSD.xml'], OutputLevel = DEBUG)

from Configurables import G4SimSvc
geantservice = G4SimSvc("G4SimSvc",
                        detector='G4DD4hepDetector',
                        physicslist="G4FtfpBert",
                        actions="G4FullSimActions")

from Configurables import GeoToGdmlDumpSvc
geodumpservice = GeoToGdmlDumpSvc("GeoDump", gdml="TestBox.gdml")

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [geoservice, geantservice, geodumpservice],
                OutputLevel=DEBUG
 )
