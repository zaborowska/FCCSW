################################################################################
# Package: GaudiTests
################################################################################
gaudi_subdir(GaudiTests v1r0)

#this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg GaudiKernel FWCore DataObjects)

find_package(ROOT)
find_package(ALBERS)
find_package(FCCEDM)

gaudi_add_module(GaudiTests
		 src/*.cpp
		 INCLUDE_DIRS ROOT FWCore GaudiKernel DataObjects FCCEDM ALBERS
       LINK_LIBRARIES GaudiAlgLib FWCore DataObjects ROOT GaudiKernel FCCEDM ALBERS)


include(CTest)
gaudi_add_test(AlgToolWithTool
               FRAMEWORK options/algToolWithTool.py)

gaudi_add_test(ServiceToolWithSameTool
               FRAMEWORK options/svcToolWithTool.py)

gaudi_add_test(ServiceToolWithOtherTool
               FRAMEWORK options/svcToolWithOtherTool.py)

gaudi_add_test(ToolWithOutput
               FRAMEWORK options/toolWithOutput.py)

gaudi_add_test(AlgToolWithVector
               FRAMEWORK options/algToolWithVector.py)

gaudi_add_test(AlgToolWithManyProperties
               FRAMEWORK options/toolProperties.py)

gaudi_add_test(AlgorithmWithManyProperties
               FRAMEWORK options/algProperties.py)

gaudi_add_test(AlgWithSubAlg
               FRAMEWORK options/algWithSubAlg.py)

gaudi_add_test(AlgWithMultipleToolInstances
               FRAMEWORK options/algWithMultipleToolInstances.py)
