from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, ToolWithOutput, ToolWithTool, TestToolAlgorithm, AlbersOutput, TestMultipleToolInstancesAlgorithm

albersevent = FCCDataSvc("EventDataSvc")

alg =  TestMultipleToolInstancesAlgorithm("alg")

out = AlbersOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg = [alg, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [albersevent],
                OutputLevel=DEBUG
 )
