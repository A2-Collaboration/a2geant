// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolHadronicProcess
// Hadonic process base with polarised nucleon scattering options
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA new "Pol" classes derived from 4.9.3 G4Had... classes

#ifndef PolHadronicProcess_h
#define PolHadronicProcess_h 1

#include "G4HadronicProcess.hh"
#include "PolNucleonRotate.hh"

class PolHadronicProcess : public G4HadronicProcess
{
public:
  PolHadronicProcess( const G4String &processName = "Hadronic", 
		     G4ProcessType   aType = fHadronic );    
  //virtual ~PolHadronicProcess();
protected:   
  virtual void FillTotalResult(G4HadFinalState * aR, const G4Track & aT);
  G4double fDefAy;         // default value of analysing power, if != 0
  G4int fParmAy;           // analysing power parametrisation to use
  PolNucleonRotate* fPolRot;
public:
  G4bool IsOrganic( const G4Track* );
  G4bool IsNucleon( const G4DynamicParticle* );
  void SetAy(G4double ay){ fPolRot->SetAy(ay); }
  void SetAyParm(G4int parm){ fPolRot->SetAyParm(parm); }
  PolNucleonRotate* GetPolRot(){ return fPolRot; }
  virtual PolNucleonRotate* GetPolRotEvent(){ 
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
  if ( (aDP->GetDefinition()==G4Proton::Proton()) ||
       (aDP->GetDefinition()==G4Neutron::Neutron()) ) {
    return true;
  } else {
    return false;
  }
}

#endif
 
