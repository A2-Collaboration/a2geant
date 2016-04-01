#! /bin/tcsh
#
# Get files from G4 distribution changed by JRMA so that specialist hadronic
# interaction classes can be more easily constructed by the user.
# 22/12/11 JRMA Update for G4.9.5 Claims neutron spin tracking fixed, new paths
#
set g4base = /home/dglazier/Work/cern/geant4
set gdist = geant4.10.01.p02
set Version = JRMA
set pFile = patch_$gdist.$Version.tar.gz
cp $pFile  $g4base/.
cd  $g4base
tar zxvf $pFile \
$gdist/source/physics_lists/constructors/hadron_elastic/include/G4HadronElasticPhysics.hh \
$gdist/source/processes/hadronic/processes/include/G4HadronElasticProcess.hh \
$gdist/source/processes/hadronic/management/include/G4HadronicProcess.hh \
$gdist/source/processes/hadronic/management/include/G4HadronInelasticProcess.hh
echo ""
echo Modified source files from file $pFile
cd
