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
  virtual void FillTotalResult(G4HadFinalState * aR, const G4Track & aT);
  virtual PolNucleonRotate* GetPolRotNP(){return fPolRotNP;} 
  virtual PolNucleonRotate* GetPolRotEvent(){ 
    if(fPolRot->DidScat())return fPolRot; 
    else if(fPolRotNP->DidScat())return fPolRotNP; 
    else return NULL;
  }  // required??
  //  void BuildThePhysicsTable();
  //virtual G4VParticleChange *PostStepDoIt(const G4Track &aTrack,
  //					  const G4Step &aStep)
  //  { return PolHadronicProcess::GeneralPostStepDoIt( aTrack, aStep ); }
  //  G4double GetMicroscopicCrossSection( const G4DynamicParticle *aParticle,
  //				       const G4Element *anElement,
  //				       G4double aTemp);
private:

  G4ParticleDefinition* theParticle;
  PolNPRotate* fPolRotNP;
   // ??
  //G4ParticleChange theParticleChange;

};
 
#endif
 
