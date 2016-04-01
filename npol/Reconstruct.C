//run with  .L ../Reconstruct.C++; Reconstruct("test.root")

#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include <TH2.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <iostream>
TVector3 ScatteredVector(TVector3 v_inc,TVector3 v_sc);
TF1* ap=new TF1("ap","[0]*(1+[1]*sin(x*TMath::DegToRad()))",-180,180);

TH2F* hSP=new TH2F("ProtThPhi","Scattered proton angles",90,-180,180,90,0,90);
TH2F* hSN=new TH2F("NeutThPhi","Scattered neutron angles",90,-180,180,90,0,90);
TH2F* hSPE=new TH2F("ProtEPhi","Scattered proton E Phi",90,-180,180,90,0,500);
TH2F* hSNE=new TH2F("NeutEPhi","Scattered neutron E Phi",90,-180,180,90,0,500);

void Reconstruct(TString filename){
ap->SetParameters(100,0.5);

  TFile* file=new TFile(filename);

  TTree* tree=(TTree*) file->Get("npol");

  TLorentzVector* inc=0;
  TLorentzVector* neut=0;
  TLorentzVector* prot=0;

  Float_t edep;
  tree->SetBranchAddress("Inc",&inc);
  tree->SetBranchAddress("SN",&neut);
  tree->SetBranchAddress("SP",&prot);
  tree->SetBranchAddress("edep",&edep);

  Float_t thresh=0.5;

  for(Int_t i=0;i<tree->GetEntries();i++){
    tree->GetEntry(i);
    //Need t rotate scattered vector into primed frame
    TVector3 scatN=ScatteredVector(inc->Vect(),neut->Vect());//neutron
    TVector3 scatP=ScatteredVector(inc->Vect(),prot->Vect());//proton
    if(edep<thresh) continue;
    hSN->Fill(scatN.Phi()*TMath::RadToDeg(),scatN.Theta()*TMath::RadToDeg());
    hSP->Fill(scatP.Phi()*TMath::RadToDeg(),scatP.Theta()*TMath::RadToDeg());
    hSNE->Fill(scatN.Phi()*TMath::RadToDeg(),neut->E()-neut->M());
    hSPE->Fill(scatP.Phi()*TMath::RadToDeg(),prot->E()-prot->M());
  }

}
TVector3 ScatteredVector(TVector3 v_inc,TVector3 v_sc){
  TVector3 XLAB(1,0,0);
  TVector3 YLAB(0,1,0);
  TVector3 ZLAB(0,0,1);

  //Define primed frame
  TVector3 Zprime=v_inc.Unit();//Nucleon momentum direction
  TVector3 Yprime=ZLAB.Cross(-v_inc);//BeamXpi momentum or protonXbeam
  Yprime=Yprime.Unit();
  TVector3 Xprime=Yprime.Cross(Zprime);
  Xprime=Xprime.Unit();
  // Xprime.Print();
  // Yprime.Print();
  //Zprime.Print();
   //Make rotation matrix
  Double_t Drot[3][3];
  Drot[0][0]=XLAB.Dot(Xprime);Drot[1][0]=YLAB.Dot(Xprime);Drot[2][0]=ZLAB.Dot(Xprime);
  Drot[0][1]=XLAB.Dot(Yprime);Drot[1][1]=YLAB.Dot(Yprime);Drot[2][1]=ZLAB.Dot(Yprime);
  Drot[0][2]=XLAB.Dot(Zprime);Drot[1][2]=YLAB.Dot(Zprime);Drot[2][2]=ZLAB.Dot(Zprime);
  //Calculate new coordinates
  TVector3 v_scat; 
  v_scat.SetX(Drot[0][0]*v_sc.X()+Drot[1][0]*v_sc.Y()+Drot[2][0]*v_sc.Z());
  v_scat.SetY(Drot[0][1]*v_sc.X()+Drot[1][1]*v_sc.Y()+Drot[2][1]*v_sc.Z());
  v_scat.SetZ(Drot[0][2]*v_sc.X()+Drot[1][2]*v_sc.Y()+Drot[2][2]*v_sc.Z());
  return v_scat;

}

TGraphErrors* FitApow(TH2F* hFit){

  const Int_t N=hFit->GetNbinsY();
  Float_t xe[N];
  Float_t x[N];
  Float_t y[N];
  Float_t ye[N];

  //project theta bins and fit analysing power
  for(Int_t i=1;i<=N;i++){
    TH1* hphi=hFit->ProjectionX("dummy",i,i);
    hphi->Fit("ap");
    x[i-1]=hFit->GetYaxis()->GetBinCenter(i);
    xe[i-1]=hFit->GetYaxis()->GetBinWidth(i);
    y[i-1]=-ap->GetParameter(1);
    ye[i-1]=ap->GetParError(1);
  }
  TGraphErrors* gr=new TGraphErrors(N,x,y,xe,ye);
  return gr;
}
