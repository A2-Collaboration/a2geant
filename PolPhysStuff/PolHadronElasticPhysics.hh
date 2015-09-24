// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolHadronElasticPhysics
// Hadron elastic scattering process with option to have polarised
// nucleon scattering
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA new "Pol" classes derived from 4.9.3 G4Had... classes

#ifndef PolHadronElasticPhysics_h
#define PolHadronElasticPhysics_h 1

#include "G4HadronElasticPhysics.hh"
#include "PolUHadronElasticProcess.hh"

class PolHadronElasticPhysics : public G4HadronElasticPhysics
{
public: 
  PolHadronElasticPhysics(const G4String& name = "elastic",
			 G4int ver = 0, G4bool hp = false,
                         G4bool glauber = false);
public: 
  // Includes option to include polarised nucleon scattering
  virtual void ConstructProcess();

protected:
  PolUHadronElasticProcess* nElPolProc;  // for neutrons
  PolUHadronElasticProcess* pElPolProc;  // for protons
public:
  PolUHadronElasticProcess* GetnElPolProc(){ return nElPolProc; }
  PolUHadronElasticProcess* GetpElPolProc(){ return pElPolProc; }
};


#endif








