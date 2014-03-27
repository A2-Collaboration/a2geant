
#include "A2Hit.hh"
#include "G4Color.hh"
#include "G4VisAttributes.hh"

G4Allocator<A2Hit> A2HitAllocator;


A2Hit::A2Hit()
{
  fEdep=0;
  fPos.setRThetaPhi(0,0,0);
  fID=0;
  fTime=0;
}


A2Hit::~A2Hit()
{ ;}


A2Hit::A2Hit(const A2Hit& right)
  :G4VHit()
{
  fEdep=right.fEdep;
  fPos+right.fPos;
  fID=right.fID;
  fTime=right.fTime;
}


const A2Hit& A2Hit::operator=(const A2Hit& right)
{

  fEdep=right.fEdep;
  fPos+right.fPos;
  fID=right.fID;
  fTime=right.fTime;
  return *this;
}


int A2Hit::operator==(const A2Hit& right) const
{
  return 0;
}


void A2Hit::Draw()
{

}


void A2Hit::Print()
{;}












