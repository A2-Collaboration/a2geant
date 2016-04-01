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
// $Id: G4HadronElasticPhysics.hh 81758 2014-06-05 08:12:06Z gcosmo $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4HadronElasticPhysics
//
// Author: 23 November 2006 V. Ivanchenko
//
// Modified:
// 29.07.2010 V.Ivanchenko rename this class from G4HadronHElasticPhysics to
//                         G4HadronElasticPhysics, old version of the class
//                         is renamed to G4HadronElasticPhysics93
// 03.06.2011 V.Ivanchenko change design - added access to pointers of the
//                         neutron process and model
//
//----------------------------------------------------------------------------
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
// 21/07/15 JRMA Adapt for G4.10.01

#ifndef PolHadronElasticPhysics_h
#define PolHadronElasticPhysics_h 1

#include "G4HadronElasticPhysics.hh"

class PolHadronElasticProcess;

class PolHadronElasticPhysics : public G4HadronElasticPhysics
{
public: 

  PolHadronElasticPhysics(G4int ver = 0); 

  virtual ~PolHadronElasticPhysics();

  // This method will be invoked in the Construct() method.
  // each physics process will be instantiated and
  // registered to the process manager of each particle type 
  virtual void ConstructProcess();
protected:
  PolHadronElasticProcess* nElPolProc;  // for neutrons
  PolHadronElasticProcess* pElPolProc;  // for protons
  G4bool fIsSpinPrecN;
  G4double fmuP;
  G4double fmuN;
public:
  PolHadronElasticProcess* GetnElPolProc(){ return nElPolProc; }
  PolHadronElasticProcess* GetpElPolProc(){ return pElPolProc; }
  void SetSpinPrecN( G4double muP, G4double muN )
  { fIsSpinPrecN = true; fmuP = muP; fmuN = muN; }
};
#endif








