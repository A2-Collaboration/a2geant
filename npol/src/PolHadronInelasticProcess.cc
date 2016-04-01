#include "PolHadronInelasticProcess.hh"
#include "G4HadronInelasticDataSet.hh"
#include "G4GenericIon.hh"
#include "G4ParticleDefinition.hh"
  
PolHadronInelasticProcess::
PolHadronInelasticProcess(const G4String& processName,
			 G4ParticleDefinition* aParticle):
  PolHadronicProcess(processName)
{
  SetProcessSubType(fHadronInelastic);
  AddDataSet(new G4HadronInelasticDataSet());
  theParticle = aParticle;
}

PolHadronInelasticProcess::~PolHadronInelasticProcess() 
{}
