from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, ToolWithOutput, ToolWithTool, TestToolAlgorithm, AlbersOutput, TestMultipleToolInstancesAlgorithm, ToolWithMember

albersevent = FCCDataSvc("EventDataSvc")


#tool1 = ToolWithMember("tool1")
#tool2 = ToolWithMember("tool2")
alg =  TestMultipleToolInstancesAlgorithm("alg")#, tool1=tool1, tool2=tool2)

out = AlbersOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg = [alg, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [albersevent],
                OutputLevel=DEBUG
 )
