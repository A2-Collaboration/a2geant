// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolHadronElasticPhysics
// Hadron elastic scattering process with option to have polarised
// nucleon scattering
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA new "Pol" classes derived from 4.9.3 G4Had... classes
//  7/09/11 JRMA use framework of G4WHadronElasticProcess.hh
//
// CHIPS for sampling scattering for p and n
// Glauber model for samplimg of high energy pi+- (E > 1GeV)
// LHEP sampling model for the other particle
// BBG cross sections for p, n and pi+- 
// LHEP cross sections for other particles

#ifndef PolHadronElasticPhysicsN_h
#define PolHadronElasticPhysicsN_h 1

#include "G4HadronElasticPhysics.hh"

class PolWHadronElasticProcess;
class PolHadronElasticPhysicsN : public G4HadronElasticPhysics
{
public: 
  PolHadronElasticPhysicsN(G4int ver = 0); 
  // obsolete
  PolHadronElasticPhysicsN(const G4String& name , 
			 G4int ver = 0, G4bool hp = false, 
			 const G4String& type="");

  virtual ~PolHadronElasticPhysicsN();

  // This method will be invoked in the Construct() method.
  // each physics process will be instantiated and
  // registered to the process manager of each particle type 
  virtual void ConstructProcess();
protected:
  PolWHadronElasticProcess* nElPolProc;  // for neutrons
  PolWHadronElasticProcess* pElPolProc;  // for protons
public:
  PolWHadronElasticProcess* GetnElPolProc(){ return nElPolProc; }
  PolWHadronElasticProcess* GetpElPolProc(){ return pElPolProc; }
};


#endif








