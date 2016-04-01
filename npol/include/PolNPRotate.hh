// Class PolNucleonRotate
// Azimuthal modulation of polarised nucleon scattering
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA New class separated from hadronic processes
// 15/11/10 JRMA Bug fix theta in A_y, add Dubna-neutron
// 15/11/15 DGLA class for elastic np sctatter (inc. quasi free)
 
#ifndef PolNPRotate_h
#define PolNPRotate_h 1

#include "PolNucleonRotate.hh"
//#include "G4ParticleDefinition.hh"
#include "TFile.h"
#include "TH2.h"
#include "TPaletteAxis.h"
using namespace CLHEP;

enum {EScatPP=1,EScatNN,EScatNP,EScatPN};

class PolNPRotate : public PolNucleonRotate
{
public:
    
  PolNPRotate( G4double, G4int, G4int = 0 ); 
  virtual ~PolNPRotate();
private:   
  TFile* fApFile;  //File containg Free nucleon-p analysing power hists
  TH2F* fAyPPh;  //proton-proton
  TH2F* fAyNPh;  //neutron-proton
  TH2F* fAyh;  //this interaction
  G4int fScatType;
  G4bool fScatFree;
  G4bool fExchange;

public:
  // G4double  GetPolarisedRotation(const G4ThreeVector,
  // 				 const G4DynamicParticle*,
  // 				 G4bool = true);
  virtual G4double Ay( G4double pp, G4double thp );
  void SetReaction(G4ParticleDefinition* inc,G4ParticleDefinition* tar,G4bool free=true);
  void SetExchange(bool ex){fExchange=ex;}//was it a charge exchange reaction
  G4int Type(){return fScatType;}
};

//-----------------------------------------------------------------------------
inline G4double PolNPRotate::Ay( G4double pp, G4double thp )
{
  G4double Ap;
  //SAID observables are for NP scatter, need opposite angles for PN
  //convert momentum to energy
  G4double mass;
  if(fScatType==EScatPP||fScatType==EScatPN) mass=938.272*MeV;
  if(fScatType==EScatNP||fScatType==EScatNN) mass=939.565*MeV;
  pp=sqrt(mass*mass+pp*pp)-mass;

  if(fScatType==EScatPN&&fExchange==false) Ap=-fAyh->GetBinContent(fAyh->FindFixBin(pp/MeV,90-thp/deg)); //SAID is for NP, for PN must switch sign and angle
  else if(fScatType==EScatPN&&fExchange==true) Ap=fAyh->GetBinContent(fAyh->FindFixBin(pp/MeV,thp/deg)); //SAID is for NP, for PN must switch sign and angle and back again
 else if(fScatType==EScatNP&&fExchange==false) Ap=fAyh->GetBinContent(fAyh->FindFixBin(pp/MeV,thp/deg)); //SAID is for NP,
 else if(fScatType==EScatNP&&fExchange==true) Ap=-fAyh->GetBinContent(fAyh->FindFixBin(pp/MeV,90-thp/deg)); //SAID is for NP, switch sign for charge exchange
 else Ap=fAyh->GetBinContent(fAyh->FindFixBin(pp/MeV,thp/deg));
  
  // if(Ap>0) Ap=1;  //maximum asymmetry
  // if(Ap<0) Ap=-1;
  
  if(fScatFree)   {
    //G4cout<<"elastic scattering on H   "<<Ap<<G4endl;
    return Ap;//elastic scattering on H
  }
  //Needs to be determined if these are reasonable
  //if (fScatType == EScatNP || fScatType == EScatPN) Ap*=1.2; //Nuclear Factors
  //if (fScatType == EScatNN || fScatType == EScatPP) Ap*=0.7; //Nuclear Factors
   return Ap;
}
inline void PolNPRotate::SetReaction(G4ParticleDefinition* inc,G4ParticleDefinition* tar,G4bool free)
{
  fScatFree=free;
  if(inc==G4Proton::Proton()&&tar==G4Proton::Proton()){fAyh=fAyPPh;fScatType=EScatPP;}
  else if(inc==G4Neutron::Neutron()&&tar==G4Proton::Proton()){fAyh=fAyNPh;fScatType=EScatNP;}
  else if(inc==G4Neutron::Neutron()&&tar==G4Neutron::Neutron()){fAyh=fAyPPh;fScatType=EScatNN;}//assume NN is same as PP
  else if(inc==G4Proton::Proton()&&tar==G4Neutron::Neutron()){fAyh=fAyNPh;fScatType=EScatPN;}//Need to change, correct hist but theta->90-theta
  else{
    G4cout<<"Warning PolNPRotate::SetReaction( did not find reaction"<<G4endl;
    G4cout<<inc->GetParticleName()<<" + "<<tar->GetParticleName()<<G4endl;
  }
}
#endif
 
