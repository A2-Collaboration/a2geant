
#include "A2RunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"



A2RunAction::A2RunAction()
{
  fEventAction=NULL;
}



A2RunAction::~A2RunAction()
{}



void A2RunAction::BeginOfRunAction(const G4Run* aRun)
{ 
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);

  //Open output file
  fEventAction=  const_cast<A2EventAction*>(static_cast<const A2EventAction*>(G4RunManager::GetRunManager()->GetUserEventAction()));
  fEventAction->PrepareOutput();
}




void A2RunAction::EndOfRunAction(const G4Run* aRun)
{
  G4int NbOfEvents = aRun->GetNumberOfEvent();
  if (NbOfEvents == 0) return;

  fEventAction->CloseOutput();

}

