################################################################################
gaudi_subdir(TestSubAlgorithm v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg)

#find_package(Geant4)
#include(${Geant4_USE_FILE})

gaudi_install_headers(TestSubAlgorithm)

gaudi_add_module(TestSubAlgorithm
                 src/*.cpp
                 INCLUDE_DIRS
                 LINK_LIBRARIES GaudiAlgLib)
