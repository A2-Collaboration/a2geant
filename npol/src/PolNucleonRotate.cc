// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolNucleonRotate
// Azimuthal modulation of polarised nucleon scattering
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA New class separated from hadronic processes
// 15/11/10 JRMA Bug fix theta in A_y, add Dubna-neutron
// 20/11/10 JRMA Save polarisation
//  4/02/12 JRMA Extend range of models and different p/12C models
 
#include "PolNucleonRotate.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include <typeinfo>
G4ThreeVector* PolNN=0;
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
PolNucleonRotate::PolNucleonRotate( G4double ay, G4int pay, G4int verbose )
{
  // verbose is by default 0 (ie not verbose)
  fDefAyP = ay;
  fParmAyP = pay;
  fDefAyC = 0;
  fParmAyC = 0;
  fN = 0;
  fVerbose = verbose;
}

//-----------------------------------------------------------------------------
G4double PolNucleonRotate::
GetPolarisedRotation(const G4ThreeVector newP,
		     const G4DynamicParticle *oldPart, G4bool IsDiff){
  // Then modify the azimuthal distribution using the chosen parameterisation
  // of the N + CH analysing power.
  // Already in frame defined by incident particle until GetTrafoToLab later
  //
  G4ThreeVector Pol(oldPart->GetPolarization());
  G4double Pt=sqrt(Pol.x()*Pol.x()+Pol.y()*Pol.y()); //transverse polarisation
  if( !Pt ) return 0.0;
  //sin(phi) asymmetry with phi0 being along the transverse polarisation dir.
  //*PolNN = Pol;
  G4double phi0 = Pol.phi();
 
  G4double ay = Ay(oldPart->GetTotalMomentum(),newP.theta());
  G4double newphi = pi*G4UniformRand();
  if( 2*G4UniformRand() > (1+ay*Pt*sin(newphi)) ) newphi=-pi+newphi; 
  newphi+=phi0;
  //  if( !IsDiff ) return newphi;//for elastic scattering
  G4double phiP = newP.phi();
  //  G4double delta_phi = newphi - newP.phi();
  G4double delta_phi = newphi - phiP;
  
  if(fVerbose > 1){
    G4cout<<" HadronicProcess Pt="<<Pt<<" phi0="<<phi0/deg<<" "
	  <<newP.theta()/deg<<" "
	  <<oldPart->GetKineticEnergy()<<" Ay="<<ay<<" phi="
	  <<newphi/deg<<" delphi="<<delta_phi/deg<<"parm "<<fParmAyP<<G4endl;
  }
  return delta_phi;
}

//-----------------------------------------------------------------------------
void PolNucleonRotate::SetAy( G4double ay )
{
  // Default constant value of Ay (if no Ay model)
  switch(fN){
  case Epp:
  case Enp:
  default:
    fDefAyP = ay;
    break;
  case EpC:
  case EnC:
    fDefAyC = ay;
    break;
  }
}

//-----------------------------------------------------------------------------
void PolNucleonRotate::SetAyParm( G4int parm )
{
  // specify parametrised Ay model
  switch(fN){
  case Epp:
  case Enp:
  default:
    fParmAyP = parm;
    break;
  case EpC:
  case EnC:
    fParmAyC = parm;;
    break;
  }
  fParmAyC=parm;
  fParmAyP=parm;
}

//----------------------------------------------------------------------------
G4double PolNucleonRotate::AyLadygin( G4double Plab, G4double th,
					     G4int sw ){
  // V.Ladygin, Dubna Report E13-99-123 (1999)
  // Plab incident lab momentum in GeV/c.
  // th lab scattering angle in radian
  // 8 parameter fit in Plab and t
  // A(Plab,t) = a*sqrt(-t)*(1 + bx*t + cx*t*t);
  //
  G4double D,E,F,G,H,J,K,L,xm1,xm2;
  switch( sw ){
  case Epp:
  case EpC:
    // proton-proton
    D=2.26; E=0.002; F=0.011; G=1.29; H=0.281; J=0.0131; K=0.175; L=0.496;
    xm1 = xm2 = mp;
    break;
  case Enp:
  case EnC:
    // neutron-proton
    D=3.74; E=-0.687; F=0.0533; G=1.87; H=-0.658; J=0.485; K=0.213; L=0.387;
    xm1 = mn; xm2 = mp;
    break;
  }
  G4double ax = (D + E + E*Plab + F*Plab*Plab)/Plab;
  G4double l = log(Plab);
  G4double bx = G + H*l + J*l*l;
  G4double cx = K + L*l;
  G4double t = Get_t(Plab,th,xm1,xm2);
  return ax*sqrt(-t)*(1 + bx*t + cx*t*t);
}

//----------------------------------------------------------------------------
G4double PolNucleonRotate::AySpinka( G4double Plab, G4double th ){
  // Argonne ZGS Ay free p-p
  // H.Spinka et al., NIM 211(1983),239-261
  // Plab incident lab momentum in GeV/c.
  // th lab scattering angle in radian
  G4double app[] = { 2.454, 0.06081, -0.0001540 };
  G4double bpp[] = { 1.963, -0.3544, 0.1966 };
  G4double cpp[] = { 0.6814, 0.300 };
  //
  G4double t,as,bs,cs;
  t = Get_t(Plab,th,mp,mp);                  // Get 4-momentum transfer 
  G4double lpl = log(Plab);
  as = (app[0] + app[1]*(1+Plab) + app[2]*Plab*Plab)/Plab;
  bs = bpp[0] + bpp[1]*lpl + bpp[2]*lpl*lpl;
  cs = cpp[0] + cpp[1]*lpl;
  //
  G4double Ap = as*sqrt(-t)*(1 + bs*t + cs*t*t);
  return Ap;

}

//----------------------------------------------------------------------------
G4double PolNucleonRotate::AyMcNaughton(G4double Plab, G4double th)
{
  // Polarised proton-Carbon12 scattering analysing power
  // M.W. McNaughton et al, NIM A241 (1985), 435-440
  // Plab incident lab momentum in GeV/c.
  // th lab scattering angle in radian
  // McNaughton Parametrisation kinetic energy < 450 MeV
  static const G4double al[]={5.3346,-5.5361,2.8353,61.915,-145.54};
  static const G4double bl[]={-12.774,-68.339,1333.5,-3713.5,3738.3};
  static const G4double cl[]={1095.3,949.5,-28012.0,96833.0,-118830.0};
  // McNaughton Parametrisation kinetic energy > 450 MeV
  static const G4double ah[]={1.6575,1.3855,9.7700,-149.27};
  static const G4double bh[]={-16.346,152.53,139.16,-3231.1};
  static const G4double ch[]={1052.2,-3210.8,-2293.3,60327.0};
  static const G4double dh[]={0.13887,-0.19266,-0.45643,8.1528};
  //
  G4double r = Plab*sin(th);            // transverse momentum
  G4double Ap,a,b,c,d,pr,ppr;
  // Tp < 100 MeV undefined
  if( Plab < 0.4 ) Ap = fDefAyC;
  // McNaughton Tp < 450 MeV, Plab < 1.023 GeV
  else if( Plab < 1.023 ){
    ppr = Plab - 0.7; a = al[0]; b = bl[0]; c = cl[0]; pr = ppr;
    for(G4int i=1; i<5; i++){
      a += al[i]*pr;
      b += bl[i]*pr;
      c += cl[i]*pr;
      pr = pr*ppr;
    }
    Ap = a*r/(1 + b*r*r + c*r*r*r*r);
  }
  // McNaughton 450 MeV < Tp < 750 MeV, Plab < 1.404 GeV
  else if( Plab < 1.404 ){
    ppr = Plab - 1.2;
    a = ah[0]; b = bh[0]; c = ch[0]; d = dh[0]; pr = ppr;
    for(G4int i=1; i<4; i++){
      a += ah[i]*pr;
      b += bh[i]*pr;
      c += ch[i]*pr;
      d += dh[i]*pr;
      pr = pr*ppr;
    }
    Ap = a*r/(1 + b*r*r + c*r*r*r*r) + d*Plab*sin(5*th);
  }
  // Tp > 750 MeV undefined
  else Ap = fDefAyC;
  return Ap;
}

//----------------------------------------------------------------------------
G4double PolNucleonRotate::AyGlister(G4double Plab, G4double th)
{  
  // Polarised proton-Carbon12 scattering analysing power
  // JLab Hall-A low energy parametrisation
  // J.Glister et al, arXiv:0904.1493v1, 9 Apr 2009
  // Plab incident lab momentum in GeV/c.
  // th lab scattering angle in radian
  static const G4double aj[] = {4.0441, 19.313, 119.27, 439.75, 9644.7};
  static const G4double bj[] = {6.4212, 111.99, -5847.9, -21750, 973130};
  static const G4double cj[] = {42.741, -8639.4, 87129, 813590, -21720000};
  static const G4double dj[] = {5826, 247010, 3376800, -11201000, -19356000};
  //
  G4double a,b,c,d,pr,ppr,r,Ap;
  ppr = Plab - 0.55;
  r = Plab*sin(th);            // transverse momentum
  a = aj[0]; b = bj[0]; c = cj[0]; d = dj[0]; pr = ppr;
  for(G4int i=1; i<5; i++){
    a += aj[i]*pr;
    b += bj[i]*pr;
    c += cj[i]*pr;
    d += dj[i]*pr;
    pr = pr*ppr;
  }
  Ap = a*r/(1 + b*r*r + c*r*r*r*r + d*r*r*r*r*r*r);
  return Ap;
}

//----------------------------------------------------------------------------
G4double PolNucleonRotate::AySikora(G4double Plab, G4double th)
{  
  // Sikora Parameterization, see Sikora thesis pages 75-76
  static const G4double aSik[] = {2.20904, 1.43983, -9.72434, -35.2466, -21.2429};
  static const G4double bSik[] = {4.9952, -7.97064, 88.3627, 296.751, 195.305};
  static const G4double cSik[] = {37.1864, -200.439, 1160.7, -3119.27, -3920.79};
  static const G4double dSik[] = {78.9808, -166.891, 1181.26, -3212.6, 4446.96};
  //  thp = thp * 0.017453293;                        // degree to radian
  G4double thp_deg = th * 180.0 / 3.1415;//thp in degrees
  Plab = Plab/1000;                                   // MeV -> GeV
  G4double r = Plab*sin(th);                       // transverse momentum
  G4double ppr, a,b,c,d;
  G4double pr;
  G4double Ac;
  if ( Plab < 0.4) Ac = fDefAyC; //if KE<80 MeV = 0.4 GeV/c, Apow = 0.2
  else if ( Plab < 1.46 && thp_deg > 2 && thp_deg <= 40) { //parameterization valid for 80 MeV < KE < 800 MeV, 2 deg < theta < 40 deg
    ppr = Plab - 1.67672;
    a = aSik[0]; b = bSik[0]; c = cSik[0]; d = dSik[0]; pr = ppr;
    for (G4int i = 1; i < 5; ++i) {
      a += aSik[i]*pr;
      b += bSik[i]*pr;
      c += cSik[i]*pr;
      d += dSik[i]*pr;
      pr = pr*ppr;
    }
    Ac = ( a*r/(1 + b*r*r + c*r*r*r*r + d*r*r*r*r*r*r) ) + 0.108356*Plab*sin(5*th);
    //G4cout<<"sikora " <<thp_deg<<"   "<<Ac<<G4endl;
  }
  else if ( Plab > 1.46 && thp_deg > 5 && thp_deg < 20) Ac = 0.22; //KE > 800 MeV, 5 deg < theta < 20 deg
  else Ac = fDefAyC;
  
}
G4double PolNucleonRotate::AyAzhgirey(G4double Plab, G4double th)
{  
  // Polarised proton-Carbon12 scattering analysing power
  // Dubna Parametrisation
  // L.S.Azhgirey et al, NIM A538 (2005), 431-441
  // Plab incident lab momentum in GeV/c.
  // th lab scattering angle in radian
  // p-graphite
  static const G4double a1[] = {3.48, -12.14, 18.9, -14.2, 4.10};
  // p-CH
  //  static const G4double a2[] = {3.02, -7.33,  6.17, -1.74, 0.0};
  G4double Ap,pr,r;
  r = Plab*sin(th);            // transverse momentum
  Ap = a1[0]*r;
  pr = r;
  for(G4int i=1; i<5; i++)
    { pr = pr*r; Ap = Ap + a1[i]*pr; }
  Ap = Ap/Plab;
  return Ap;
}

//----------------------------------------------------------------------------
G4double PolNucleonRotate::
  Get_t(G4double p1, G4double th3, G4double xm1, G4double xm2)
{
  // Calculate Mandlestam t for elastic scattering,
  // particle 1 (mass xm1) incident on particle 2 (mass xm2)
  // given incident lab momentum p1  and scattering angle th3
  G4double e1 = sqrt(p1*p1 + xm1*xm1);
  G4double p = p1;
  G4double E = e1 + xm2;
  G4double M = sqrt(E*E - p*p);
  G4double e3cm = (M*M + xm1*xm1 - xm2*xm2)/(2.0*M);
  G4double p3cm = sqrt( e3cm*e3cm - xm1*xm1 );
  G4double sinth3 = sin(th3);
  G4double costh3 = cos(th3);
  G4double a = (M*M + xm1*xm1 - xm2*xm2)*p*costh3;
  G4double b = 2*E*sqrt(M*M*p3cm*p3cm - xm1*xm1*p*p*sinth3*sinth3);
  G4double c = 2*(M*M + p*p*sinth3*sinth3);
  G4double p3 = (a + b)/c;
  G4double e3 = sqrt(p3*p3 + xm1*xm1);
  G4double e4 = E - e3;
  G4double t = -2*xm2*(e4 - xm2);
  return t;
}
