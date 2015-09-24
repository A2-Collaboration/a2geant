// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolHadronElasticPhysics
// Hadron elastic scattering process with option to have polarised
// nucleon scattering
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA new "Pol" classes derived from 4.9.3 G4Had... classes
// 15/01/11 JRMA G4.9.4 updates

#include "PolUHadronElasticProcess.hh"
#include "PolHadronicProcess.hh"
#include "globals.hh"
#include "G4CrossSectionDataStore.hh"
#include "G4HadronElasticDataSet.hh"
#include "G4VQCrossSection.hh"
// G4.9.4 update
//#include "G4QElasticCrossSection.hh"
#include "G4QProtonElasticCrossSection.hh"
#include "G4QNeutronElasticCrossSection.hh"
//
#include "G4QCHIPSWorld.hh"
#include "G4Element.hh"
#include "G4ElementVector.hh"
#include "G4IsotopeVector.hh"
#include "G4Neutron.hh"
#include "G4Proton.hh"
#include "G4HadronElastic.hh"

 
//------------------------------------------------------------------------------
PolUHadronElasticProcess::PolUHadronElasticProcess(const G4String& pName, G4double elow)
  : G4UHadronElasticProcess(pName, elow)
{
  fPolRot = new PolNucleonRotate(0,0);
}

// //M. Sikora, Oct 12, 2012
// //Trying to fix memory leaks
// PolUHadronElasticProcess::~PolUHadronElasticProcess() {
//   delete fPolRot;
// }

//------------------------------------------------------------------------------
G4VParticleChange* PolUHadronElasticProcess::PostStepDoIt(
				  const G4Track& track, 
				  const G4Step& step)
{
  // Adapt from G4UHadronElasticProcess 4.9.3
  // include asimuthal modulation nucleon organic scattering
  G4ForceCondition   cn;
  aParticleChange.Initialize(track);
  G4double kineticEnergy = track.GetKineticEnergy();
  if(kineticEnergy <= lowestEnergy) 
    return G4VDiscreteProcess::PostStepDoIt(track,step);

  G4double mfp = GetMeanFreePath(track, 0.0, &cn);
  if(mfp == DBL_MAX) 
    return G4VDiscreteProcess::PostStepDoIt(track,step);

  G4Material* material = track.GetMaterial();

  // Select element
  const G4ElementVector* theElementVector = material->GetElementVector();
  G4Element* elm = (*theElementVector)[0];
  G4int nelm = material->GetNumberOfElements() - 1;
  if (nelm > 0) {
    G4double x = G4UniformRand()*cross;
    G4int i = -1;
    do {i++;} while (x > xsec[i] && i < nelm);
    elm = (*theElementVector)[i];
  }
  G4double Z = elm->GetZ();
  G4double A = G4double(G4int(elm->GetN()+0.5));
  G4int iz = G4int(Z);

  // Select isotope
  G4IsotopeVector* isv = elm->GetIsotopeVector(); 
  G4int ni = 0;
  if(isv) ni = isv->size();

  if(ni == 1) { 
    A = G4double((*isv)[0]->GetN());
  } else if(ni > 1) {

    G4double* ab = elm->GetRelativeAbundanceVector();
    G4int j = -1;
    ni--;
    // Special treatment of hydrogen and helium for CHIPS
    if(iz <= 2 && kineticEnergy > thEnergy &&
       (theParticle == theProton || theParticle == theNeutron)) {
      G4double x = G4UniformRand()*xsecH[ni];
      do {j++;} while (x > xsecH[j] && j < ni);

      // GHAD cross sections
    } else {
      G4double y = G4UniformRand();
      do {
	j++;
	y -= ab[j];
      } while (y > 0.0 && j < ni);
    }
    A = G4double((*isv)[j]->GetN());
  } 

  G4HadronicInteraction* hadi = 
    ChooseHadronicInteraction( kineticEnergy, material, elm);

  // Initialize the hadronic projectile from the track
  //  G4cout << "track " << track.GetDynamicParticle()->Get4Momentum()<<G4endl;
  G4HadProjectile thePro(track);
  if(verboseLevel>1) 
    G4cout << "G4UHadronElasticProcess::PostStepDoIt for " 
	   << theParticle->GetParticleName() 
	   << " Target Z= " << Z 
	   << " A= " << A << G4endl; 
  targetNucleus.SetParameters(A, Z);

  aParticleChange.Initialize(track);
  G4HadFinalState* result = hadi->ApplyYourself(thePro, targetNucleus);
  G4ThreeVector indir = track.GetMomentumDirection();
  // Pol extra bit, for Nucleons and H,C materials only
  // Sample modulated azimuthal distribution
  // polarisation needs to be defined in the proton frame
  // i.e if we have it in the lab must do Pol.rotateUz(-indir)
  G4double phiPol = 0.0;
  if( (iz == 1) || (iz == 6) ){
    phiPol = fPolRot->GetPolarisedRotation(result->GetMomentumChange(),
					   track.GetDynamicParticle(),false);
    //phiPol = 0;//M. Sikora, April 6
    G4cout<<"elastic"<<G4endl;
  }
  //  G4ThreeVector outdir = (result->GetMomentumChange()).rotateUz(indir);
  G4ThreeVector outdir = (result->GetMomentumChange());
  if( phiPol )outdir.setPhi(phiPol);  // only if pol-N on organic
  outdir.rotateUz(indir);
  
  if(verboseLevel>1) 
    G4cout << "Efin= " << result->GetEnergyChange()
	   << " de= " << result->GetLocalEnergyDeposit()
	   << " nsec= " << result->GetNumberOfSecondaries()
	   << " dir= " << outdir
	   << G4endl;
  
  aParticleChange.ProposeEnergy(result->GetEnergyChange());
  aParticleChange.ProposeMomentumDirection(outdir);
  if(result->GetNumberOfSecondaries() > 0) {
    aParticleChange.SetNumberOfSecondaries(1);
    G4DynamicParticle* p = result->GetSecondary(0)->GetParticle();
    G4ThreeVector pdir = p->GetMomentumDirection();
    // G4cout << "recoil " << pdir << G4endl;
    // Polarised/Organic only
    // need to rotate the recoil so it is back-to back again
    if( phiPol ){
      phiPol = phiPol - result->GetMomentumChange().phi();
      pdir.rotateZ(phiPol);
    }
    pdir = pdir.rotateUz(indir);
    //G4cout << "recoil rotated " << pdir << G4endl;
    p->SetMomentumDirection(pdir);
    aParticleChange.AddSecondary(p);
  } else {
    aParticleChange.SetNumberOfSecondaries(0);
    aParticleChange.ProposeLocalEnergyDeposit(result->GetLocalEnergyDeposit());
  }
  result->Clear();

  return G4VDiscreteProcess::PostStepDoIt(track,step);
}

