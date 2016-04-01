// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolHadronInelasticPhysics
// Additional class for inelastic neutron stuff
// 20/06/09 Adapt Derek Glazier's class DGPolHadronInelasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA new "Pol" classes compatible with 4.9.3 G4Had... classes

#include "PolHadronInelasticPhysics.hh"
#include "PolHadronInelasticProcess.hh"
#include "PolHadronQuasiFreeProcess.hh"

#include "globals.hh"
#include "G4ios.hh"
#include <iomanip>   
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4ShortLivedConstructor.hh"
#include "G4ProcessManager.hh"
#include "G4SystemOfUnits.hh"

PolHadronInelasticPhysics::PolHadronInelasticPhysics(const G4String& name)
                    :  G4VPhysicsConstructor(name) 
{}

void PolHadronInelasticPhysics::CreateModels()
{
  theProtonInelastic = NULL;
  theNeutronInelastic = NULL;
  theBinary =  NULL;
  if(namePhysics.contains("QF")){
    theProtonInelastic = 
      new PolHadronQuasiFreeProcess("PolProtonQuasiFree",G4Proton::Proton());
    theNeutronInelastic =
      new PolHadronQuasiFreeProcess("PolNeutronQuasiFree",G4Neutron::Neutron());
  }
  else {
  theProtonInelastic = 
    new PolHadronInelasticProcess("PolProtonInelastic",G4Proton::Proton());
  theNeutronInelastic =
    new PolHadronInelasticProcess("PolNeutronInelastic",G4Neutron::Neutron());
  }
  theBinary = new G4BinaryCascade;
  theMin = 0;
  theMax = 9.9*GeV;

}

PolHadronInelasticPhysics::~PolHadronInelasticPhysics() 
{
   delete theBinary;
   delete theProtonInelastic;
   delete theNeutronInelastic;
}

void PolHadronInelasticPhysics::ConstructParticle()
{
  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();

  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();

  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();  
}

void PolHadronInelasticPhysics::ConstructProcess()
{
  CreateModels();
   // thePro->Build();
  // to use this build would mean needing classes for each type of
  // proton builder that use from PolProtonBuilder
  theProtonInelastic->AddDataSet(&theXSec);  
  theBinary->SetMinEnergy(theMin);
  theBinary->SetMaxEnergy(theMax);
  theProtonInelastic->RegisterMe(theBinary);
  G4ProcessManager * theProcMan = G4Proton::Proton()->GetProcessManager();
  theProcMan->AddDiscreteProcess(theProtonInelastic);

  theNeutronInelastic->AddDataSet(&theNXSec);  
  //theBinary->SetMinEnergy(theMin);
  //theBinary->SetMaxEnergy(theMax);
  theNeutronInelastic->RegisterMe(theBinary);
  G4ProcessManager * theNProcMan = G4Neutron::Neutron()->GetProcessManager();
  theNProcMan->AddDiscreteProcess(theNeutronInelastic);
}

