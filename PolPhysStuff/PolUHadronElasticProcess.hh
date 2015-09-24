// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolHadronElasticPhysics
// Hadron elastic scattering process with option to have polarised
// nucleon scattering
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA new "Pol" classes derived from 4.9.3 G4Had... classes

#ifndef PolUHadronElasticProcess_h
#define PolUHadronElasticProcess_h 1
 
#include "G4UHadronElasticProcess.hh"
#include "PolNucleonRotate.hh"

class PolUHadronElasticProcess : public G4UHadronElasticProcess
{
public:
  PolUHadronElasticProcess(const G4String& procName = "hElastic", 
			  G4double elow = 19.*MeV); 
  //virtual ~PolUHadronElasticProcess();
  virtual G4VParticleChange* PostStepDoIt(const G4Track& aTrack, 
					  const G4Step& aStep);
  PolNucleonRotate* GetPolRot(){ return fPolRot; }
protected:
  PolNucleonRotate* fPolRot;
  void SetAy(G4double ay){ fPolRot->SetAy(ay); }
  void SetAyParm(G4int parm){ fPolRot->SetAyParm(parm); }
};

#endif
