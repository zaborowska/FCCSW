from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, ToolWithOutput, ToolWithTool, TestToolAlgorithm, AlbersOutput

albersevent = FCCDataSvc("EventDataSvc")

toolA = ToolWithOutput("ToolA")
toolA.DataOutputs.testOutput.Path = "particlesA"
toolB = ToolWithTool("ToolB", tool=toolA)
alg = TestToolAlgorithm("alg", tool=toolB);

out = AlbersOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg = [alg, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [albersevent],
                OutputLevel=DEBUG
 )
