
#include "A2VisHit.hh"
#include "G4Color.hh"
#include "G4VisAttributes.hh"

G4Allocator<A2VisHit> A2VisHitAllocator;

A2VisHit::A2VisHit()
{
  fEdep=0;
  fPos.setRThetaPhi(0,0,0);
  fID=0;
  fTime=0;
  fLV=NULL;
  fCharge=0;

  fHitPosVisAtt=NULL;
  fHitNegVisAtt=NULL;
  fHitNeutVisAtt=NULL;
  fDefVisAtt=NULL;
}


A2VisHit::~A2VisHit()
{ 
  Reset();
;}
void A2VisHit::Reset()
{ 
  
  fDefVisAtt=new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  fLV->SetVisAttributes(fDefVisAtt);

  //delete fHitPosVisAtt;
  //delete fHitNegVisAtt;
  //delete fHitNeutVisAtt;
  //delete fDefVisAtt;
;}



void A2VisHit::Draw(G4double thresh,G4String opt)
{
  if(fEdep/MeV<thresh)return;
  //opt 0 - charge of the first particle to deposit energy
  if(opt=="charge"){
    fHitPosVisAtt=new G4VisAttributes(G4Colour(0.1,0.1,1.0));
    fHitNegVisAtt=new G4VisAttributes(G4Colour(0.1,1,0.1));
    fHitNeutVisAtt=new G4VisAttributes(G4Colour(0.1,1,0.1));
    if(fCharge>0)fLV->SetVisAttributes(fHitPosVisAtt);
    if(fCharge<0)fLV->SetVisAttributes(fHitNegVisAtt);
    if(fCharge==0)fLV->SetVisAttributes(fHitNeutVisAtt);
  }
  //opt > 0 strength of the hit, opt gives the max energy for shading
  //i.e not shaded above opt MeV but full colour
  if(opt=="depth"){
    //    G4double frac=fEdep/MeV/opt;
    G4double frac=(fPos.mag()-20*cm)/cm/50;
    if(frac>1)frac=1;
    fHitPosVisAtt=new G4VisAttributes(G4Colour(1,(1-frac)*0.8,(1-frac)*0.8));
    fLV->SetVisAttributes(fHitPosVisAtt);
  }
  //i.e not shaded above opt MeV but full colour
  if(opt=="edep"){
    G4double frac=fEdep/MeV/70;   
    if(frac>1)frac=1;
    fHitPosVisAtt=new G4VisAttributes(G4Colour((1-frac)*0.8,(1-frac)*0.8,1));
    fLV->SetVisAttributes(fHitPosVisAtt);
  }
  if(opt=="time"){
    G4double frac=fTime/10/ns;
    
    G4cout<<"FRAC "<<frac<<G4endl;
    if(frac>1)frac=1;
    fHitPosVisAtt=new G4VisAttributes(G4Colour((frac)*0.8,1,(frac)*0.8));
    fLV->SetVisAttributes(fHitPosVisAtt);
  }
}



