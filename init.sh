#!/bin/sh -u
source /afs/cern.ch/lhcb/software/releases/LBSCRIPTS/LBSCRIPTS_v8r4p3/InstallArea/scripts/LbLogin.sh --cmtconfig x86_64-slc6-gcc49-opt

export DELPHES_DIR=/afs/cern.ch/exp/fcc/sw/0.5/Delphes-3.3.1-newPCMs/x86_64-slc6-gcc49-opt
export PYTHIA_DIR=/afs/cern.ch/sw/lcg/releases/LCG_80/MCGenerators/pythia8/212/x86_64-slc6-gcc49-opt/

export CMAKE_PREFIX_PATH=$FCCEDM:$ALBERS:$DELPHES_DIR:$CMAKE_PREFIX_PATH:$PYTHIA_DIR

# set up Pythia8 Index.xml
export PYTHIA8_XML=/afs/cern.ch/sw/lcg/releases/LCG_80/MCGenerators/pythia8/212/x86_64-slc6-gcc49-opt/share/Pythia8/xmldoc

# add Geant4 data files
source /afs/cern.ch/sw/lcg/external/geant4/10.1/setup_g4datasets.sh

# add DD4hep
export inithere=$PWD
cd  /build/azaborow/FCC/DD4hep/20152311/build
source bin/thisdd4hep.sh
cd $inithere

export CMTPROJECTPATH=/afs/cern.ch/exp/fcc/sw/0.5/
source /afs/cern.ch/sw/lcg/contrib/gcc/4.9.3/x86_64-slc6/setup.sh
