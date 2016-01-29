from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, TestVectorAlgorithm, AlbersOutput

albersevent = FCCDataSvc("EventDataSvc")

alg = TestVectorAlgorithm("alg", tool = "ToolWithVector")
alg.string = ["aa", "bb"]
alg.tool.string = ["aa", "bb"]
alg.int = [1,2,3]
alg.tool.int = [1,2,3]
alg.double = [10.3, 30.5, -23.6]
alg.tool.double = [10.3, 30.5, -23.6]

out = AlbersOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg = [alg, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [albersevent],
                OutputLevel=DEBUG
 )
