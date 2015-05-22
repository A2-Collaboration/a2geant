
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
#ifdef G4UI_USE_QT
#include "G4UIQt.hh"
#include "G4Qt.hh"
#endif
#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif
#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "A2DetectorConstruction.hh"
#include "A2PhysicsList.hh"
#include "A2PrimaryGeneratorAction.hh"
#include "A2RunAction.hh"
#include "A2EventAction.hh"
#include "A2SteppingAction.hh"
#include "A2SteppingVerbose.hh"

//#include "LHEP_BIC.hh"

#include <getopt.h>
//#include <CMakeG4Env.hh>

int main(int argc,char** argv) {
  //  SET_G4_ENV_BY_CMAKE();

  // Define options
  const char *optsShort = "hm:i:o:n:d:";
  const struct option optsLong[] = {
    {"help", no_argument,      NULL,'h'},
    {"mac",  required_argument,NULL,'m'},
    {"if",   required_argument,NULL,'i'},
    {"of",   required_argument,NULL,'o'},
    {"num",  required_argument,NULL,'n'},
    {"det",  required_argument,NULL,'d'},
    {"gui",  no_argument,NULL,'g'},
    {NULL,   0                ,NULL, 0 }
  };
  
  // Parse options
  G4int isInteractive  = 1;			// No macro so interactive (default)
  G4String nameFileMac = "macros/vis.mac";	// Default macro for interactive mode
  G4int numberOfEvents = -1;
#if defined(G4UI_USE_XM) || defined(G4UI_USE_WIN32)
  // Customize the G4UIXm,Win32 menubar with a macro file :
//   nameFileMac = "visTutor/gui.mac");
#endif
  G4String nameFileInput, nameFileOutput;
  G4String  detSetup="macros/DetectorSetup.mac";
  G4int rez;
  G4int iOpt = -1;
  G4bool isOutputSet = false;
  G4bool gotOptions = false; //got some options so use them
  G4bool gui=false; 
  while ( (rez=getopt_long(argc,argv,optsShort,optsLong,&iOpt)) != -1 )
  {
    gotOptions = true;
    switch (rez)
    {
      case 'h':
	G4cout << G4endl;
	G4cout << "Usage: " << argv[0] << " [--mac=file] [--if=file] [--of=file] [--num=N]  [--det=file] [--help]" << G4endl;
	G4cout << G4endl;
	G4cout << "Options: " << G4endl;
	G4cout << "\t-h --help \t print this help and exit" << G4endl;
	G4cout << "\t-i --if   \t input mkin file (overwrites /A2/event/setInputFile command in macro)" << G4endl;
	G4cout << "\t-m --mac  \t .mac file to run in batch mode" << G4endl;
	G4cout << "\t-n --num  \t # of events to simulate" << G4endl;
	G4cout << "\t-d --det  \t detector setup macro" << G4endl;
	G4cout << "\t-g --gui  \t use gui" << G4endl;
	G4cout << "\t-o --of   \t output file (overwrites /A2/event/setOutputputFile command in macro)" << G4endl;
	G4cout << G4endl;
	exit(EXIT_SUCCESS);
      case 'm':
	nameFileMac = optarg;
	G4cout << "Going to run file from command line macro " << nameFileMac << G4endl;
	isInteractive = 0;
	break;
      case 'i':
	nameFileInput = optarg;
	G4cout << "Going to run file from command line argument! " << nameFileInput << G4endl;
	if (!isOutputSet)
	{
	  nameFileOutput = nameFileInput;
	  G4int place = nameFileInput.find_last_of("/");
	  nameFileOutput.insert(place+1,"tr_");
	}
	break;
      case 'o':
	nameFileOutput = optarg;
	isOutputSet = true;
	G4cout << "Going to save file from command line argument! " << nameFileOutput << G4endl;
	break;
      case 'd':
	detSetup = optarg;
	G4cout << "Going use detector setup from command line argument! " << detSetup  << G4endl;
	break;
      case 'n':
	numberOfEvents = atoi(optarg);
	break;
      case 'g':
	gui=true;
	break;
      case '?':
      default:
	G4cout << "Unknown option!" << G4endl;
	exit(EXIT_FAILURE);
    }
  }
  
  //
  if (!gotOptions)
  {
    //no options so look for macro on command line
    if (argc > 1)
    {
      isInteractive = 0;
      TString cl1(argv[1]);
      if (cl1.Contains(".mac")) nameFileMac = argv[1];
      TString cl2(argv[2]);
      if (cl2.Contains(".root")) nameFileInput = argv[2];
      if (argc == 3)
      {
	//make the ouput file name from the input
	nameFileOutput = nameFileInput;
	G4int place = nameFileOutput.find_last_of("/");
	nameFileOutput.insert(place+1,"tr_");
      }
      TString cl3(argv[3]);
      if (cl3.Contains(".root")) nameFileOutput = argv[3];
    }
  }

  // Choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  
  // My verbose output class
  G4VSteppingVerbose::SetInstance(new A2SteppingVerbose);
     
  // Construct the default run manager
  G4RunManager * runManager = new G4RunManager;

  // Set mandatory initialization classes
  A2DetectorConstruction* detector = new A2DetectorConstruction(detSetup);
  detector->SetIsInteractive(isInteractive);
  runManager->SetUserInitialization(detector);
  //runManager->SetUserInitialization(new LHEP_BIC);
  // Use below insted if cannot install physics_list
  runManager->SetUserInitialization(new A2PhysicsList);

  G4UIsession* session = 0;
  G4UIExecutive* uiexecutive =0;
   // Just run from command line at the moment
  if (isInteractive)// Define UI session for interactive mode.
  {
    if(!gui)session = new G4UIterminal(new G4UItcsh);
    else uiexecutive = new G4UIExecutive(argc,argv);
  }
  
#ifdef G4VIS_USE
  // Visualization manager
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
  if (!session) visManager->SetVerboseLevel("quiet");
#endif
  // Set user action classes
  A2PrimaryGeneratorAction* pga=new A2PrimaryGeneratorAction();
  pga->SetDetCon(detector);
  runManager->SetUserAction(pga);

  A2RunAction* runaction = new A2RunAction;  
  runManager->SetUserAction(runaction);
  A2EventAction* eventaction = new A2EventAction(runaction);
  eventaction->SetIsInteractive(isInteractive);
  runManager->SetUserAction(eventaction);
  runManager->SetUserAction(new A2SteppingAction(detector, eventaction));
  // Initialize G4 kernel
//   runManager->Initialize();
    
  // Get the pointer to the User Interface manager 
  G4UImanager* UI = G4UImanager::GetUIpointer();

  // Set macro (and run, ignoring all arguments, if  /run/beamOn command is set in the macro)
  UI->ApplyCommand("/control/execute " + nameFileMac);
  
  // Set input file
  if (!nameFileInput.empty())
    {
      UI->ApplyCommand("/A2/generator/InputFile " + nameFileInput);
    }
  
  // Set output file
  if (!nameFileOutput.empty())
    {
      eventaction->SetOutFileName(nameFileOutput);
    }
  
  // Set and prepare input if it has been set
  pga->SetUpROOTInput();
  
  if (session||uiexecutive)   // Define UI session for interactive mode.
    {
      // G4UIterminal is a (dumb) terminal.
      if(!gui)session->SessionStart();
      else {
	UI->ApplyCommand("/control/execute macros/gui.mac");
	uiexecutive->SessionStart();
	delete uiexecutive;
      }
      delete session;
    }
  else           // Batch mode with ROOT input
    {
      
      if (pga->GetMode() == EPGA_ROOT || numberOfEvents > 0)
	{
	  // Run in batch mode
	  if (numberOfEvents < 0) numberOfEvents=pga->GetNEvents();
	  G4cout << "Will analyse " << numberOfEvents << " events." << G4endl;
	  runManager->BeamOn(numberOfEvents);
	}
    }
  
  // Job termination
#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;

  return 0;
}
