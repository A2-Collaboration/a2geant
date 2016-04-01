#include "PolHadronQuasiFreeProcess.hh"
#include "G4HadronInelasticDataSet.hh"
#include "G4GenericIon.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"

PolHadronQuasiFreeProcess::
PolHadronQuasiFreeProcess(const G4String& processName,
			 G4ParticleDefinition* aParticle):
  PolHadronicProcess(processName)
{
  SetProcessSubType(fHadronInelastic);
  AddDataSet(new G4HadronInelasticDataSet());
  theParticle = aParticle;
  fPolRotNP = new PolNPRotate(0,0,0);
}

PolHadronQuasiFreeProcess::~PolHadronQuasiFreeProcess() 
{
  delete fPolRotNP;
}
void PolHadronQuasiFreeProcess::FillResult(G4HadFinalState * aR, const G4Track & aT)
{
  theTotalResult->ProposeLocalEnergyDeposit(aR->GetLocalEnergyDeposit());

  G4double rotation = CLHEP::twopi*G4UniformRand();
  G4ThreeVector it(0., 0., 1.);

  G4double efinal = aR->GetEnergyChange();
  if(efinal < 0.0) { efinal = 0.0; }

  // check status of primary
  if(aR->GetStatusChange() == stopAndKill) {
    theTotalResult->ProposeTrackStatus(fStopAndKill);
    theTotalResult->ProposeEnergy( 0.0 );

    // check its final energy
  } else if(0.0 == efinal) {
    theTotalResult->ProposeEnergy( 0.0 );
    if(aT.GetParticleDefinition()->GetProcessManager()
       ->GetAtRestProcessVector()->size() > 0)
         { theTotalResult->ProposeTrackStatus(fStopButAlive); }
    else { theTotalResult->ProposeTrackStatus(fStopAndKill); }

    // primary is not killed apply rotation and Lorentz transformation
  } else  {
    theTotalResult->ProposeTrackStatus(fAlive);
    G4double mass = aT.GetParticleDefinition()->GetPDGMass();
    G4double newE = efinal + mass;
    G4double newP = std::sqrt(efinal*(efinal + 2*mass));
    G4ThreeVector newPV = newP*aR->GetMomentumChange();
    G4LorentzVector newP4(newE, newPV);
    newP4.rotate(rotation, it);
    newP4 *= aR->GetTrafoToLab();
    theTotalResult->ProposeMomentumDirection(newP4.vect().unit());
    newE = newP4.e() - mass;
    if(G4HadronicProcess_debug_flag && newE <= 0.0) {
      G4ExceptionDescription ed;
      DumpState(aT,"Primary has zero energy after interaction",ed);
      G4Exception("G4HadronicProcess::FillResults", "had011", JustWarning, ed);
    }
    if(newE < 0.0) { newE = 0.0; }
    theTotalResult->ProposeEnergy( newE );
  }
  //G4cout << "FillResult: Efinal= " << efinal << " status= " 
  //	 << theTotalResult->GetTrackStatus() 
  //	 << "  fKill= " << fStopAndKill << G4endl;

  // check secondaries: apply rotation and Lorentz transformation
  G4int nSec = aR->GetNumberOfSecondaries();
  // Polarised nucleon scattering in hydro-carbon material
  // Derek G's comment
  // Loop through all produced particles and find the one with
  // the highest kinetic energy which shall be identified as the original
  // particle allowing for charge exchange.
  // the initial particle is killed unless 
  // "Keep initial projectile" is displayed
  G4double phiPol = 0.0;    // phi modification angle
  if( IsOrganic(&aT) ){     
    if( IsNucleon( aT.GetDynamicParticle() ) ){
      //for quassifree need two final state particles for (quasi-elastic)
      //choose 2 most energetic (kinetic)
      G4double highKE = 0.;
      G4int ip = -1;
      G4double highKE2 = 0.;
      G4int ip2 = -1;
      for (G4int i = 0; i < aR->GetNumberOfSecondaries(); ++i) {
	G4DynamicParticle* secDP = aR->GetSecondary(i)->GetParticle();
	//G4cout<<i<<" "<<secDP->GetDefinition()->GetParticleName()<<G4endl;
	if( IsNucleon(secDP) )
	  if(secDP->GetKineticEnergy()>highKE)
	    { highKE = secDP->GetKineticEnergy(); ip=i;}
      }
      for (G4int i = 0; i < aR->GetNumberOfSecondaries(); ++i) {
	G4DynamicParticle* secDP = aR->GetSecondary(i)->GetParticle();
	if( IsNucleon(secDP) )
	  if(secDP->GetKineticEnergy()>highKE2 && i!=ip)
	    { highKE2 = secDP->GetKineticEnergy(); ip2 = i;}
      }
      // G4cout<<aR->GetNumberOfSecondaries()<<" "<<highKE+highKE2<<" "<<aT.GetDynamicParticle()->GetKineticEnergy()<<G4endl;
     // Recalculate phi angle, and return the rotation needed to get it there 
      // phiPol=fPolRot->GetPolarisedRotation(aR->GetSecondary(ip)->GetParticle(),
      //				  aT.GetDynamicParticle());
      if (ip > -1 && ip2 > -1) {
 	//at least 2 nucleons in final state, so potential quasifree
	//incident particle is given, target could be either of the most energetic two nucleons
	G4ParticleDefinition* targetDef = NULL;
	G4ParticleDefinition* Def1 = aR->GetSecondary(ip)->GetParticle()->GetDefinition();
	G4ParticleDefinition* Def2 = aR->GetSecondary(ip2)->GetParticle()->GetDefinition();
	fPolRotNP->SetExchange(false);	
	if (Def1 == Def2 && Def1 == aT.GetDefinition()) targetDef = Def1; //incident and target the same
	else if (Def1 != Def2 && Def1 == aT.GetDefinition()) 
	  targetDef = Def2; //incident and target different, incident and scattered same
	else if (Def1 != Def2 && Def2 == aT.GetDefinition()){
	  targetDef = Def1; //incident and target different, target and scattered same
	  fPolRotNP->SetExchange(true);	
	}
	else targetDef = NULL; //both secondaries are different to incident so not quasifree
	//if(highKE+highKE2<aT.GetDynamicParticle()->GetKineticEnergy()*0.65)targetDef=NULL; //too much energy loss not quasi free!!!Need something better
	if (targetDef) {
	  //set initial state
	  fPolRotNP->SetReaction(aT.GetDefinition(),targetDef,false); //false=>quasifree  
	  //was final state scattered nucleon a proton or neutron?
	  phiPol = fPolRotNP->GetPolarisedRotation(aR->GetSecondary(ip)->GetParticle()->GetMomentum(),aT.GetDynamicParticle());
	}
	else phiPol = 0; //not quasifree or similar, assume no analysing power
      }
      else if (ip > -1) {
	phiPol=fPolRot->GetPolarisedRotation(aR->GetSecondary(ip)->GetParticle()->GetMomentum(),aT.GetDynamicParticle()); //not quasifree but can use other parameterisation
      }
      else phiPol = 0; //no nucleon in final state
    }
  }
  else phiPol = 0.0;       // no polarised scattering
  
  
  theTotalResult->SetNumberOfSecondaries(nSec);
  G4double weight = aT.GetWeight();
  
  if (nSec > 0) {
    G4double time0 = aT.GetGlobalTime();
    for (G4int i = 0; i < nSec; ++i) {
      G4LorentzVector theM = aR->GetSecondary(i)->GetParticle()->Get4Momentum();
      // check is there is azimuthal modulation
      if(!phiPol) theM.rotate(rotation, it);
      //else theM.rotateZ(phiPol);
      else theM.rotate(phiPol, it);
      theM *= aR->GetTrafoToLab();
       aR->GetSecondary(i)->GetParticle()->Set4Momentum(theM);

      // time of interaction starts from zero
      G4double time = aR->GetSecondary(i)->GetTime();
      if (time < 0.0) { time = 0.0; }

      // take into account global time
      time += time0;

      G4Track* track = new G4Track(aR->GetSecondary(i)->GetParticle(),
                                   time, aT.GetPosition());
      track->SetCreatorModelIndex(aR->GetSecondary(i)->GetCreatorModelType());
      G4double newWeight = weight*aR->GetSecondary(i)->GetWeight();
	// G4cout << "#### ParticleDebug "
	// <<GetProcessName()<<" "
	//<<aR->GetSecondary(i)->GetParticle()->GetDefinition()->GetParticleName()<<" "
	// <<aScaleFactor<<" "
	// <<XBiasSurvivalProbability()<<" "
	// <<XBiasSecondaryWeight()<<" "
	// <<aT.GetWeight()<<" "
	// <<aR->GetSecondary(i)->GetWeight()<<" "
	// <<aR->GetSecondary(i)->GetParticle()->Get4Momentum()<<" "
	// <<G4endl;
      track->SetWeight(newWeight);
      track->SetTouchableHandle(aT.GetTouchableHandle());
      theTotalResult->AddSecondary(track);
      if (G4HadronicProcess_debug_flag) {
        G4double e = track->GetKineticEnergy();
        if (e <= 0.0) {
          G4ExceptionDescription ed;
          DumpState(aT,"Secondary has zero energy",ed);
          ed << "Secondary " << track->GetDefinition()->GetParticleName()
             << G4endl;
          G4Exception("G4HadronicProcess::FillResults", "had011", 
		      JustWarning,ed);
        }
      }
    }
  }
  aR->Clear();
}
