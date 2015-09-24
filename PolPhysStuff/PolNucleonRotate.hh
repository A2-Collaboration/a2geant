// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolNucleonRotate
// Azimuthal modulation of polarised nucleon scattering
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA New class separated from hadronic processes
// 15/11/10 JRMA Bug fix theta in A_y, add Dubna-neutron
// August 15, 2012 Add Mark Sikora analyzing power parameterization 

#ifndef PolNucleonRotate_h
#define PolNucleonRotate_h 1

#include "G4HadronicProcess.hh"
#include "G4LorentzVector.hh"

enum {EAyNull, EAyMcNaughton, EAyJLab, EAySikora, EAyDubna, EAyDubnaNeutron};

class PolNucleonRotate
{
public:
    
  PolNucleonRotate( G4double, G4int, G4int = 0 ); 
  virtual ~PolNucleonRotate(){if(fScatVec) delete fScatVec;}
protected:   
  G4double fDefAy;         // default value of analysing power, if != 0
  G4int fParmAy;           // analysing power parametrisation to use

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
  G4double  GetPolarisedRotation(const G4DynamicParticle*,
  				 const G4DynamicParticle*,
  				 G4bool = true);
  virtual G4double Ay( G4double, G4double );
  void SetAy(G4double ay){ fDefAy = ay; }
  void SetAyParm(G4int parm){ fParmAy = parm; }

  void SetSaveScat(G4bool save=true){fSaveScat=save;}
  void SetDetFrame(G4bool save=true){fIsDetFrame=save;}
  G4double GetScatPhi(){return fScatPhi;}
  G4double GetScatTheta(){return fScatTheta;}
  G4double GetScatE(){return fScatE;}
  G4double GetScatEout(){return fScatEout;}
  G4double GetScatAy(){return fScatAy;}
  G4LorentzVector* GetScatVec() {return fScatVec;}
  G4bool DidScat(){return fDidScat;}
  void ResetEvent(){fDidScat=false;fScatE=0;}//Needs to be set at start of event to make
                                    //sure only new scatters are saved
  //void ToggleDidScat(G4bool truth_value) {fDidScat = truth_value;};//MHS
  G4int fVerbose;
};

//-----------------------------------------------------------------------------
inline G4double PolNucleonRotate::Ay( G4double pp, G4double thp )
{
  // Polarised Nucleon-Carbon12-Hydrogen scattering analysing power
  //
  // M.W. McNaughton et al, NIM A241 (1985), 435-440
  // McNaughton Parametrisation kinetic energy < 450 MeV
  static const G4double al[]={5.3346,-5.5361,2.8353,61.915,-145.54};
  static const G4double bl[]={-12.774,-68.339,1333.5,-3713.5,3738.3};
  static const G4double cl[]={1095.3,949.5,-28012.0,96833.0,-118830.0};
  // McNaughton Parametrisation kinetic energy > 450 MeV
  static const G4double ah[]={1.6575,1.3855,9.7700,-149.27};
  static const G4double bh[]={-16.346,152.53,139.16,-3231.1};
  static const G4double ch[]={1052.2,-3210.8,-2293.3,60327.0};
  static const G4double dh[]={0.13887,-0.19266,-0.45643,8.1528};
  // JLab Parametrisation
  // J.Glister et al, arXiv:0904.1493v1, 9 Apr 2009
  static const G4double aj[] = {4.0441, 19.313, 119.27, 439.75, 9644.7};
  static const G4double bj[] = {6.4212, 111.99, -5847.9, -21750, 973130};
  static const G4double cj[] = {42.741, -8639.4, 87129, 813590, -21720000};
  static const G4double dj[] = {5826, 247010, 3376800, -11201000, -19356000};
  // Dubna
  // L.S.Azhgirey et al, NIM A538 (2005), 431-441
  // p-graphite
  static const G4double a1[] = {3.48, -12.14, 18.9, -14.2, 4.10};
  // p-CH
  static const G4double a2[] = {3.02, -7.33,  6.17, -1.74, 0.0};
  // Sikora Parameterization, see Sikora thesis pages 75-76
  static const G4double aSik[] = {2.20904, 1.43983, -9.72434, -35.2466, -21.2429};
  static const G4double bSik[] = {4.9952, -7.97064, 88.3627, 296.751, 195.305};
  static const G4double cSik[] = {37.1864, -200.439, 1160.7, -3119.27, -3920.79};
  static const G4double dSik[] = {78.9808, -166.891, 1181.26, -3212.6, 4446.96};
 
  //  thp = thp * 0.017453293;                        // degree to radian
  G4double thp_deg = thp * 180.0 / 3.1415;//thp in degrees
  pp = pp/1000;                                   // MeV -> GeV
  G4double r = pp*sin(thp);                       // transverse momentum
  G4double ppr, a,b,c,d;
  G4double pr;
  G4double Ac;
  //G4cout<<fParmAy<<" "<<fDefAy<<" "<<pp<<" "<<thp<<" "<<sin(thp)<<G4endl;
  switch( fParmAy ){
  case EAyNull:
  default:
    Ac = fDefAy;
    break;
  case EAyMcNaughton:
    // McNaughton T nucleon < 450 MeV, momentum < 1.023 GeV
    if( pp < 0.4 ) Ac = fDefAy; // less 100 MeV undefined
    else if( pp < 1.023 ){
      ppr = pp - 0.7;
      a = al[0]; b = bl[0]; c = cl[0]; pr = ppr;
      for(G4int i=1; i<5; i++){
	a += al[i]*pr;
	b += bl[i]*pr;
	c += cl[i]*pr;
	pr = pr*ppr;
      }
      Ac = a*r/(1 + b*r*r + c*r*r*r*r);
    }
    // McNaughton 450 MeV < T nucleon < 750 MeV, momentum < 1.404 GeV
    else if( pp < 1.404 ){
      ppr = pp - 1.2;
      a = ah[0]; b = bh[0]; c = ch[0]; d = dh[0]; pr = ppr;
      for(G4int i=1; i<4; i++){
	a += ah[i]*pr;
	b += bh[i]*pr;
	c += ch[i]*pr;
	d += dh[i]*pr;
	pr = pr*ppr;
      }
      Ac = a*r/(1 + b*r*r + c*r*r*r*r) + d*pp*sin(5*thp);
    }
    else Ac = fDefAy;    // > 750 MeV undefined
    break;
  case EAyJLab:
    ppr = pp - 0.55;
    // JLab Hall-A low energy
    a = aj[0]; b = bj[0]; c = cj[0]; d = dj[0]; pr = ppr;
    for(G4int i=1; i<5; i++){
      a += aj[i]*pr;
      b += bj[i]*pr;
      c += cj[i]*pr;
      d += dj[i]*pr;
      pr = pr*ppr;
    }
    Ac = a*r/(1 + b*r*r + c*r*r*r*r + d*r*r*r*r*r*r);
    break;
  case EAySikora:
    // Sikora parameterization
    if ( pp < 0.4) Ac = 0.2; //if KE<80 MeV = 0.4 GeV/c, Apow = 0.2
    else if ( pp < 1.46 && thp_deg > 2 && thp_deg <= 40) { //parameterization valid for 80 MeV < KE < 800 MeV, 2 deg < theta < 40 deg
      ppr = pp - 1.67672;
      a = aSik[0]; b = bSik[0]; c = cSik[0]; d = dSik[0]; pr = ppr;
      for (G4int i = 1; i < 5; ++i) {
      	a += aSik[i]*pr;
      	b += bSik[i]*pr;
      	c += cSik[i]*pr;
      	d += dSik[i]*pr;
      	pr = pr*ppr;
      }
      Ac = ( a*r/(1 + b*r*r + c*r*r*r*r + d*r*r*r*r*r*r) ) + 0.108356*pp*sin(5*thp);
      //G4cout<<"sikora " <<thp_deg<<"   "<<Ac<<G4endl;
    }
    else if ( pp > 1.46 && thp_deg > 5 && thp_deg < 20) Ac = 0.22; //KE > 800 MeV, 5 deg < theta < 20 deg
    else Ac = fDefAy;
    //G4cout<<"sikora    momentum="<<pp<<" GeV  "<<" KE=  "<<1000*(-0.938+sqrt(0.938*0.938+pp*pp))<<" MeV   Theta= "<<thp_deg<<"   Apow= "<<Ac<<G4endl;
    break;
  case EAyDubna:
  case EAyDubnaNeutron:
    // Dubna/Hall-A high energy
    // Ax = p-C, Aw = p-CH
    G4double Ax = a1[0]*r;
    G4double Aw = a2[0]*r;
    pr = r;
    for(G4int i=1; i<5; i++)
      { pr = pr*r; Ax = Ax + a1[i]*pr; Aw = Aw + a2[i]*pr; }
    Ax = Ax/pp; Aw = Aw/pp;
    Ac = Aw;
    //  printf("%f %f   %f %f %f %f\n",pp,r,Ac,Aj,Ax,Aw);
    if( fParmAy == EAyDubnaNeutron ) Ac = Ac*0.4;   // assume n-n smaller
    break;
  }
  //G4cout<<Ac<<G4endl;
  return Ac;
}

#endif
 
