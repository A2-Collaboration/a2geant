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
// $Id: G4HadronicProcess.hh 86448 2014-11-12 09:48:41Z gcosmo $
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
// G4HadronicProcess
//
// This is the top level Hadronic Process class
// The inelastic, elastic, capture, and fission processes
// should derive from this class
//
// original by H.P.Wellisch
// J.L. Chuma, TRIUMF, 10-Mar-1997
// Last modified: 04-Apr-1997
// 19-May-2008 V.Ivanchenko cleanup and added comments
// 05-Jul-2010 V.Ivanchenko cleanup commented lines
// 28-Jul-2012 M.Maire add function GetTargetDefinition() 
// 14-Sep-2012 Inherit from RestDiscrete, use subtype code (now in ctor) to
//		configure base-class
// 28-Sep-2012 M. Kelsey -- Undo inheritance change, keep new ctor
//
// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolHadronicProcess
// Hadonic process base with polarised nucleon scattering options
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA new "Pol" classes derived from 4.9.3 G4Had... classes
// 21/07/15 JRMA adapt for G4.10.01

#ifndef PolHadronicProcess_h
#define PolHadronicProcess_h 1
 
#include "G4HadronicProcess.hh"
#include "PolNucleonRotate.hh"

class PolHadronicProcess : public G4HadronicProcess
{
public:
  PolHadronicProcess(const G4String& processName="Hadronic",
		    G4ProcessType procType=fHadronic);    

  // Preferred signature for subclasses, specifying their subtype here
  PolHadronicProcess(const G4String& processName, 
		    G4HadronicProcessType subType);    
protected:    
  // fill result
  // generic PostStepDoIt recommended for all derived classes
  virtual G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&);  
  virtual void FillResult(G4HadFinalState* aR, const G4Track& aT);
  G4double fDefAy;         // default value of analysing power, if != 0
  G4int fParmAy;           // analysing power parametrisation to use
  PolNucleonRotate* fPolRot;
public:
  virtual G4bool IsOrganic( const G4Track* );
  virtual G4bool IsNucleon( const G4DynamicParticle* );
  virtual void SetAy(G4double ay){ fPolRot->SetAy(ay); }
  virtual void SetAyParm(G4int parm){ fPolRot->SetAyParm(parm); }
  virtual PolNucleonRotate* GetPolRot(){ return fPolRot; }
  virtual PolNucleonRotate* GetPolRotEvent(){ //did a polarised scatter happen this event?
    if(fPolRot->DidScat())return fPolRot; 
    else return NULL;
  }

};

//-----------------------------------------------------------------------------
inline G4bool PolHadronicProcess::IsOrganic( const G4Track* aT )
{
  // Determine if the material currently tracking through
  // contains hydrogen or carbon
  //
  const G4ElementVector* elemVec = aT->GetMaterial()->GetElementVector();
  G4int nelm = aT->GetMaterial()->GetNumberOfElements();
  for( G4int i=0; i<nelm; i++ ){
    G4int z = G4int( (*elemVec)[i]->GetZ() );
    if( (z == 1) || (z == 6) ) { return true; }
  }
  return false;
}
 
//-----------------------------------------------------------------------------
inline G4bool PolHadronicProcess::IsNucleon( const G4DynamicParticle* aDP )
{
  // Is the particle a nucleon
  if( (aDP->GetDefinition()==G4Proton::Proton()) ||
      (aDP->GetDefinition()==G4Neutron::Neutron()) )
    return true;
  else return false;
}
 
#endif
 
