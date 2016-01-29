from Gaudi.Configuration import *
from Configurables import ApplicationMgr, TestVectorAlgorithm, ToolWithVector

toolvector = ToolWithVector("ToolWithVector")
toolvector.string = ["aa", "bb"]
toolvector.int = [1,2,3]
toolvector.double = [10.3, 30.5, -23.6]
alg = TestVectorAlgorithm("alg", tool = toolvector)
alg.string = ["aa", "bb"]
alg.int = [1,2,3]
alg.double = [10.3, 30.5, -23.6]

ApplicationMgr( TopAlg = [alg],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [],
                OutputLevel=DEBUG
 )
