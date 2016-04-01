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
//
// $Id: G4HadronElasticProcess.hh 86393 2014-11-10 16:34:27Z gcosmo $
//
// Geant4 Hadron Elastic Scattering Process -- header file
// 
// Created 26 July 2012 V.Ivanchenko from G4WHadronElasticProcess
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
//

// Class Description
// General process for hadron nuclear elastic scattering  
// Class Description - End

#ifndef PolHadronElasticProcess_h
#define PolHadronElasticProcess_h 1
 
//#include "globals.hh"
#include "G4HadronElasticProcess.hh"
#include "PolNucleonRotate.hh"
#include "PolNPRotate.hh"

class PolHadronElasticProcess : public G4HadronElasticProcess
{
public:

  PolHadronElasticProcess(const G4String& procName = "hadElastic");

  virtual ~PolHadronElasticProcess();
 
  virtual G4VParticleChange* PostStepDoIt(const G4Track& aTrack, 
					  const G4Step& aStep);
  PolNucleonRotate* GetPolRot(){ return fPolRot; }
  PolNucleonRotate* GetPolRotNP(){ return fPolRotNP; }
  virtual PolNucleonRotate* GetPolRotEvent(){ //return whicheveer rotation was used in this event
    if(fPolRot->DidScat())return fPolRot; 
    else if(fPolRotNP->DidScat())return fPolRotNP; 
    else return NULL;
  }
protected:
  PolNucleonRotate* fPolRot;
  PolNPRotate* fPolRotNP;
  void SetAy(G4double ay){ fPolRot->SetAy(ay); }
  void SetAyParm(G4int parm){ fPolRot->SetAyParm(parm); }

};

#endif
