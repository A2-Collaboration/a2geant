// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolWHadronElasticPhysics
// Hadron elastic scattering process with option to have polarised
// nucleon scattering
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA new "Pol" classes derived from 4.9.3 G4Had... classes
//  6/09/11 JRMA new "Pol" classes derived from 4.9.4 G4WHadronElasticProcess

// Class Description
// Process for hadron nuclear elastic scattering using optimal 
// combination of Geant4 models
// Class Description - End

#ifndef PolWHadronElasticProcess_h
#define PolWHadronElasticProcess_h 1
 
#include "G4WHadronElasticProcess.hh"
#include "PolNucleonRotate.hh"
#include "PolNPRotate.hh"

class PolWHadronElasticProcess : public G4WHadronElasticProcess
{
public:
  PolWHadronElasticProcess(const G4String& procName = "PolhadElastic");
  virtual ~PolWHadronElasticProcess();
  virtual G4VParticleChange* PostStepDoIt(const G4Track& aTrack, 
					  const G4Step& aStep);
  PolNucleonRotate* GetPolRot(){ return fPolRot; }
  PolNucleonRotate* GetPolRotNP(){ return fPolRotNP; }
  virtual PolNucleonRotate* GetPolRotEvent(){ //return whicheveer rotation was used in this event
    if(fPolRot->DidScat())return fPolRot; 
    else if(fPolRotNP->DidScat())return fPolRotNP; 
    else return NULL;
  }
protected:
  PolNucleonRotate* fPolRot;
  PolNPRotate* fPolRotNP;
  void SetAy(G4double ay){ fPolRot->SetAy(ay); }
  void SetAyParm(G4int parm){ fPolRot->SetAyParm(parm); }
};


#endif
