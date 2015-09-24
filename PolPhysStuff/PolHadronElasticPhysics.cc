// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolHadronElasticPhysics
// Hadron elastic scattering process with option to have polarised
// nucleon scattering
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA new "Pol" classes derived from 4.9.3 G4Had... classes

#include "PolHadronElasticPhysics.hh"

#include "G4HadronicProcess.hh"
#include "G4HadronElasticProcess.hh"
#include "G4HadronicInteraction.hh"
#include "G4LElastic.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4Neutron.hh"
#include "G4VQCrossSection.hh"
#include "G4UElasticCrossSection.hh"
#include "G4BGGNucleonElasticXS.hh"
#include "G4BGGPionElasticXS.hh"

//---------------------------------------------------------------------------
PolHadronElasticPhysics::PolHadronElasticPhysics(
    const G4String& name,  G4int ver, G4bool hp, G4bool glauber)
//  : G4HadronElasticPhysics(name, ver, hp, glauber)
  : G4HadronElasticPhysics93(name, ver, hp, glauber)
{
  wasActivated = false;
}

//---------------------------------------------------------------------------
void PolHadronElasticPhysics::ConstructProcess()
{
  // Adapt from G4HadronElasticPhysics 4.9.3
  // Add specialist instances of polarised UHadronElasticProcess
  // for proton and neutron
  if(wasActivated) return;
  wasActivated = true;

  //  if(verbose > 1) {
  G4cout << "### HadronElasticPhysics Construct Processes with the model <" 
	 << mname << ">" << G4endl;
  //  }
  G4HadronicProcess* hel = 0;
  G4VQCrossSection* man = 0; 

  if((mname == "elastic") || (mname == "NPelastic")) {
    G4HadronElastic* he = new G4HadronElastic();
    model = he;
    man = he->GetCS();
    he->SetQModelLowLimit(0.0);
    he->SetLowestEnergyLimit(0.0);
  } else {
    model = new G4LElastic();
  }

  theParticleIterator->reset();
  while( (*theParticleIterator)() )
  {
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String pname = particle->GetParticleName();
    if(pname == "anti_lambda"  ||
       pname == "anti_neutron" ||
       pname == "anti_omega-"  || 
       pname == "anti_proton"  || 
       pname == "anti_sigma-"  || 
       pname == "anti_sigma+"  || 
       pname == "anti_xi-"  || 
       pname == "anti_xi0"  || 
       pname == "kaon-"     || 
       pname == "kaon+"     || 
       pname == "kaon0S"    || 
       pname == "kaon0L"    || 
       pname == "lambda"    || 
       pname == "omega-"    || 
       pname == "sigma-"    || 
       pname == "sigma+"    || 
       pname == "xi-"       || 
       pname == "alpha"     ||
       pname == "deuteron"  ||
       pname == "triton") {
      
      if(mname == "elastic") {
	G4UHadronElasticProcess* h = new G4UHadronElasticProcess("hElastic");
	h->SetQElasticCrossSection(man);
        hel = h;
        if(glFlag) hel->AddDataSet(new G4UElasticCrossSection(particle));
      } else {                   
	hel = new G4HadronElasticProcess("hElastic");
      }
      hel->RegisterMe(model);
      pmanager->AddDiscreteProcess(hel);
      if(verbose > 1)
	G4cout << "### HadronElasticPhysics added for " 
	       << particle->GetParticleName() << G4endl;

      // proton case
      // Consider both standard and polarised elastic process
    } else if(pname == "proton") {
      if((mname == "elastic") || (mname  == "NPelastic")) {  // extra
	G4UHadronElasticProcess* h;
	if(mname == "NPelastic")
	  h = pElPolProc = new PolUHadronElasticProcess("hElastic");
	else h = new G4UHadronElasticProcess("hElastic");
	h->SetQElasticCrossSection(man);
        hel = h;
        if(glFlag) hel->AddDataSet(new G4BGGNucleonElasticXS(particle));
      } else {                   
	hel = new G4HadronElasticProcess("hElastic");
      }
      hel->RegisterMe(model);
      pmanager->AddDiscreteProcess(hel);
      if(verbose > 1)
	G4cout << "### HadronElasticPhysics added for " 
	       << particle->GetParticleName() << G4endl;

      // neutron case
      // Consider both standard and polarised elastic process
    } 
    else if(pname == "neutron") {   
      if((mname == "elastic") || (mname  == "NPelastic")) {
	G4UHadronElasticProcess* h;
	if(mname == "NPelastic") 
	  h = nElPolProc = new PolUHadronElasticProcess("hElastic");
	else h = new G4UHadronElasticProcess("hElastic");
	G4HadronElastic* nhe = new G4HadronElastic();
	nhe->SetQModelLowLimit(0.0);
	nhe->SetLowestEnergyLimit(0.0);
	neutronModel = nhe;
	h->SetQElasticCrossSection(nhe->GetCS());
        hel = h;
        if(glFlag) hel->AddDataSet(new G4BGGNucleonElasticXS(particle));
      } 
      else {                   
	hel = new G4HadronElasticProcess("hElastic");
	neutronModel = new G4LElastic();
      }

      if(hpFlag) {
	neutronModel->SetMinEnergy(19.5*MeV);
	neutronHPModel = new G4NeutronHPElastic();
	hel->RegisterMe(neutronHPModel);
	hel->AddDataSet(new G4NeutronHPElasticData());
      }

      hel->RegisterMe(neutronModel);
      pmanager->AddDiscreteProcess(hel);

      if(verbose > 1)
	G4cout << "### HadronElasticPhysics added for " 
	       << particle->GetParticleName() << G4endl;

      // pion case
    } else if(pname == "pi+" || pname == "pi-") {
      if(mname == "elastic") {
	G4UHadronElasticProcess* h = new G4UHadronElasticProcess("hElastic");
	h->SetQElasticCrossSection(man);
        hel = h;
        if(glFlag) hel->AddDataSet(new G4BGGPionElasticXS(particle));
      } else {                   
	hel = new G4HadronElasticProcess("hElastic");
      }
      hel->RegisterMe(model);
      pmanager->AddDiscreteProcess(hel);

      if(verbose > 1)
	G4cout << "### HadronElasticPhysics added for " 
	       << particle->GetParticleName() << G4endl;
    }
  }
}


