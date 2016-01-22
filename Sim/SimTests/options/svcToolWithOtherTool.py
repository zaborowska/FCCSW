from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, ToolWithOtherTool, OtherTool, TestToolService, AlbersOutput

albersevent = FCCDataSvc("EventDataSvc")

toolA = OtherTool("ToolA")
toolB = ToolWithOtherTool("ToolB", tool=toolA)
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
