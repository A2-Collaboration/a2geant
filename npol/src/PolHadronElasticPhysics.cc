//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: G4HadronElasticPhysics.cc 89171 2015-03-23 14:58:41Z gcosmo $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4HadronElasticPhysics 
//
// Author: 23 November 2006 V. Ivanchenko
//
// Modified:
// 21.03.2007 V.Ivanchenko Use G4BGGNucleonElasticXS and G4BGGPionElasticXS; 
//                         Reduce thresholds for HE and Q-models to zero
// 03.06.2010 V.Ivanchenko cleanup constructors and ConstructProcess method
// 29.07.2010 V.Ivanchenko rename this class from G4HadronHElasticPhysics to
//                         G4HadronElasticPhysics, old version of the class
//                         is renamed to G4HadronElasticPhysics93
//
//----------------------------------------------------------------------------
//
// CHIPS for sampling scattering for p and n
// Glauber model for samplimg of high energy pi+- (E > 1GeV)
// LHEP sampling model for the other particle
// BBG cross sections for p, n and pi+- 
// LHEP cross sections for other particles
//
// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolHadronElasticPhysics
// Hadron elastic scattering process with option to have polarised
// nucleon scattering
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA new "Pol" classes derived from 4.9.3 G4Had... classes
//  7/09/11 JRMA use framework of G4WHadronElasticProcess.hh
// 21/12/11 JRMA G4VHadronElastic.hh -> G4HadronElastic.hh
// 29/01/12 JRMA Some cleanup for G4.9.5 classes
// 30/01/12 JRMA Set spin tracking for N...add switch
// 21/07/15 JRMA Adapt to G4.10.01

#include "PolHadronElasticPhysics.hh"

#include "G4SystemOfUnits.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4Transportation.hh"

#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"

#include "PolHadronElasticProcess.hh"
#include "G4HadronElastic.hh"
#include "G4ChipsElasticModel.hh"
#include "G4ElasticHadrNucleusHE.hh"
#include "G4AntiNuclElastic.hh"

#include "G4BGGNucleonElasticXS.hh"
#include "G4BGGPionElasticXS.hh"

#include "G4CrossSectionDataSetRegistry.hh"

#include "G4ChipsProtonElasticXS.hh"
#include "G4ChipsNeutronElasticXS.hh"

#include "G4CrossSectionElastic.hh"

// factory
#include "G4PhysicsConstructorFactory.hh"
//
G4_DECLARE_PHYSCONSTR_FACTORY(G4HadronElasticPhysics);
//
G4ThreadLocal G4bool G4HadronElasticPhysics::wasActivated = false;
G4ThreadLocal G4HadronElastic* G4HadronElasticPhysics::neutronModel = 0;
G4ThreadLocal G4HadronicProcess* G4HadronElasticPhysics::neutronProcess = 0;

PolHadronElasticPhysics::PolHadronElasticPhysics(G4int ver)
  : G4HadronElasticPhysics(ver)
{
  fIsSpinPrecN = false;
}


PolHadronElasticPhysics::~PolHadronElasticPhysics()
{}

void PolHadronElasticPhysics::ConstructProcess()
{
  if(wasActivated) { return; }
  wasActivated = true;

  const G4double elimitPi = 1.0*GeV;
  const G4double elimitAntiNuc = 100.*MeV;
  const G4double delta = 0.1*MeV;
  if(verbose > 1) {
    G4cout << "### HadronElasticPhysics::ConstructProcess: Elimit for pi " 
	   << elimitPi/GeV << " GeV" << G4endl;
    G4cout << "                                         for anti-neuclei " 
	   << elimitAntiNuc/GeV << " GeV"	   << G4endl;
  }

  G4AntiNuclElastic* anuc = new G4AntiNuclElastic();
  anuc->SetMinEnergy(elimitAntiNuc);
  G4CrossSectionElastic* anucxs = 
    new G4CrossSectionElastic(anuc->GetComponentCrossSection());

  G4HadronElastic* lhep0 = new G4HadronElastic();
  G4HadronElastic* lhep1 = new G4HadronElastic();
  G4HadronElastic* lhep2 = new G4HadronElastic();
  lhep1->SetMaxEnergy(elimitPi+delta);
  lhep2->SetMaxEnergy(elimitAntiNuc+delta);

  G4ChipsElasticModel* chipsp = new G4ChipsElasticModel();
  neutronModel = new G4ChipsElasticModel();

  G4ElasticHadrNucleusHE* he = new G4ElasticHadrNucleusHE(); 
  he->SetMinEnergy(elimitPi);

  aParticleIterator->reset();
  while( (*aParticleIterator)() )
  {
    G4ParticleDefinition* particle = aParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String pname = particle->GetParticleName();
    if(pname == "anti_lambda"  ||
       pname == "anti_neutron" ||
       pname == "anti_omega-"  || 
       pname == "anti_sigma-"  || 
       pname == "anti_sigma+"  || 
       pname == "anti_xi-"  || 
       pname == "anti_xi0"  || 
       pname == "lambda"    || 
       pname == "omega-"    || 
       pname == "sigma-"    || 
       pname == "sigma+"    || 
       pname == "xi-"       || 
       pname == "alpha"     ||
       pname == "deuteron"  ||
       pname == "triton"   
       ) {
      
      G4HadronElasticProcess* hel = new G4HadronElasticProcess();
      hel->RegisterMe(lhep0);
      pmanager->AddDiscreteProcess(hel);
      if(verbose > 1) {
	G4cout << "### HadronElasticPhysics: " << hel->GetProcessName()
	       << " added for " << particle->GetParticleName() << G4endl;
      }

    } else if(pname == "proton") {   

      G4HadronElasticProcess* hel = pElPolProc = new PolHadronElasticProcess();
      //hel->AddDataSet(new G4BGGNucleonElasticXS(particle));

      //      hel->AddDataSet(new G4ChipsProtonElasticXS());
      hel->AddDataSet(G4CrossSectionDataSetRegistry::Instance()->GetCrossSectionDataSet(G4ChipsProtonElasticXS::Default_Name()));
     
      hel->RegisterMe(chipsp);
      pmanager->AddDiscreteProcess(hel);
      if(verbose > 1) {
	G4cout << "### HadronElasticPhysics: " << hel->GetProcessName()
	       << " added for " << particle->GetParticleName() << G4endl;
      }
      if( fIsSpinPrecN ){
	G4Transportation* trans =
	  (G4Transportation*)((*pmanager->GetAlongStepProcessVector())[0]);
	trans->EnableUseMagneticMoment();
	particle->SetPDGMagneticMoment(fmuP);
      }
    } else if(pname == "neutron") {   

      neutronProcess = nElPolProc = new PolHadronElasticProcess();
      //neutronProcess->AddDataSet(new G4BGGNucleonElasticXS(particle));
      //neutronProcess->AddDataSet(G4CrossSectionDataSetRegistry::Instance()->GetCrossSectionDataSet(G4ChipsNeutronElasticXS::Default_Name()));
      neutronProcess->RegisterMe(neutronModel);
      pmanager->AddDiscreteProcess(neutronProcess);
      if(verbose > 1) {
	G4cout << "### HadronElasticPhysics: " 
	       << neutronProcess->GetProcessName()
	       << " added for " << particle->GetParticleName() << G4endl;
      }
      if( fIsSpinPrecN ){
	G4Transportation* trans =
	  (G4Transportation*)((*pmanager->GetAlongStepProcessVector())[0]);
	trans->EnableUseMagneticMoment();
	particle->SetPDGMagneticMoment(fmuN);
      }
    } else if (pname == "pi+" || pname == "pi-") { 

      G4HadronElasticProcess* hel = new G4HadronElasticProcess();
      hel->AddDataSet(new G4BGGPionElasticXS(particle));
      hel->RegisterMe(lhep1);
      hel->RegisterMe(he);
      pmanager->AddDiscreteProcess(hel);
      if(verbose > 1) {
	G4cout << "### HadronElasticPhysics: " << hel->GetProcessName()
	       << " added for " << particle->GetParticleName() << G4endl;
      }

    } else if(pname == "kaon-"     || 
	      pname == "kaon+"     || 
	      pname == "kaon0S"    || 
	      pname == "kaon0L" 
	      ) {
      
      G4HadronElasticProcess* hel = new G4HadronElasticProcess();
      hel->RegisterMe(lhep0);
      pmanager->AddDiscreteProcess(hel);
      if(verbose > 1) {
	G4cout << "### HadronElasticPhysics: " << hel->GetProcessName()
	       << " added for " << particle->GetParticleName() << G4endl;
      }

    } else if(
       pname == "anti_proton"    || 
       pname == "anti_alpha"     ||
       pname == "anti_deuteron"  ||
       pname == "anti_triton"    ||
       pname == "anti_He3"       ) {

      G4HadronElasticProcess* hel = new G4HadronElasticProcess();
      hel->AddDataSet(anucxs);
      hel->RegisterMe(lhep2);
      hel->RegisterMe(anuc);
      pmanager->AddDiscreteProcess(hel);
    }
  }
}


G4HadronElastic* G4HadronElasticPhysics::GetNeutronModel()
{
  return neutronModel;
}

G4HadronicProcess* G4HadronElasticPhysics::GetNeutronProcess()
{
  return neutronProcess;
}


