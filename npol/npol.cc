
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

#ifdef G4UI_USE_XM
#include "G4UIXm.hh"
#endif

#ifdef G4UI_USE_WIN32
#include "G4UIWin32.hh"
#endif

#include "Randomize.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#include "npolDetCon.hh"
#include "npolPrimGen.hh"
#include "npolEventAction.hh"
#include "npolSteppingAction.hh"
#include "PolPhysicsList.hh"
//#include "QGSP.hh"
#include "QGSP_BIC.hh"
#include <stdlib.h>

using namespace CLHEP;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
int main(int argc,char** argv)
{
  // Choose the Random engine
  //
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  if(argc!=4){ G4cerr<<"pol macro rootfile targetthick(cm)"<<G4endl;exit(0);}
     
  // Construct the default run manager
  //
  G4RunManager * runManager = new G4RunManager;

  // Set mandatory initialization classes
  //
npolDetectorConstruction* detector = new npolDetectorConstruction(atof(argv[3]));
  runManager->SetUserInitialization(detector);
  //
  // G4VUserPhysicsList* physics = new QGSP_BIC;
  // G4VUserPhysicsList* physics = new PolPhysicsList;
  PolPhysicsList* physics = new PolPhysicsList;
  runManager->SetUserInitialization(physics);

  G4UIsession* session=0;
  
  if (argc==1)   // Define UI session for interactive mode.
    {
     session = new G4UIterminal(new G4UItcsh);      

    }
  
#ifdef G4VIS_USE
  // Visualization manager
  //
  G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
#endif
    
  // Set user action classes
  //
    // npolEventAction* event_action = new npolEventAction();
  npolEventAction* event_action = new npolEventAction(argv[2]);
  event_action->SetPhysicsList((PolPhysicsList*)physics);
  G4VUserPrimaryGeneratorAction* gen_action = new npolPrimaryGeneratorAction(detector,event_action);
  runManager->SetUserAction(gen_action);

  runManager->SetUserAction(event_action);
  runManager->SetUserAction(new npolSteppingAction(detector, event_action));

  // Initialize G4 kernel
  //
  // runManager->Initialize();
    
  // Get the pointer to the User Interface manager
  //
  G4UImanager* UI = G4UImanager::GetUIpointer();  

  if (session)   // Define UI session for interactive mode
    {
      // G4UIterminal is a (dumb) terminal
      //
      UI->ApplyCommand("/control/execute vis.mac");    

      session->SessionStart();
      delete session;
    }
  else           // Batch mode
    { 
#ifdef G4VIS_USE
      visManager->SetVerboseLevel("quiet");
#endif
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UI->ApplyCommand(command+fileName);
    }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !

#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;
  //delete verbosity;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
