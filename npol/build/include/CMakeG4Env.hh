#ifndef CMakeG4Env_hh
#define CMakeG4Env_hh 1
#include <stdlib.h>
#define SET_G4_ENV_BY_CMAKE() \
setenv("G4LEVELGAMMADATA","/home/dglazier/Work/cern/geant4/geant4.10.01_data/PhotonEvaporation3.1",0); \
setenv("G4NEUTRONXSDATA","/home/dglazier/Work/cern/geant4/geant4.10.01_data/G4NEUTRONXS1.4",0); \
setenv("G4LEDATA","/home/dglazier/Work/cern/geant4/geant4.10.01_data/G4EMLOW6.41",0); \
setenv("G4NEUTRONHPDATA","/home/dglazier/Work/cern/geant4/geant4.10.01_data/G4NDL4.5",0); \
setenv("G4RADIOACTIVEDATA","/home/dglazier/Work/cern/geant4/geant4.10.01_data/RadioactiveDecay4.2",0); \
setenv("G4ABLADATA","/home/dglazier/Work/cern/geant4/geant4.10.01_data/G4ABLA3.0",0); \
setenv("G4PIIDATA","/home/dglazier/Work/cern/geant4/geant4.10.01_data/G4PII1.3",0); \
setenv("G4SAIDXSDATA","/home/dglazier/Work/cern/geant4/geant4.10.01_data/G4SAIDDATA1.1",0); \
setenv("G4REALSURFACEDATA","/home/dglazier/Work/cern/geant4/geant4.10.01_data/RealSurface1.0",0); \

#endif