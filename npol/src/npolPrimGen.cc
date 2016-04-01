

#include "npolPrimGen.hh"

#include "npolDetCon.hh"
#include "npolEventAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

npolPrimaryGeneratorAction::npolPrimaryGeneratorAction(
						       npolDetectorConstruction* npolDC,npolEventAction* npolEA)
  :npolDetector(npolDC),npolEvAct(npolEA)
{
  G4int n_particle = 1;
  particleGun  = new G4ParticleGun(n_particle);
  

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle
                    = particleTable->FindParticle(particleName="proton");
  particleGun->SetParticleDefinition(particle);
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  particleGun->SetParticleEnergy(100.*MeV);
  // G4double position = -0.5*(npolDetector->GetWorldSizeX());
  G4double position = 0*cm;
  particleGun->SetParticlePosition(G4ThreeVector(0.*cm,0.*cm,position));

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

npolPrimaryGeneratorAction::~npolPrimaryGeneratorAction()
{
  delete particleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void npolPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //this function is called at the begining of event
  // 
//   G4double x0 = -0.5*(npolDetector->GetWorldSizeX());
//   G4double y0 = 0.*cm, z0 = 0.*cm;
//   if (rndmFlag == "on")
//      {y0 = (npolDetector->GetCalorSizeYZ())*(G4UniformRand()-0.5);
//       z0 = (npolDetector->GetCalorSizeYZ())*(G4UniformRand()-0.5);
//      } 
//   particleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
  G4ThreeVector mom=particleGun->GetParticleMomentumDirection();
  G4double mass=particleGun->GetParticleDefinition()->GetPDGMass()/MeV;
  G4double energy=particleGun->GetParticleEnergy()/MeV;
  npolEvAct->SetIncMass(mass);
  mom=mom*sqrt((energy+mass)*(energy+mass)-mass*mass);
  npolEvAct->SetIncMom(mom);
  //G4cout<<"Energy "<<energy/MeV<<G4endl;
  particleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

