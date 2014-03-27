
#ifndef A2VisHit_h
#define A2VisHit_h 1

#include "A2Hit.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"


class A2VisHit : public A2Hit
{
public:
  
  A2VisHit();
  ~A2VisHit();
  
  virtual void Draw(G4double thresh,G4String opt);
  virtual void Draw(){;}
  inline void* operator new(size_t);
  inline void  operator delete(void*);

  void Reset();
private:
  
  G4LogicalVolume *fLV; //Logical volume of fired detector
  G4int fCharge;  //charge of the hit
  G4int fOrigID;  //Id of original ancestor track

  G4VisAttributes* fHitPosVisAtt;
  G4VisAttributes* fHitNegVisAtt;
  G4VisAttributes* fHitNeutVisAtt;
  G4VisAttributes* fDefVisAtt;
public:
  
  void SetLogicalVolume(G4LogicalVolume*  lv) {fLV=lv;};
  void SetCharge(G4int ch){fCharge=ch;}
  void SetOrigID(G4int id){fOrigID=id;}
  
};


typedef G4THitsCollection<A2VisHit> A2VisHitsCollection;

extern G4Allocator<A2VisHit> A2VisHitAllocator;


inline void* A2VisHit::operator new(size_t)
{
  void* aHit;
  aHit = (void*) A2VisHitAllocator.MallocSingle();
  return aHit;
}


inline void A2VisHit::operator delete(void* aHit)
{
  A2VisHitAllocator.FreeSingle((A2VisHit*) aHit);
}

#endif
