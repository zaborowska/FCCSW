from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, TestSubAlg

albersevent = FCCDataSvc("EventDataSvc")


testsub = TestSubAlg("testsubalgname")

ApplicationMgr( TopAlg = [testsub],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [albersevent], # order! geo needed by geant
                OutputLevel=DEBUG
 )
