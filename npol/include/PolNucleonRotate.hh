// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolNucleonRotate
// Azimuthal modulation of polarised nucleon scattering
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA New class separated from hadronic processes
// 15/11/10 JRMA Bug fix theta in A_y, add Dubna-neutron
// 20/11/10 JRMA Save polarisation
//  4/02/12 JRMA Extend range of models and different p and 12C models
 
#ifndef PolNucleonRotate_h
#define PolNucleonRotate_h 1

#include "G4HadronicProcess.hh"

enum { EAyNull, EAyLadygin, EAySpinka, EAyMcNaughton, EAyGlister, EAyAzhgirey, EAySikora};
enum { Epp, Enp, EpC, EnC };
static const G4double mp = 0.938272;  // proton mass
static const G4double mn = 0.939565;  // neutron mass

class PolNucleonRotate
{
public:
    
  PolNucleonRotate( G4double, G4int, G4int = 0 ); 
  //  virtual ~PolNucleonRotate();
private:   
  G4double fDefAyP;         // default value of analysing power, if != 0
  G4int fParmAyP;           // analysing power parametrisation to use
  G4double fDefAyC;         // default value of analysing power, if != 0
  G4int fParmAyC;           // analysing power parametrisation to use
  G4int fN;                 // incident neutron or proton

protected:
  G4double    fScatPhi;  //First scatter phi of event
  G4double    fScatTheta; //First scatter theta of event
  G4double     fScatE; //First scatter energy of event (incoming)
  G4double     fScatEout; //First scatter energy of event (scattered)
  G4double     fScatAy; //First scatter anaysing power
  G4double  fScatInM;  //incident particle mass (proton)(neutron)
  G4double  fScatOutM;  //scattered particle
  G4bool fSaveScat;  //are you saving scatter info
  G4bool fDidScat;   //did this event have a scatter
  G4bool fIsDetFrame;   //is the particle polarisation defined in lab frame?
  G4LorentzVector* fScatVec; //slightly strange, incident energy, scattered angles and mass

public:
  G4double  GetPolarisedRotation(const G4ThreeVector,
  				 const G4DynamicParticle*,
  				 G4bool = true);
  virtual G4double Ay( G4double, G4double );
  // particular parametrisations of Ay
  G4double AyLadygin( G4double, G4double, G4int );  // free np,pp
  G4double AySpinka( G4double, G4double );              // free pp
  G4double AyMcNaughton( G4double, G4double );          // p-12C
  G4double AyGlister( G4double, G4double );             // p-12C
  G4double AyAzhgirey( G4double, G4double );             // p-12C, p-CH
  G4double AySikora( G4double, G4double );             // p-12C
  void SetAy(G4double);
  void SetAyParm(G4int);
  void SetN(G4int n){ fN = n; }
  G4double Get_t( G4double, G4double, G4double, G4double);
 void ResetEvent(){fDidScat=false;fScatE=0;}//Needs to be set at start of event to make sure only new scatters are saved
  //functions to get the scatter kinematics and properties
  void SetSaveScat(G4bool save=true){fSaveScat=save;}
  void SetDetFrame(G4bool save=true){fIsDetFrame=save;}
  G4double GetScatPhi(){return fScatPhi;}
  G4double GetScatTheta(){return fScatTheta;}
  G4double GetScatE(){return fScatE;}
  G4double GetScatEout(){return fScatEout;}
  G4double GetScatAy(){return fScatAy;}
  G4LorentzVector* GetScatVec() {return fScatVec;}
  G4bool DidScat(){return fDidScat;}

  G4int fVerbose;
};

//-----------------------------------------------------------------------------
inline G4double PolNucleonRotate::Ay( G4double Plab, G4double th )
{
  // Polarised Nucleon-Carbon12-Hydrogen scattering analysing power
  //
  G4double pp = Plab/1000;                            // MeV -> GeV
  G4double Ap,fact;
  G4int parm;
  if( fN < EpC ) { parm = fParmAyP; fact = fDefAyP; }
  else { parm = fParmAyC; fact = fDefAyC; }
  switch( parm ){
  case EAyNull:
  default:
    Ap = 1.0;
    break;
  case EAyLadygin:
    Ap = AyLadygin(pp,th,fN);
    break;
  case EAySpinka:
    Ap = AySpinka(pp,th);
    break;
  case EAyMcNaughton:
    Ap = AyMcNaughton(pp,th);
    break;
  case EAyGlister:
    Ap = AyGlister(pp,th);
    break;
  case EAyAzhgirey:
    Ap = AyAzhgirey(pp,th);
    break;
  case EAySikora:
    Ap = AySikora(pp,th);
    break;
  }
  //Ap *= fact;//multiply by default...
  //Ap=1;
  return Ap;
}


#endif
 
