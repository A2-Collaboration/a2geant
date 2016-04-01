#ifndef PolHadronInelasticProcess_h
#define PolHadronInelasticProcess_h 1

#include "PolHadronicProcess.hh"

class G4ParticleDefinition;

class PolHadronInelasticProcess : public PolHadronicProcess
{
public:
  PolHadronInelasticProcess(const G4String &processName,
			    G4ParticleDefinition *aParticle );
  virtual ~PolHadronInelasticProcess();
  virtual G4bool IsApplicable(const G4ParticleDefinition& aP)
  { return  theParticle == &aP || theParticle == G4GenericIon::GenericIon(); }
  // required??
  //  void BuildThePhysicsTable();
  //virtual G4VParticleChange *PostStepDoIt(const G4Track &aTrack,
  //					  const G4Step &aStep)
  //  { return PolHadronicProcess::GeneralPostStepDoIt( aTrack, aStep ); }
  //  G4double GetMicroscopicCrossSection( const G4DynamicParticle *aParticle,
  //				       const G4Element *anElement,
  //				       G4double aTemp);
private:

  G4ParticleDefinition* theParticle;
  // ??
  //G4ParticleChange theParticleChange;

};
 
#endif
 
