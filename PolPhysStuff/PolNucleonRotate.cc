// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolNucleonRotate
// Azimuthal modulation of polarised nucleon scattering
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA New class separated from hadronic processes
// 20/11/10 JRMA Save polarisation
 
#include "PolNucleonRotate.hh"
#include "G4RotationMatrix.hh"
#include "G4HadSignalHandler.hh"
#include "G4RunManager.hh"
#include "A2CBOutput.hh"
#include "A2EventAction.hh"
#include <typeinfo>
//G4ThreeVector* PolNN=NULL;
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
PolNucleonRotate::PolNucleonRotate( G4double ay, G4int pay, G4int verbose )
{
  // verbose is by default 0 (ie not verbose)
  fDefAy = ay;
  fParmAy = pay;
  fVerbose = verbose;
  fSaveScat=false;
  fDidScat=false;
  fIsDetFrame=false;
  fScatVec=NULL;
  fScatVec=new G4LorentzVector();
  fScatTheta=0.0;
  fScatPhi=0.0;
  
}

//-----------------------------------------------------------------------------
G4double PolNucleonRotate::
GetPolarisedRotation(const G4ThreeVector newP,
		     const G4DynamicParticle *oldPart, G4bool IsDiff){
 // Then modify the azimuthal distribution using the chosen parameterisation
  // of the N + CH analysing power.
  // Already in frame defined by incident particle until GetTrafoToLab later
  //
  G4ThreeVector Pol(oldPart->GetPolarization());
  if(fIsDetFrame){//for case where incident direction is close to z-axis it might be better to define polarisation in lab frame, if you use the particle frame the polarisation direction is rotated by the particle phi, which may not produce the desired results (e.g. polarisation will average to zero)
    G4RotationMatrix toZ;
    G4ThreeVector theOrgMom= oldPart->GetMomentumDirection();
    toZ.rotateZ(-theOrgMom.phi());
    toZ.rotateY(-theOrgMom.theta());
    Pol*=(toZ);
    G4cout<<"IsDetFrame    old part theta = "<<theOrgMom.theta()*180./3.14<<G4endl;
  }
  
  G4double Pt=sqrt(Pol.x()*Pol.x()+Pol.y()*Pol.y()); //transverse polarisation
  if( !Pt ) return 0.0;
  //sin(phi) asymmetry with phi0 being along the transverse polarisation dir.
  // *PolNN = Pol;
  G4double phi0 = Pol.phi();
  // G4double ay = Ay(oldPart->GetKineticEnergy(),newP.theta());//McNaughton takes momentum
  G4double ay = Ay(oldPart->GetTotalMomentum(),newP.theta());
  G4double newphi = pi*G4UniformRand();
  if( 2*G4UniformRand() > (1+ay*Pt*sin(newphi)) ) newphi=-pi+newphi; 
  newphi+=phi0;
  //newphi=pi+phi0;
  G4double phiP = newP.phi();
  //  G4double delta_phi = newphi - newP.phi();
  G4double delta_phi = newphi - phiP;
  //fVerbose=2;
  if(fVerbose >1){
    G4cout<<" HadronicProcess Pt="<<Pt<<" phi0="<<phi0/deg<<" "
	  <<newP.theta()/deg<<" "
	  <<oldPart->GetKineticEnergy()<<" Ay="<<ay<<" phi="
	  <<newphi/deg<<" delphi="<<delta_phi/deg<<" "<<fSaveScat<<" "<<fDidScat<<" "<<fIsDetFrame<<G4endl;
  }
  
  
  //Save scatter dynamics
  if(fSaveScat&&!fDidScat){//save the first scattering interaction of this event
    fScatPhi=newphi;
    fScatTheta=newP.theta();
    //G4cout<<fScatTheta/deg<<G4endl;
    fScatE=oldPart->GetKineticEnergy()/MeV;
    fScatAy=ay;
    fDidScat=true;
    G4double momentum=oldPart->GetTotalMomentum()/MeV;
    fScatVec->setRThetaPhi(momentum,fScatTheta,fScatPhi);
    fScatVec->setE(sqrt(fScatOutM*fScatOutM+momentum*momentum));
    A2CBOutput* cbout=((A2EventAction*)G4RunManager::GetRunManager()->GetUserEventAction())->GetOutput();
    cbout->SetThetaSc(fScatTheta/deg);
    cbout->SetPhiSc(fScatPhi/deg);
  }
  
  if( !IsDiff ) return newphi;
  return delta_phi;
  
}
//-----------------------------------------------------------------------------
G4double PolNucleonRotate::
GetPolarisedRotation(const G4DynamicParticle *newPart,
		     const G4DynamicParticle *oldPart, G4bool IsDiff)
{

  //G4cout<<oldPart->GetDefinition()->GetPDGMass()/MeV<<"   "<<newPart->GetDefinition()->GetPDGMass()/MeV<<"      "<<IsDiff<<G4endl;

  // This variant takes new and old dynamic particles
  if(fSaveScat){
    fScatInM=oldPart->GetDefinition()->GetPDGMass()/MeV;//proton or neutron?
    fScatOutM=newPart->GetDefinition()->GetPDGMass()/MeV;//proton or neutron?
  }
  return GetPolarisedRotation( newPart->GetMomentum(), oldPart, IsDiff );
}
