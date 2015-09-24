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

#include "PolWHadronElasticProcess.hh"
#include "globals.hh"
#include "G4CrossSectionDataStore.hh"
#include "G4HadronElasticDataSet.hh"
#include "G4VQCrossSection.hh"
#include "G4Element.hh"
#include "G4ElementVector.hh"
#include "G4IsotopeVector.hh"
#include "G4Neutron.hh"
#include "G4ProductionCutsTable.hh"
 
PolWHadronElasticProcess::PolWHadronElasticProcess(const G4String& pName)
  : G4WHadronElasticProcess(pName) 
{
  fPolRot = new PolNucleonRotate(0,0,0);
  fPolRotNP = new PolNPRotate(0,0,0);
  verboseLevel=0;
}

PolWHadronElasticProcess::~PolWHadronElasticProcess()
{
  //M. Sikora, Oct 12, 2012
  //Trying to stop memory leaks
  //delete fPolRot;
  //delete fPolRotNP;
}

G4VParticleChange* PolWHadronElasticProcess::PostStepDoIt(
				  const G4Track& track, 
				  const G4Step& step)
{
  aParticleChange.Initialize(track);
  G4double kineticEnergy = track.GetKineticEnergy();
  const G4DynamicParticle* dynParticle = track.GetDynamicParticle();
  const G4ParticleDefinition* part = dynParticle->GetDefinition();

  // protection against numerical problems
  if(part == theNeutron) {
    if(kineticEnergy <= lowestEnergyNeutron) 
      return G4VDiscreteProcess::PostStepDoIt(track,step);
  } else {
    if(kineticEnergy <= lowestEnergy)
      return G4VDiscreteProcess::PostStepDoIt(track,step);
  }

  G4Material* material = track.GetMaterial();
  G4String volume_name = (G4String)track.GetVolume()->GetName();//M. Sikora, April 8, 2013
  G4CrossSectionDataStore* store = GetCrossSectionDataStore();
  G4double xsec = store->GetCrossSection(dynParticle,material);
  if(xsec <= DBL_MIN) return G4VDiscreteProcess::PostStepDoIt(track,step);

  // Select element
  G4Element* elm = store->SampleZandA(dynParticle,material,targetNucleus);
  G4int iz = G4int(elm->GetZ());

  G4HadronicInteraction* hadi = 
    ChooseHadronicInteraction( kineticEnergy, material, elm);

  size_t idx = track.GetMaterialCutsCouple()->GetIndex();
  G4double tcut = 
    (*(G4ProductionCutsTable::GetProductionCutsTable()->GetEnergyCutsVector(3)))[idx];
  hadi->SetRecoilEnergyThreshold(tcut);

  // Initialize the hadronic projectile from the track
  //  G4cout << "track " << track.GetDynamicParticle()->Get4Momentum()<<G4endl;
  G4HadProjectile thePro(track);
  if(verboseLevel>1) {
    G4cout << "G4WHadronElasticProcess::PostStepDoIt for " 
	   << part->GetParticleName()
	   << " in " << material->GetName() 
	   << " Target Z= " << iz 
	   << " A= " << targetNucleus.GetA_asInt() << G4endl; 
  }
  G4HadFinalState* result = hadi->ApplyYourself(thePro, targetNucleus);
  G4ThreeVector indir = track.GetMomentumDirection();
  //  G4ThreeVector outdir = (result->GetMomentumChange()).rotateUz(indir);
  // Pol extra bit, for Nucleons and H,C materials only
  // Sample modulated azimuthal distribution
  // polarisation needs to be defined in the proton frame
  // i.e if we have it in the lab must do Pol.rotateUz(-indir)
  G4double phiPol = 0.0;
  if( (iz == 1) ){//special case of nucleon-proton scattering
    fPolRotNP->SetReaction(track.GetDynamicParticle()->GetDefinition(),G4Proton::Proton());  //i.e. which nucleon-proton scatter
    phiPol = fPolRotNP->GetPolarisedRotation(result->GetMomentumChange(),
					   track.GetDynamicParticle(),false);
  } else if( (iz == 6) || volume_name.contains("SKI") || volume_name.contains("TUNL")) {//give skirt,tunnel apow
    phiPol = fPolRot->GetPolarisedRotation(result->GetMomentumChange(),
     					   track.GetDynamicParticle(),false);
  }
   //  G4ThreeVector outdir = (result->GetMomentumChange()).rotateUz(indir);
  G4ThreeVector outdir = (result->GetMomentumChange());
  if( phiPol )outdir.setPhi(phiPol);  // only if pol-N on organic
  //G4cout << "1elastic recoil rotated " << outdir.theta()/deg<< " "<<outdir.phi()/deg << " IN "<<indir.theta()/deg<< "  "<<indir.phi()/deg<<G4endl;
  //***********line below commented out by M. Sikora, April 6, 2013 *******
  //outdir.rotateUz(-indir);//does not seeem to be woring correctly remove but must replace for nucleons moving away fromaz axis, also for secondries
  outdir.rotateUz(indir);//M. Sikora, April 6, 2013, changed to match old DGPolUHadronElasticProcess.cc line 362, geant4/polA2/src
  //****************
  //if( phiPol )outdir.setPhi(phiPol); //BUGFIX needed

  //G4cout << "2elastic recoil rotated " << outdir.theta()/deg<< " "<<outdir.phi()/deg << " IN "<<indir<<G4endl;

  if(verboseLevel>1) {
    G4cout << "Efin= " << result->GetEnergyChange()
	   << " de= " << result->GetLocalEnergyDeposit()
	   << " nsec= " << result->GetNumberOfSecondaries()
	   << " dir= " << outdir
	   << " phiPol= " << phiPol
	   << G4endl;
  }
  
  aParticleChange.ProposeEnergy(result->GetEnergyChange());
  aParticleChange.ProposeMomentumDirection(outdir);
  if(result->GetNumberOfSecondaries() > 0) {
    aParticleChange.SetNumberOfSecondaries(1);
    G4DynamicParticle* p = result->GetSecondary(0)->GetParticle();
    G4ThreeVector pdir = p->GetMomentumDirection();
    // G4cout << "recoil " << pdir << G4endl;
    if( phiPol ){
      phiPol = phiPol - result->GetMomentumChange().phi();
      pdir.rotateZ(phiPol);
    }
    G4double fixphi= pdir.phi();
    pdir = pdir.rotateUz(indir);
    //pdir.setPhi(fixphi); 
    p->SetMomentumDirection(pdir);
    aParticleChange.AddSecondary(p);
  } else {
    aParticleChange.SetNumberOfSecondaries(0);
    aParticleChange.ProposeLocalEnergyDeposit(result->GetLocalEnergyDeposit());
  }

  result->Clear();
  
  return G4VDiscreteProcess::PostStepDoIt(track,step);
}
