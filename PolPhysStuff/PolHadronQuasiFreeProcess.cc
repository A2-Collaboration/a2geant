#include "PolHadronQuasiFreeProcess.hh"
#include "G4HadronInelasticDataSet.hh"
#include "G4GenericIon.hh"
#include "G4ParticleDefinition.hh"
  
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
  //M. Sikora, Oct 12, 2012
  //Added to try to stop memory leaks...
  //delete fPolRotNP;


}
void 
PolHadronQuasiFreeProcess::FillTotalResult(G4HadFinalState * aR, const G4Track & aT)
{
  theTotalResult->Clear();
  theTotalResult->ProposeLocalEnergyDeposit(0.);
  theTotalResult->Initialize(aT);
  theTotalResult->SetSecondaryWeightByProcess(true);
  theTotalResult->ProposeTrackStatus(fAlive);
  G4double rotation = 2.*pi*G4UniformRand();
  G4ThreeVector it(0., 0., 1.);

  if(aR->GetStatusChange()==stopAndKill)
  {
    if( xBiasOn && G4UniformRand()<XBiasSurvivalProbability() )
    {
      theTotalResult->ProposeParentWeight( XBiasSurvivalProbability()*aT.GetWeight() );
    }
    else
    {
      theTotalResult->ProposeTrackStatus(fStopAndKill);
      theTotalResult->ProposeEnergy( 0.0 );
    }
  }
  else if(aR->GetStatusChange()!=stopAndKill )
  {
    if(aR->GetStatusChange()==suspend)
    {
      theTotalResult->ProposeTrackStatus(fSuspend);
      if(xBiasOn)
      {
        G4Exception("G4HadronicProcess", "007", FatalException,
        "Cannot cross-section bias a process that suspends tracks.");
      }
    } else if (aT.GetKineticEnergy() == 0) {
      theTotalResult->ProposeTrackStatus(fStopButAlive);
    }

    if(xBiasOn && G4UniformRand()<XBiasSurvivalProbability())
    {
      theTotalResult->ProposeParentWeight( XBiasSurvivalProbability()*aT.GetWeight() );
      G4double newWeight = aR->GetWeightChange()*aT.GetWeight();
      G4double newM=aT.GetDefinition()->GetPDGMass();
      G4double newE=aR->GetEnergyChange() + newM;
      G4double newP=std::sqrt(newE*newE - newM*newM);
      G4DynamicParticle * aNew = 
      new G4DynamicParticle(aT.GetDefinition(), newE, newP*aR->GetMomentumChange());
      G4HadSecondary * theSec = new G4HadSecondary(aNew, newWeight);
      aR->AddSecondary(theSec);
    }
    else
    {
      G4double newWeight = aR->GetWeightChange()*aT.GetWeight();
      theTotalResult->ProposeParentWeight(newWeight); // This is multiplicative
      if(aR->GetEnergyChange()>-.5) 
      {
        theTotalResult->ProposeEnergy(aR->GetEnergyChange());
      }
      G4LorentzVector newDirection(aR->GetMomentumChange().unit(), 1.);
      newDirection*=aR->GetTrafoToLab();
      theTotalResult->ProposeMomentumDirection(newDirection.vect());
    }
  }
  else
  {
    G4cerr << "Track status is "<< aR->GetStatusChange()<<G4endl;
    G4Exception("G4HadronicProcess", "007", FatalException,
    "use of unsupported track-status.");
  }

  if(GetProcessName() != "hElastic" && GetProcessName() != "HadronElastic"
     &&  theTotalResult->GetTrackStatus()==fAlive
     && aR->GetStatusChange()==isAlive)
    {
    // Use for debugging:   G4double newWeight = theTotalResult->GetParentWeight();

    G4double newKE = std::max(DBL_MIN, aR->GetEnergyChange());
    G4DynamicParticle* aNew = new G4DynamicParticle(aT.GetDefinition(), 
                                                    aR->GetMomentumChange(), 
                                                    newKE);
    G4HadSecondary* theSec = new G4HadSecondary(aNew, 1.0);
    aR->AddSecondary(theSec);
    aR->SetStatusChange(stopAndKill);

    theTotalResult->ProposeTrackStatus(fStopAndKill);
    theTotalResult->ProposeEnergy( 0.0 );

  }
  theTotalResult->ProposeLocalEnergyDeposit(aR->GetLocalEnergyDeposit());
  theTotalResult->SetNumberOfSecondaries(aR->GetNumberOfSecondaries());

  if(aR->GetStatusChange() != stopAndKill)
  {
    G4double newM=aT.GetDefinition()->GetPDGMass();
    G4double newE=aR->GetEnergyChange() + newM;
    G4double newP=std::sqrt(newE*newE - newM*newM);
    G4ThreeVector newPV = newP*aR->GetMomentumChange();
    G4LorentzVector newP4(newE, newPV);
    newP4.rotate(rotation, it);
    newP4*=aR->GetTrafoToLab();
    theTotalResult->ProposeMomentumDirection(newP4.vect().unit());
  }
  // Polarised nucleon scattering in hydro-carbon material
  // Derek G's comment
  // Loop through all produced particles and find the one with
  // the highest kinetic energy which shall be identified as the original
  // particle allowing for charge exchange.
  // the initial particle is killed unless 
  // "Keep initial projectile" is displayed
  G4double phiPol = 0.0;    // phi modification angle
  //G4cout<<"PolHadronQuasiFreeProcess::FillTotalResult "<<IsOrganic(&aT) <<" "<<IsNucleon( aT.GetDynamicParticle())<<" "<<aR->GetNumberOfSecondaries()<<G4endl;
  //if( IsOrganic(&aT) ){     // does tracking material contain H,C?//commented out by M. Sikora, April 27, 2013
  if ( IsOrganic(&aT) || aT.GetVolume()->GetName().contains("SKI") || 
       aT.GetVolume()->GetName().contains("TUNL")) {     // does tracking material contain H,C? or did it scatter in skirt/tunnel?
    if( IsNucleon( aT.GetDynamicParticle() ) ){
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
      // Recalculate phi angle, and return the rotation needed to get it there 
      // phiPol=fPolRot->GetPolarisedRotation(aR->GetSecondary(ip)->GetParticle(),
      //				  aT.GetDynamicParticle());
      if (ip > -1 && ip2 > -1) {
 	//at least 2 nucleons in final state, so potential quasifree
	//incident particle is given, target could be either of the most energetic two nucleons
	G4ParticleDefinition* targetDef = NULL;
	G4ParticleDefinition* Def1 = aR->GetSecondary(ip)->GetParticle()->GetDefinition();
	G4ParticleDefinition* Def2 = aR->GetSecondary(ip2)->GetParticle()->GetDefinition();
	if (Def1 == Def2 && Def1 == aT.GetDefinition()) targetDef = Def1; //incident and target the same
	else if (Def1 != Def2 && Def1 == aT.GetDefinition()) targetDef = Def2; //incident and target different
	else if (Def1 != Def2 && Def2 == aT.GetDefinition()) targetDef = Def1; //incident and target different
	else targetDef = NULL; //both secondaries are different to incident so not quasifree
	if (targetDef) {
	  fPolRotNP->SetReaction(aT.GetDefinition(),targetDef,false); //false=>quasifree  
	  phiPol = fPolRotNP->GetPolarisedRotation(aR->GetSecondary(ip)->GetParticle(),
						   aT.GetDynamicParticle());
	}
	else phiPol = 0; //not quasifree or similar, assume no analysing power
      }
      else if (ip > -1) {
	phiPol=fPolRot->GetPolarisedRotation(aR->GetSecondary(ip)->GetParticle(),
					     aT.GetDynamicParticle()); //not quasifree but can use other parameterisation
      }
      else phiPol = 0; //no nucleon in final state
    }
  }
  else phiPol = 0.0;       // no polarised scattering
  
  for(G4int i=0; i<aR->GetNumberOfSecondaries(); i++)
    {
      G4LorentzVector theM = aR->GetSecondary(i)->GetParticle()->Get4Momentum();
    // Note the extra random azimuthal rotation is removed...   
    // if( !phiPol )    theM.rotate(rotation, it);
    // else
    theM.rotateZ(phiPol);
    theM*=aR->GetTrafoToLab();
    aR->GetSecondary(i)->GetParticle()->Set4Momentum(theM);
    G4double time = aR->GetSecondary(i)->GetTime();
    if(time<0) time = aT.GetGlobalTime();

    G4Track* track = new G4Track(aR->GetSecondary(i)->GetParticle(),
				 time, aT.GetPosition());

    G4double newWeight = aT.GetWeight()*aR->GetSecondary(i)->GetWeight();
    //static G4double pinelcount=0;
    if(xBiasOn) newWeight *= XBiasSecondaryWeight();
    track->SetWeight(newWeight);
    track->SetTouchableHandle(aT.GetTouchableHandle());
    theTotalResult->AddSecondary(track);
  }

  aR->Clear(); 

  return;
}
