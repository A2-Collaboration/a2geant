// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolHadronElasticPhysics
// Hadron elastic scattering process with option to have polarised
// nucleon scattering
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA new "Pol" classes derived from 4.9.3 G4Had... classes
//  7/09/11 JRMA use framework of G4WHadronElasticProcess.hh
//
// CHIPS for sampling scattering for p and n
// Glauber model for samplimg of high energy pi+- (E > 1GeV)
// LHEP sampling model for the other particle
// BBG cross sections for p, n and pi+- 
// LHEP cross sections for other particles

#include "PolHadronElasticPhysicsN.hh"

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"

#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4Neutron.hh"

#include "PolWHadronElasticProcess.hh"
//#include "G4WHadronElasticProcess.hh"
#include "G4VHadronElastic.hh"
#include "G4CHIPSElastic.hh"
#include "G4ElasticHadrNucleusHE.hh"
#include "G4BGGNucleonElasticXS.hh"
#include "G4BGGPionElasticXS.hh"
#include "G4NeutronElasticXS.hh"
#include "G4CHIPSElasticXS.hh"

PolHadronElasticPhysicsN::PolHadronElasticPhysicsN(G4int ver)
  : G4HadronElasticPhysics(ver)
{
}

PolHadronElasticPhysicsN::PolHadronElasticPhysicsN(const G4String& name,
    G4int ver, G4bool hp, const G4String&)
  : G4HadronElasticPhysics(name, ver, hp)
{
  if(verbose > 1) { 
    G4cout << "### G4HadronElasticPhysics: " << GetPhysicsName() 
	   << G4endl; 
  }
}

PolHadronElasticPhysicsN::~PolHadronElasticPhysicsN()
{}

void PolHadronElasticPhysicsN::ConstructProcess()
{
  if(wasActivated) { return; }
  wasActivated = true;

  G4double elimit = 1.0*GeV;
  if(verbose > 1) {
    G4cout << "### HadronElasticPhysics Construct Processes with HE limit " 
	   << elimit << " MeV" << G4endl;
  }

  G4VHadronElastic* plep0 = new G4VHadronElastic();
  G4VHadronElastic* plep1 = new G4VHadronElastic();
  plep1->SetMaxEnergy(elimit);

  G4CHIPSElastic* chipsp = new G4CHIPSElastic();
  G4CHIPSElastic* chipsn = new G4CHIPSElastic();

  G4ElasticHadrNucleusHE* he = new G4ElasticHadrNucleusHE(); 
  he->SetMinEnergy(elimit);

  theParticleIterator->reset();
  while( (*theParticleIterator)() )
  {
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String pname = particle->GetParticleName();
    G4WHadronElasticProcess* hel;
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
       pname == "pi-"       || 
       pname == "pi+"       || 
       pname == "sigma-"    || 
       pname == "sigma+"    || 
       pname == "xi-"       || 
       pname == "alpha"     ||
       pname == "deuteron"  ||
       pname == "triton") {
      hel = new G4WHadronElasticProcess();
      if (pname == "pi+" || pname == "pi-") { 
	hel->AddDataSet(new G4BGGPionElasticXS(particle));
	hel->RegisterMe(plep1);
	hel->RegisterMe(he);
      } else {
	hel->RegisterMe(plep0);
      }
      // Proton case
    } else if(pname == "proton") {
      hel = pElPolProc = new PolWHadronElasticProcess();
      hel->AddDataSet(new G4CHIPSElasticXS());
      hel->RegisterMe(chipsp);
      // Neutron case
    } else if(pname == "neutron") {   
      hel = nElPolProc = new PolWHadronElasticProcess();
      hel->AddDataSet(new G4CHIPSElasticXS());
      hel->RegisterMe(chipsn);
    }
    else continue;
    pmanager->AddDiscreteProcess(hel);
    if(verbose > 1) {
      G4cout << "### HadronElasticPhysics: " << hel->GetProcessName()
	     << " added for " << particle->GetParticleName() << G4endl;
    }
  }
}
