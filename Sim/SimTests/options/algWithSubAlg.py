from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, TestSubAlgorithm

albersevent = FCCDataSvc("EventDataSvc")


testsub = TestSubAlgorithm("myAlg")

ApplicationMgr( TopAlg = [testsub],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [albersevent], # order! geo needed by geant
                OutputLevel=DEBUG
 )
