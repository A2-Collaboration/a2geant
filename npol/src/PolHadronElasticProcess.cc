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
// $Id: G4HadronElasticProcess.cc 86393 2014-11-10 16:34:27Z gcosmo $
//
// Geant4 Hadron Elastic Scattering Process 
// 
// Created 26 July 2012 V.Ivanchenko from G4WHadronElasticProcess
//  
// Modified:
// 14-Sep-12 M.Kelsey -- Pass subType code to base ctor
//
// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolWHadronElasticPhysics
// Hadron elastic scattering process with option to have polarised
// nucleon scattering
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA new "Pol" classes derived from 4.9.3 G4Had... classes
//  6/09/11 JRMA new "Pol" classes derived from 4.9.4 G4WHadronElasticProcess
//  6/02/12 JRMA Only call PolNucleonRotate if iz = 1, ie N-N scattering
// 21/07/15 JRMA Apapt for G4.10.01
// 15/11/15 DGLA class for elastic np scatter (inc. quasi free)

#include <iostream>
#include <typeinfo>

#include "PolHadronElasticProcess.hh"
#include "G4SystemOfUnits.hh"
#include "G4Nucleus.hh"
#include "G4ProcessManager.hh"
#include "G4CrossSectionDataStore.hh"
#include "G4HadronElasticDataSet.hh"
#include "G4ProductionCutsTable.hh"
#include "G4HadronicException.hh"
#include "G4HadronicDeprecate.hh"
#include "G4HadronicInteraction.hh"
#include "G4VCrossSectionRatio.hh"

PolHadronElasticProcess::PolHadronElasticProcess(const G4String& pName)
  : G4HadronElasticProcess(pName) 
{
  fPolRot = new PolNucleonRotate(0,0);
  fPolRotNP = new PolNPRotate(0,0,0);
}

PolHadronElasticProcess::~PolHadronElasticProcess()
{}

G4VParticleChange* 
PolHadronElasticProcess::PostStepDoIt(const G4Track& track, 
				     const G4Step& /*step*/)
{
  theTotalResult->Clear();
  theTotalResult->Initialize(track);
  G4double weight = track.GetWeight();
  theTotalResult->ProposeWeight(weight);

  // For elastic scattering, _any_ result is considered an interaction
  ClearNumberOfInteractionLengthLeft();

  G4double kineticEnergy = track.GetKineticEnergy();
  const G4DynamicParticle* dynParticle = track.GetDynamicParticle();
  const G4ParticleDefinition* part = dynParticle->GetDefinition();

  // NOTE:  Very low energy scatters were causing numerical (FPE) errors
  //        in earlier releases; these limits have not been changed since.
  if (kineticEnergy <= lowestEnergy)   return theTotalResult;

  G4Material* material = track.GetMaterial();
  G4Nucleus* targNucleus = GetTargetNucleusPointer();

  // Select element
  G4Element* elm = 0;
  G4int iz;
  try
    {
      elm = GetCrossSectionDataStore()->SampleZandA(dynParticle, material, 
						    *targNucleus);
      iz = (G4int)elm->GetZ();
    }
  catch(G4HadronicException & aR)
    {
      G4ExceptionDescription ed;
      aR.Report(ed);
      DumpState(track,"SampleZandA",ed); 
      ed << " PostStepDoIt failed on element selection" << G4endl;
      G4Exception("G4HadronElasticProcess::PostStepDoIt", "had003", 
		  FatalException, ed);
    }

  // Initialize the hadronic projectile from the track
  //  G4cout << "track " << track.GetDynamicParticle()->Get4Momentum()<<G4endl;
  G4HadProjectile theProj(track);
  G4HadronicInteraction* hadi = 0;
  G4HadFinalState* result = 0;

  if(fDiffraction) {
    G4double ratio = 
      fDiffractionRatio->ComputeRatio(part, kineticEnergy,
				      targNucleus->GetZ_asInt(),
				      targNucleus->GetA_asInt());
    // diffraction is chosen
    if(ratio > 0.0 && G4UniformRand() < ratio) {
      try
	{
	  result = fDiffraction->ApplyYourself(theProj, *targNucleus);
	}
      catch(G4HadronicException aR)
	{
	  G4ExceptionDescription ed;
	  aR.Report(ed);
	  ed << "Call for " << fDiffraction->GetModelName() << G4endl;
	  ed << "Target element "<< elm->GetName()<<"  Z= " 
	     << targNucleus->GetZ_asInt() 
	     << "  A= " << targNucleus->GetA_asInt() << G4endl;
	  DumpState(track,"ApplyYourself",ed);
	  ed << " ApplyYourself failed" << G4endl;
	  G4Exception("G4HadronElasticProcess::PostStepDoIt", "had006", 
		      FatalException, ed);
	}
      // Check the result for catastrophic energy non-conservation
      result = CheckResult(theProj, *targNucleus, result);

      result->SetTrafoToLab(theProj.GetTrafoToLab());
      ClearNumberOfInteractionLengthLeft();

      FillResult(result, track);

      if (epReportLevel != 0) {
	CheckEnergyMomentumConservation(track, *targNucleus);
      }
      return theTotalResult;
    }
  }      

  // ordinary elastic scattering
  try
    {
      hadi = ChooseHadronicInteraction( theProj, *targNucleus, material, elm );
    }
  catch(G4HadronicException & aE)
    {
      G4ExceptionDescription ed;
      aE.Report(ed);
      ed << "Target element "<< elm->GetName()<<"  Z= " 
	 << targNucleus->GetZ_asInt() << "  A= " 
	 << targNucleus->GetA_asInt() << G4endl;
      DumpState(track,"ChooseHadronicInteraction",ed);
      ed << " No HadronicInteraction found out" << G4endl;
      G4Exception("G4HadronElasticProcess::PostStepDoIt", "had005", 
		  FatalException, ed);
    }

  size_t idx = track.GetMaterialCutsCouple()->GetIndex();
  G4double tcut = (*(G4ProductionCutsTable::GetProductionCutsTable()
		     ->GetEnergyCutsVector(3)))[idx];
  hadi->SetRecoilEnergyThreshold(tcut);

  if(verboseLevel>1) {
    G4cout << "G4HadronElasticProcess::PostStepDoIt for " 
	   << part->GetParticleName()
	   << " in " << material->GetName() 
	   << " Target Z= " << targNucleus->GetZ_asInt() 
	   << " A= " << targNucleus->GetA_asInt() << G4endl; 
  }

  try
    {
      result = hadi->ApplyYourself( theProj, *targNucleus);
    }
  catch(G4HadronicException aR)
    {
      G4ExceptionDescription ed;
      aR.Report(ed);
      ed << "Call for " << hadi->GetModelName() << G4endl;
      ed << "Target element "<< elm->GetName()<<"  Z= " 
	 << targNucleus->GetZ_asInt() 
	 << "  A= " << targNucleus->GetA_asInt() << G4endl;
      DumpState(track,"ApplyYourself",ed);
      ed << " ApplyYourself failed" << G4endl;
      G4Exception("G4HadronElasticProcess::PostStepDoIt", "had006", 
		  FatalException, ed);
    }

  // Check the result for catastrophic energy non-conservation
  // cannot be applied because is not guranteed that recoil 
  // nucleus is created
  // result = CheckResult(theProj, targNucleus, result);

  // directions
  G4ThreeVector indir = track.GetMomentumDirection();
  G4double phi = CLHEP::twopi*G4UniformRand();
  G4ThreeVector it(0., 0., 1.);
  G4ThreeVector outdir = result->GetMomentumChange();
  // Pol extra bit, for Nucleons and H,C materials only
  // Sample modulated azimuthal distribution
  // polarisation needs to be defined in the proton frame
  // i.e if we have it in the lab must do Pol.rotateUz(-indir)
  G4double phiPol = 0.0;
  if( iz == 1 ){ //elastic scattering on hydrogen
    fPolRotNP->SetReaction(track.GetDynamicParticle()->GetDefinition(),G4Proton::Proton());  //i.e. which nucleon-proton scatter
    fPolRotNP->SetExchange(false); //free elastic always deals with incident particle ofter scatter
    phiPol = fPolRotNP->GetPolarisedRotation(outdir,
					     track.GetDynamicParticle(),true); //true=> true free scatter
  }
  if( iz == 6 ){//elastic scatting on carbon, other elements need to be added 
    phiPol = fPolRot->GetPolarisedRotation(outdir,
					   track.GetDynamicParticle(),true);
  }

  if(verboseLevel>1) {
    G4cout << "Efin= " << result->GetEnergyChange()
	   << " de= " << result->GetLocalEnergyDeposit()
	   << " nsec= " << result->GetNumberOfSecondaries()
	   << " dir= " << outdir
	   << G4endl;
  }
  // energies  
  G4double edep = result->GetLocalEnergyDeposit();
  G4double efinal = result->GetEnergyChange();
  if(efinal < 0.0) { efinal = 0.0; }
  if(edep < 0.0)   { edep = 0.0; }

  // NOTE:  Very low energy scatters were causing numerical (FPE) errors
  //        in earlier releases; these limits have not been changed since.
  if(efinal <= lowestEnergy) {
    edep += efinal;
    efinal = 0.0;
  }

  // primary change
  theTotalResult->ProposeEnergy(efinal);
  G4TrackStatus status = track.GetTrackStatus();
  if(efinal > 0.0) {
    if(phiPol)
      outdir.rotate(phiPol, it);
    else
      outdir.rotate(phi, it);
    outdir.rotateUz(indir);
    theTotalResult->ProposeMomentumDirection(outdir);
  } else {
    if(part->GetProcessManager()->GetAtRestProcessVector()->size() > 0)
         { status = fStopButAlive; }
    else { status = fStopAndKill; }
    theTotalResult->ProposeTrackStatus(status);
  }

  //G4cout << "Efinal= " << efinal << "  TrackStatus= " << status << G4endl;

  theTotalResult->SetNumberOfSecondaries(0);

  // recoil
  if(result->GetNumberOfSecondaries() > 0) {
     G4DynamicParticle* p = result->GetSecondary(0)->GetParticle();

    if(p->GetKineticEnergy() > tcut) {
      theTotalResult->SetNumberOfSecondaries(1);
      G4ThreeVector pdir = p->GetMomentumDirection();
      //!! is not needed for models inheriting G4HadronElastic
      if(phiPol)pdir.rotate(phiPol, it);
      else pdir.rotate(phi, it);
      pdir.rotateUz(indir);
      p->SetMomentumDirection(pdir);

      // in elastic scattering time and weight are not changed
      G4Track* t = new G4Track(p, track.GetGlobalTime(), 
			       track.GetPosition());
      t->SetWeight(weight);
      t->SetTouchableHandle(track.GetTouchableHandle());
      theTotalResult->AddSecondary(t);

    } else {
      edep += p->GetKineticEnergy();
      delete p;
    }
  }
  theTotalResult->ProposeLocalEnergyDeposit(edep);
  theTotalResult->ProposeNonIonizingEnergyDeposit(edep);
  result->Clear();

  return theTotalResult;
}

