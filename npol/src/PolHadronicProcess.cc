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
// $Id: G4HadronicProcess.cc 86863 2014-11-19 14:39:31Z gcosmo $
//
// -------------------------------------------------------------------
//
// GEANT4 Class source file
//
// G4HadronicProcess
//
// original by H.P.Wellisch
// J.L. Chuma, TRIUMF, 10-Mar-1997
//
// Modifications:
// 05-Jul-2010 V.Ivanchenko cleanup commented lines
// 20-Jul-2011 M.Kelsey -- null-pointer checks in DumpState()
// 24-Sep-2011 M.Kelsey -- Use envvar G4HADRONIC_RANDOM_FILE to save random
//		engine state before each model call
// 18-Oct-2011 M.Kelsey -- Handle final-state cases in conservation checks.
// 14-Mar-2012 G.Folger -- enhance checks for conservation of energy, etc.
// 28-Jul-2012 M.Maire  -- add function GetTargetDefinition() 
// 14-Sep-2012 Inherit from RestDiscrete, use subtype code (now in ctor) to
//		configure base-class
// 28-Sep-2012 Restore inheritance from G4VDiscreteProcess, remove enable-flag
//		changing, remove warning message from original ctor.
//
// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolHadronicProcess
// Hadonic process base with polarised nucleon scattering options
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA new "Pol" classes derived from 4.9.3 G4Had... classes
// 21/07/15 JRMA adapt for G4.10.01

#include "PolHadronicProcess.hh"

#include "G4Types.hh"
#include "G4SystemOfUnits.hh"
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
#include "G4NucleiProperties.hh"

#include "G4HadronicException.hh"
#include "G4HadronicProcessStore.hh"

#include <typeinfo>
#include <sstream>
#include <iostream>

#include <stdlib.h>
static const char* G4Hadronic_Random_File = getenv("G4HADRONIC_RANDOM_FILE");
//----------------------------------------------------------------------------
PolHadronicProcess::PolHadronicProcess( const G4String &processName,
                                      G4ProcessType   aType ) :
G4HadronicProcess( processName, aType)
{ 
  fDefAy = 0.0;
  fParmAy = EAyNull;
  fPolRot = new PolNucleonRotate(0,0);
}

//----------------------------------------------------------------------------
PolHadronicProcess::PolHadronicProcess( const G4String &processName,
                                      G4HadronicProcessType   aType ) :
G4HadronicProcess( processName, aType)
{ 
  fDefAy = 0.0;
  fParmAy = EAyNull;
  fPolRot = new PolNucleonRotate(0,0);
}

//----------------------------------------------------------------------------
void PolHadronicProcess::FillResult(G4HadFinalState * aR, const G4Track & aT)
{
  theTotalResult->ProposeLocalEnergyDeposit(aR->GetLocalEnergyDeposit());

  G4double rotation = CLHEP::twopi*G4UniformRand();
  G4ThreeVector it(0., 0., 1.);

  G4double efinal = aR->GetEnergyChange();
  if(efinal < 0.0) { efinal = 0.0; }

  // check status of primary
  if(aR->GetStatusChange() == stopAndKill) {
    theTotalResult->ProposeTrackStatus(fStopAndKill);
    theTotalResult->ProposeEnergy( 0.0 );

    // check its final energy
  } else if(0.0 == efinal) {
    theTotalResult->ProposeEnergy( 0.0 );
    if(aT.GetParticleDefinition()->GetProcessManager()
       ->GetAtRestProcessVector()->size() > 0)
         { theTotalResult->ProposeTrackStatus(fStopButAlive); }
    else { theTotalResult->ProposeTrackStatus(fStopAndKill); }

    // primary is not killed apply rotation and Lorentz transformation
  } else  {
    theTotalResult->ProposeTrackStatus(fAlive);
    G4double mass = aT.GetParticleDefinition()->GetPDGMass();
    G4double newE = efinal + mass;
    G4double newP = std::sqrt(efinal*(efinal + 2*mass));
    G4ThreeVector newPV = newP*aR->GetMomentumChange();
    G4LorentzVector newP4(newE, newPV);
    newP4.rotate(rotation, it);
    newP4 *= aR->GetTrafoToLab();
    theTotalResult->ProposeMomentumDirection(newP4.vect().unit());
    newE = newP4.e() - mass;
    if(G4HadronicProcess_debug_flag && newE <= 0.0) {
      G4ExceptionDescription ed;
      DumpState(aT,"Primary has zero energy after interaction",ed);
      G4Exception("G4HadronicProcess::FillResults", "had011", JustWarning, ed);
    }
    if(newE < 0.0) { newE = 0.0; }
    theTotalResult->ProposeEnergy( newE );
  }
  //G4cout << "FillResult: Efinal= " << efinal << " status= " 
  //	 << theTotalResult->GetTrackStatus() 
  //	 << "  fKill= " << fStopAndKill << G4endl;

  // check secondaries: apply rotation and Lorentz transformation
  G4int nSec = aR->GetNumberOfSecondaries();
  // Polarised nucleon scattering in hydro-carbon material
  // Derek G's comment
  // Loop through all produced particles and find the one with
  // the highest kinetic energy which shall be identified as the original
  // particle allowing for charge exchange.
  // the initial particle is killed unless 
  // "Keep initial projectile" is displayed
  G4double phiPol = 0.0;    // phi modification angle
  if( IsOrganic(&aT) ){     // does tracking material contain H,C?
    if( IsNucleon( aT.GetDynamicParticle() ) ){
      G4double highKE = 0.;
      G4int ip = 0;
      for(G4int i=0; i<nSec; i++){
	G4DynamicParticle* secDP = aR->GetSecondary(i)->GetParticle();
	if( IsNucleon(secDP) )
	  if(secDP->GetKineticEnergy()>highKE)
	    { highKE = secDP->GetKineticEnergy(); ip=i;}
      }
      // Recalculate phi angle, and return the rotation needed to get it there 
      phiPol=
	fPolRot->GetPolarisedRotation(aR->GetSecondary(ip)->GetParticle()->
				      GetMomentum(), aT.GetDynamicParticle());
    }
  }
  theTotalResult->SetNumberOfSecondaries(nSec);
  G4double weight = aT.GetWeight();

  if (nSec > 0) {
    G4double time0 = aT.GetGlobalTime();
    for (G4int i = 0; i < nSec; ++i) {
      G4LorentzVector theM = aR->GetSecondary(i)->GetParticle()->Get4Momentum();
      // check is there is azimuthal modulation
      if(!phiPol) theM.rotate(rotation, it);
      else theM.rotate(phiPol, it);
      theM *= aR->GetTrafoToLab();
      aR->GetSecondary(i)->GetParticle()->Set4Momentum(theM);
      // time of interaction starts from zero
      G4double time = aR->GetSecondary(i)->GetTime();
      if (time < 0.0) { time = 0.0; }

      // take into account global time
      time += time0;

      G4Track* track = new G4Track(aR->GetSecondary(i)->GetParticle(),
                                   time, aT.GetPosition());
      track->SetCreatorModelIndex(aR->GetSecondary(i)->GetCreatorModelType());
      G4double newWeight = weight*aR->GetSecondary(i)->GetWeight();
	// G4cout << "#### ParticleDebug "
	// <<GetProcessName()<<" "
	//<<aR->GetSecondary(i)->GetParticle()->GetDefinition()->GetParticleName()<<" "
	// <<aScaleFactor<<" "
	// <<XBiasSurvivalProbability()<<" "
	// <<XBiasSecondaryWeight()<<" "
	// <<aT.GetWeight()<<" "
	// <<aR->GetSecondary(i)->GetWeight()<<" "
	// <<aR->GetSecondary(i)->GetParticle()->Get4Momentum()<<" "
	// <<G4endl;
      track->SetWeight(newWeight);
      track->SetTouchableHandle(aT.GetTouchableHandle());
      theTotalResult->AddSecondary(track);
      if (G4HadronicProcess_debug_flag) {
        G4double e = track->GetKineticEnergy();
        if (e <= 0.0) {
          G4ExceptionDescription ed;
          DumpState(aT,"Secondary has zero energy",ed);
          ed << "Secondary " << track->GetDefinition()->GetParticleName()
             << G4endl;
          G4Exception("G4HadronicProcess::FillResults", "had011", 
		      JustWarning,ed);
        }
      }
    }
  }
  aR->Clear();
}

/* end of file */

//----------------------------------------------------------------------------
G4VParticleChange* PolHadronicProcess::PostStepDoIt(const G4Track& aTrack, const G4Step&)
{
  //G4cout << "PostStepDoIt " << aTrack.GetDefinition()->GetParticleName()
  //	 << " Ekin= " << aTrack.GetKineticEnergy() << G4endl;
  // if primary is not Alive then do nothing
  theTotalResult->Clear();
  theTotalResult->Initialize(aTrack);
  theTotalResult->ProposeWeight(aTrack.GetWeight());
  if(aTrack.GetTrackStatus() != fAlive) { return theTotalResult; }

  // Find cross section at end of step and check if <= 0
  //
  const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
  G4Material* aMaterial = aTrack.GetMaterial();

  G4Element* anElement = 0;
  try
  {
     anElement = theCrossSectionDataStore->SampleZandA(aParticle,
						       aMaterial,
						       targetNucleus);
  }
  catch(G4HadronicException & aR)
  {
    G4ExceptionDescription ed;
    aR.Report(ed);
    DumpState(aTrack,"SampleZandA",ed);
    ed << " PostStepDoIt failed on element selection" << G4endl;
    G4Exception("G4HadronicProcess::PostStepDoIt", "had003", FatalException,
		ed);
  }

  // check only for charged particles
  if(aParticle->GetDefinition()->GetPDGCharge() != 0.0) {
    if (GetElementCrossSection(aParticle, anElement, aMaterial) <= 0.0) {
      // No interaction
      return theTotalResult;
    }    
  }

  // Next check for illegal track status
  //
  if (aTrack.GetTrackStatus() != fAlive && 
      aTrack.GetTrackStatus() != fSuspend) {
    if (aTrack.GetTrackStatus() == fStopAndKill ||
        aTrack.GetTrackStatus() == fKillTrackAndSecondaries ||
        aTrack.GetTrackStatus() == fPostponeToNextEvent) {
      G4ExceptionDescription ed;
      ed << "G4HadronicProcess: track in unusable state - "
	 << aTrack.GetTrackStatus() << G4endl;
      ed << "G4HadronicProcess: returning unchanged track " << G4endl;
      DumpState(aTrack,"PostStepDoIt",ed);
      G4Exception("G4HadronicProcess::PostStepDoIt", "had004", JustWarning, ed);
    }
    // No warning for fStopButAlive which is a legal status here
    return theTotalResult;
  }

  // Initialize the hadronic projectile from the track
  thePro.Initialise(aTrack);

  try
  {
    theInteraction =
      ChooseHadronicInteraction( thePro, targetNucleus, aMaterial, anElement );
  }
  catch(G4HadronicException & aE)
  {
    G4ExceptionDescription ed;
    aE.Report(ed);
    ed << "Target element "<<anElement->GetName()<<"  Z= "
       << targetNucleus.GetZ_asInt() << "  A= "
       << targetNucleus.GetA_asInt() << G4endl;
    DumpState(aTrack,"ChooseHadronicInteraction",ed);
    ed << " No HadronicInteraction found out" << G4endl;
    G4Exception("G4HadronicProcess::PostStepDoIt", "had005", FatalException,
		ed);
  }

  G4HadFinalState* result = 0;
  G4int reentryCount = 0;

  do
  {
    try
    {
      // Save random engine if requested for debugging
      if (G4Hadronic_Random_File) {
         CLHEP::HepRandom::saveEngineStatus(G4Hadronic_Random_File);
      }
      // Call the interaction
      result = theInteraction->ApplyYourself( thePro, targetNucleus);
      ++reentryCount;
    }
    catch(G4HadronicException aR)
    {
      G4ExceptionDescription ed;
      aR.Report(ed);
      ed << "Call for " << theInteraction->GetModelName() << G4endl;
      ed << "Target element "<<anElement->GetName()<<"  Z= "
	 << targetNucleus.GetZ_asInt()
	 << "  A= " << targetNucleus.GetA_asInt() << G4endl;
      DumpState(aTrack,"ApplyYourself",ed);
      ed << " ApplyYourself failed" << G4endl;
      G4Exception("G4HadronicProcess::PostStepDoIt", "had006", FatalException,
		  ed);
    }

    // Check the result for catastrophic energy non-conservation
    CheckResult(thePro, targetNucleus, result);

    if(reentryCount>100) {
      G4ExceptionDescription ed;
      ed << "Call for " << theInteraction->GetModelName() << G4endl;
      ed << "Target element "<<anElement->GetName()<<"  Z= "
	 << targetNucleus.GetZ_asInt()
	 << "  A= " << targetNucleus.GetA_asInt() << G4endl;
      DumpState(aTrack,"ApplyYourself",ed);
      ed << " ApplyYourself does not completed after 100 attempts" << G4endl;
      G4Exception("G4HadronicProcess::PostStepDoIt", "had006", FatalException,
		  ed);
    }
  }
  while(!result);

  result->SetTrafoToLab(thePro.GetTrafoToLab());

  ClearNumberOfInteractionLengthLeft();

  FillResult(result, aTrack);

  if (epReportLevel != 0) {
    CheckEnergyMomentumConservation(aTrack, targetNucleus);
  }
  //G4cout << "PostStepDoIt done " << G4endl;
  return theTotalResult;
}
//*/

