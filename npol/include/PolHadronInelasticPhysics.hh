// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolHadronInelasticPhysics
// Additional class for inelastic neutron stuff
// 20/06/09 Adapt Derek Glazier's class DGPolHadronInelasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA new "Pol" classes compatible with 4.9.3 G4Had... classes

#ifndef PolHadronInelasticPhysics_h
#define PolHadronInelasticPhysics_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4VPhysicsConstructor.hh"
#include "G4ProtonInelasticCrossSection.hh"
#include "G4NeutronInelasticCrossSection.hh"
#include "G4BinaryProtonBuilder.hh"
#include "G4BinaryNeutronBuilder.hh"
#include "PolHadronInelasticProcess.hh"

class PolHadronInelasticPhysics : public G4VPhysicsConstructor
{
public: 
    PolHadronInelasticPhysics(const G4String& name ="hadron");
    virtual ~PolHadronInelasticPhysics();

public: 
    virtual void ConstructParticle();
    virtual void ConstructProcess();

private:
  void CreateModels();
  
  // PolHadronInelasticProcess * theProtonInelastic;
  // PolHadronInelasticProcess * theNeutronInelastic;
  PolHadronicProcess * theProtonInelastic;
  PolHadronicProcess * theNeutronInelastic;

  G4ProtonInelasticCrossSection theXSec;
  G4NeutronInelasticCrossSection theNXSec;
  G4BinaryCascade * theBinary;    
  G4double theMin;
  G4double theMax;
public:
  // PolHadronInelasticProcess * GettheProtonInelastic()
  // { return theProtonInelastic; }
  // PolHadronInelasticProcess * GettheNeutronInelastic()
  // { return theNeutronInelastic; }
  PolHadronicProcess * GettheProtonInelastic()
  { return theProtonInelastic; }
  PolHadronicProcess * GettheNeutronInelastic()
  { return theNeutronInelastic; }
};

#endif

