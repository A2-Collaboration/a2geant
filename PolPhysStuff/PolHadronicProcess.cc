// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolHadronicProcess
// Hadonic process base with polarised nucleon scattering options
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA new "Pol" classes derived from 4.9.3 G4Had... classes

#include "TMath.h"
#include "G4Types.hh"
#include "PolHadronicProcess.hh"
#include "G4HadProjectile.hh"
#include "G4ElementVector.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Element.hh"
#include "G4ParticleChange.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4ProcessVector.hh"
#include "G4ProcessManager.hh"
#include "G4StableIsotopes.hh"
#include "G4HadTmpUtil.hh"
#include "G4HadLeadBias.hh"
#include "G4HadronicException.hh"
#include "G4HadReentrentException.hh"
#include "G4HadronicWhiteBoard.hh"
#include "G4HadSignalHandler.hh"
#include "G4HadronicProcessStore.hh"
#include <typeinfo>
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
PolHadronicProcess::PolHadronicProcess( const G4String &processName,
                                      G4ProcessType   aType ) :
G4HadronicProcess( processName, aType)
{ 
  fDefAy = 0.0;
  fParmAy = EAyNull;
  fPolRot = NULL;
  fPolRot = new PolNucleonRotate(0,0);
}

//M. Sikora, Oct 12, 2012
//Trying to stop memory leaks
// PolHadronicProcess::~PolHadronicProcess() {
//   delete fPolRot;
// }

//---------------------------------------------------------------------------
void 
PolHadronicProcess::FillTotalResult(G4HadFinalState * aR, const G4Track & aT)
{
  // Adapted from G4HadronicProcess 4.9.3
  // Add polarised nucleon scattering
  //
  theTotalResult->Clear();
  theTotalResult->ProposeLocalEnergyDeposit(0.);
  theTotalResult->Initialize(aT);
  theTotalResult->SetSecondaryWeightByProcess(true);
  theTotalResult->ProposeTrackStatus(fAlive);
  G4double rotation = 2.*pi*G4UniformRand();
  G4ThreeVector it(0., 0., 1.);

  if(aR->GetStatusChange()==stopAndKill)
  {
    if( xBiasOn && G4UniformRand()<XBiasSurvivalProbability() )
    {
      theTotalResult->ProposeParentWeight( XBiasSurvivalProbability()*aT.GetWeight() );
    }
    else
    {
      theTotalResult->ProposeTrackStatus(fStopAndKill);
      theTotalResult->ProposeEnergy( 0.0 );
    }
  }
  else if(aR->GetStatusChange()!=stopAndKill )
  {
    if(aR->GetStatusChange()==suspend)
    {
      theTotalResult->ProposeTrackStatus(fSuspend);
      if(xBiasOn)
      {
        G4Exception("G4HadronicProcess", "007", FatalException,
        "Cannot cross-section bias a process that suspends tracks.");
      }
    } else if (aT.GetKineticEnergy() == 0) {
      theTotalResult->ProposeTrackStatus(fStopButAlive);
    }

    if(xBiasOn && G4UniformRand()<XBiasSurvivalProbability())
    {
      theTotalResult->ProposeParentWeight( XBiasSurvivalProbability()*aT.GetWeight() );
      G4double newWeight = aR->GetWeightChange()*aT.GetWeight();
      G4double newM=aT.GetDefinition()->GetPDGMass();
      G4double newE=aR->GetEnergyChange() + newM;
      G4double newP=std::sqrt(newE*newE - newM*newM);
      G4DynamicParticle * aNew = 
      new G4DynamicParticle(aT.GetDefinition(), newE, newP*aR->GetMomentumChange());
      G4HadSecondary * theSec = new G4HadSecondary(aNew, newWeight);
      aR->AddSecondary(theSec);
    }
    else
    {
      G4double newWeight = aR->GetWeightChange()*aT.GetWeight();
      theTotalResult->ProposeParentWeight(newWeight); // This is multiplicative
      if(aR->GetEnergyChange()>-.5) 
      {
        theTotalResult->ProposeEnergy(aR->GetEnergyChange());
      }
      G4LorentzVector newDirection(aR->GetMomentumChange().unit(), 1.);
      newDirection*=aR->GetTrafoToLab();
      theTotalResult->ProposeMomentumDirection(newDirection.vect());
    }
  }
  else
  {
    G4cerr << "Track status is "<< aR->GetStatusChange()<<G4endl;
    G4Exception("G4HadronicProcess", "007", FatalException,
    "use of unsupported track-status.");
  }

  if(GetProcessName() != "hElastic" && GetProcessName() != "HadronElastic"
     &&  theTotalResult->GetTrackStatus()==fAlive
     && aR->GetStatusChange()==isAlive)
    {
    // Use for debugging:   G4double newWeight = theTotalResult->GetParentWeight();

    G4double newKE = std::max(DBL_MIN, aR->GetEnergyChange());
    G4DynamicParticle* aNew = new G4DynamicParticle(aT.GetDefinition(), 
                                                    aR->GetMomentumChange(), 
                                                    newKE);
    G4HadSecondary* theSec = new G4HadSecondary(aNew, 1.0);
    aR->AddSecondary(theSec);
    aR->SetStatusChange(stopAndKill);

    theTotalResult->ProposeTrackStatus(fStopAndKill);
    theTotalResult->ProposeEnergy( 0.0 );

  }
  theTotalResult->ProposeLocalEnergyDeposit(aR->GetLocalEnergyDeposit());
  theTotalResult->SetNumberOfSecondaries(aR->GetNumberOfSecondaries());

  if(aR->GetStatusChange() != stopAndKill)
  {
    G4double newM=aT.GetDefinition()->GetPDGMass();
    G4double newE=aR->GetEnergyChange() + newM;
    G4double newP=std::sqrt(newE*newE - newM*newM);
    G4ThreeVector newPV = newP*aR->GetMomentumChange();
    G4LorentzVector newP4(newE, newPV);
    newP4.rotate(rotation, it);
    newP4*=aR->GetTrafoToLab();
    theTotalResult->ProposeMomentumDirection(newP4.vect().unit());
  }

  // Polarised nucleon scattering in hydro-carbon material
  // Derek G's comment
  // Loop through all produced particles and find the one with
  // the highest kinetic energy which shall be identified as the original
  // particle allowing for charge exchange.
  // the initial particle is killed unless 
  // "Keep initial projectile" is displayed
  G4double phiPol = 0.0;    // phi modification angle
  G4double highKE = 0.0;    // highest KE of secondary nucleons
  G4int ip = 0;             // index of secondary nucleon with highest KE
  //*******commented out by M. Sikora, April 9, 2013, want to give skirt/tunnel apow
  // if ( IsOrganic(&aT) ) {     // does tracking material contain H,C?
  if ( IsOrganic(&aT) || aT.GetVolume()->GetName().contains("SKI") || 
       aT.GetVolume()->GetName().contains("TUNL")) {     // does tracking material contain H,C? or did it scatter in skirt/tunnel?
    
    //************************
    if ( IsNucleon(aT.GetDynamicParticle()) ) {
      highKE = 0.0;
      ip = 0;
      for (G4int i = 0; i < aR->GetNumberOfSecondaries(); ++i) {
	G4DynamicParticle* secDP = aR->GetSecondary(i)->GetParticle();
	//G4cout<<"HAd "<<i<<" "<<secDP->GetDefinition()->GetParticleName()<<" "<< secDP->GetKineticEnergy()<<" "<< secDP->GetMomentum().theta()/deg<<G4endl;
	if (IsNucleon(secDP) && secDP->GetKineticEnergy() > highKE) {
	  highKE = secDP->GetKineticEnergy(); 
	  ip=i;
	}
      }
      //G4cout<<"hadronic process! "<<ip<<"/"<<aR->GetNumberOfSecondaries()<<"  KE = "<<highKE<<"     ";
      // Recalculate phi angle, and return the rotation needed to get it there 
      phiPol=fPolRot->GetPolarisedRotation(aR->GetSecondary(ip)->GetParticle(),
					   aT.GetDynamicParticle());
    }
  }
  else phiPol = 0.0;       // no polarised scattering
  //G4cout<<phiPol<<G4endl;
  for (G4int i = 0; i < aR->GetNumberOfSecondaries(); ++i) {
    G4LorentzVector theM = aR->GetSecondary(i)->GetParticle()->Get4Momentum();
    // Note the extra random azimuthal rotation is removed...   
    //if( !phiPol )    theM.rotate(rotation, it);
    //else
    if (highKE > 0.0) {//added by M. Sikora, April 4, 2013
      theM.rotateZ(phiPol);
    }
    G4double fixphi=theM.phi();
    // G4cout<<"1 had sec "<<theM.theta()/deg<<" "<<theM.phi()/deg<<G4endl;
    theM*=aR->GetTrafoToLab();//BUGFIX needed, randomising phi!!
    // theM.setPhi(fixphi);
   
    aR->GetSecondary(i)->GetParticle()->Set4Momentum(theM);
    //  G4cout<<"2 had sec "<<theM.theta()/deg<<" "<<theM.phi()/deg<<" "<< aT.GetDynamicParticle()->GetMomentum().theta()/deg<<" "<< aT.GetDynamicParticle()->GetMomentum().phi()/deg<<G4endl;
    G4double time = aR->GetSecondary(i)->GetTime();
    if(time<0) time = aT.GetGlobalTime();

    G4Track* track = new G4Track(aR->GetSecondary(i)->GetParticle(),
				 time, aT.GetPosition());

    G4double newWeight = aT.GetWeight()*aR->GetSecondary(i)->GetWeight();
    //static G4double pinelcount=0;
    if(xBiasOn) newWeight *= XBiasSecondaryWeight();
    track->SetWeight(newWeight);
    track->SetTouchableHandle(aT.GetTouchableHandle());
    theTotalResult->AddSecondary(track);
  }

  aR->Clear(); 

  return;
}
