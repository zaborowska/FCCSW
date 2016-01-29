from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, ToolWithOutput, ToolWithTool, TestToolService, AlbersOutput

albersevent = FCCDataSvc("EventDataSvc")

toolA = ToolWithOutput("ToolA")
toolA.DataOutputs.testOutput.Path = "particlesA"
toolB = ToolWithTool("ToolB", tool=toolA)
svc = TestToolService("svc", tool=toolB);

out = AlbersOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg = [out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [albersevent, svc],
                OutputLevel=DEBUG
 )
