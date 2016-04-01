#ifndef PolHadronQuasiFreeProcess_h
#define PolHadronQuasiFreeProcess_h 1

#include "PolHadronicProcess.hh"
#include "PolNPRotate.hh"

class G4ParticleDefinition;

class PolHadronQuasiFreeProcess : public PolHadronicProcess
{
public:
  PolHadronQuasiFreeProcess(const G4String &processName,
			    G4ParticleDefinition *aParticle );
  virtual ~PolHadronQuasiFreeProcess();
  virtual G4bool IsApplicable(const G4ParticleDefinition& aP)
  { return  theParticle == &aP || theParticle == G4GenericIon::GenericIon(); }
  virtual void FillResult(G4HadFinalState * aR, const G4Track & aT);
  virtual PolNucleonRotate* GetPolRotNP(){return fPolRotNP;} 
  virtual PolNucleonRotate* GetPolRotEvent(){ 
    if(fPolRot->DidScat())return fPolRot; 
    else if(fPolRotNP->DidScat())return fPolRotNP; 
    else return NULL;
  } 
private:

  G4ParticleDefinition* theParticle;
  PolNPRotate* fPolRotNP;

};
 
#endif
 
