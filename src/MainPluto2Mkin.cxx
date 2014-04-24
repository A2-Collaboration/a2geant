// SVN Info: $Id: MainPluto2Mkin.cxx 486 2009-08-19 15:11:46Z werthm $

/*************************************************************************
 * Author: Dominik Werthmueller, 2009
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MainPluto2Mkin                                                       //
//                                                                      //
// Contains the main method for the pluto2mkin event generator output   //
// file conversion tool.                                                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TFile.h"
#include "TTree.h"
#include "TError.h"
#include "TRandom3.h"

#include "TOSUtils.h"
#include "TOMCMkinWriter.h"


enum EOMCGen_ARGCHK_CODES
{
    EP2M_ARGCHK_CONVERT_OK,
    EP2M_ARGCHK_DO_PRINT_HELP,
    EP2M_ARGCHK_NO_OPTION,
    EP2M_ARGCHK_BAD_OPTION,
    EP2M_ARGCHK_NO_FILE
};


Char_t gInputFile[256];
Double_t gTargetLength;
Double_t gBeamEnergy;
Double_t gBeamDiameter;
Int_t gNSkip;
Int_t gSkip[99];


//______________________________________________________________________________
Bool_t ConvertFile()
{
    // Convert the pluto input file into a mkin file.
    // Return kFALSE if an error occured.
    
    // try to open ROOT file
    TFile* fIn = new TFile(gInputFile);
    if (fIn->IsZombie())
    {
        delete fIn;
        Error("ConvertFile", "Could not open '%s'!", gInputFile);
        return kFALSE;
    }

    // try to read tree
    TTree* t = (TTree*) fIn->Get("data");
    if (!t)
    {
        delete fIn;
        Error("ConvertFile", "Could not find Pluto event tree in the file '%s'!", gInputFile);
        return kFALSE;
    }
    
    // create output file name
    Char_t outName[256];
    sprintf(outName, "%s_mkin.root", TOSUtils::ExtractPureFileName(gInputFile));
    
    // create random number generator
    TRandom3* rand =  new TRandom3(0);

    // user output
    printf("\n");
    printf("########################################################\n");
    printf("#                                                      #\n");
    printf("#  pluto2mkin                                          #\n");
    printf("#                                                      #\n");
    printf("#  Input file  : %-36s  #\n", gInputFile);
    printf("#  Output file : %-36s  #\n", outName);
    printf("#  Random seed : %-36d  #\n", rand->GetSeed());
    if (gNSkip) printf("#  Skipped IDs : %-36s  #\n", TOSUtils::FormatArrayList(gNSkip, gSkip));
    else printf("#  Skipped IDs : none                                  #\n");
    printf("#                                                      #\n");
    printf("#  Photon beam energy : %-7.2f MeV                    #\n", gBeamEnergy);
    printf("#  Beam diameter      : %-6.3f cm                      #\n", gBeamDiameter);
    printf("#  Target length      : %-6.3f cm                      #\n", gTargetLength);
    printf("#                                                      #\n");
    printf("########################################################\n");
    printf("\n");
 
    // connect branches
    Int_t nPart;
    Int_t part;
    Double_t partPx[99];
    Double_t partPy[99];
    Double_t partPz[99];
    Double_t partE[99];
    Int_t partID[99];

    t->SetMakeClass(1);
    t->SetBranchAddress("Npart",           &nPart);
    t->SetBranchAddress("Particles",       &part);
    t->SetBranchAddress("Particles.fP.fX", partPx);
    t->SetBranchAddress("Particles.fP.fY", partPy);
    t->SetBranchAddress("Particles.fP.fZ", partPz);
    t->SetBranchAddress("Particles.fE",    partE);
    t->SetBranchAddress("Particles.pid",   partID);
    
    // check number of particles, create particle list of mkin supported
    // particles
    t->GetEntry(0);
    printf("-> %d particles found\n", nPart);
    if (nPart > 99)
    {
        delete fIn;
        Error("ConvertFile", "Maximum 99 particles are supported by the mkin format!");
        return kFALSE;
    }
    TOMCParticle* mkinPart[nPart];
    Int_t mkinPlutoIndex[nPart];
    Int_t nMkinPart = 0;
     
    // loop over all Pluto particles
    for (Int_t i = 0; i < nPart; i++)
    {   
        Int_t pdgID;

        // get PDG ID
        Int_t plutoID = partID[i];
    
        // handle self-defined particles
        if (plutoID == 45) pdgID = TOGlobals::kDeuteronID;
	if (plutoID == 41) pdgID = 113;  //rho0 wrong defined, change that Geant4 doesn't crash
        else
        {
            pdgID = TOGlobals::GetPDG_DB()->ConvertGeant3ToPdg(plutoID);

            // check if ID is a valid GEANT3 ID
            if (pdgID == 0) printf("Pluto particle '%d' has no valid GEANT3 ID. Setting GEANT3 ID to 0. "
                                   "This particle will not be trackable!\n", plutoID);
        }

        // get particle from database
        TParticlePDG* pdgPart = TOGlobals::GetPDG_DB()->GetParticle(pdgID);

        // check if ID should be skipped
        Bool_t skip = kFALSE;
        for (Int_t j = 0; j < gNSkip; j++) if (i == gSkip[j]) skip = kTRUE;
        if (skip)
        {
            printf("Skipping %s with Pluto index %d as requested\n", pdgPart->GetName(), i);
            continue;
        }

        // create particle for mkin output
        mkinPart[nMkinPart] = new TOMCParticle(pdgPart, kNoDecay);
        mkinPlutoIndex[nMkinPart] = i;

        // user output
        printf("Pluto Index: %-2d ===> Mkin Index: %-2d  :  Added %-15s (PDG: %-5d   G3: %-2d)\n", 
               i, nMkinPart+1, pdgPart->GetName(), pdgID, plutoID);
        
        // increment mkin particle index
        nMkinPart++;
    }

    // open mkin output writer
    TOMCMkinWriter* mkinWriter = new TOMCMkinWriter(outName);
    mkinWriter->Initialize(mkinPart, nMkinPart);

    // loop over all events
    for (Int_t i = 0; i < t->GetEntries(); i++)
    {
        // read event
        t->GetEntry(i);

        // save all mkin particles
        for (Int_t j = 0; j < nMkinPart; j++)
        {
            // get pluto id
            Int_t id = mkinPlutoIndex[j];

            // set 4-vector
            mkinPart[j]->SetP4(partPx[id], partPy[id], partPz[id], partE[id]);
        }

        // set photon beam energy
        if (gBeamEnergy > 0.) mkinWriter->SetBeamP4(0., 0., gBeamEnergy/1000., gBeamEnergy/1000.);
        else mkinWriter->SetBeamP4(partPx[0], partPy[0], partPz[0], TMath::Sqrt(partPx[0]*partPx[0] + partPy[0]*partPy[0] + partPz[0]*partPz[0]));

        // set vertex position 
        Double_t vX = 0.;
        Double_t vY = 0.;
        if (gBeamDiameter > 0.)
        {
            // randomize the vertex within the beam spot and the target
            vX = 1e10; 
            vY = 1e10; 
            Double_t beamRad = gBeamDiameter / 2.;
            while (vX*vX+vY*vY > beamRad*beamRad)
            {
                vX = beamRad * (2. * rand->Uniform(1.) - 1.);
                vY = beamRad * (2. * rand->Uniform(1.) - 1.);
            }
        }
    
        if (gBeamDiameter > 0. || gTargetLength > 0.) 
            mkinWriter->SetVertex(vX, vY, gTargetLength / 2. * (2. * rand->Uniform(1.) - 1.));

        // save mkin event
        mkinWriter->SaveEvent();
    }

    // save mkin file
    mkinWriter->WriteFile();

    // free memory
    for (Int_t i = 0; i < nMkinPart; i++) delete mkinPart[i];
    delete fIn;
    delete mkinWriter;
    delete rand;

    return kTRUE;
}

//______________________________________________________________________________
void PrintHelp()
{
    // Print the help to this application.

    printf("\n");
    printf("pluto2mkin - a Pluto to mkin event file converter\n");
    printf("(C) 2009 by Dominik Werthmueller\n");
    printf("\n");
    printf("Usage: pluto2mkin [--beam] [--target] --input filename\n");
    printf("\n");
    printf("    --help              prints this help\n");
    printf("\n");
    printf("\n");
    printf("    --skip n[,m,l,...]  skip particle with Pluto indices n,m,l,...\n");
    printf("\n");
    printf("\n");
    printf("    --beam              specifiy beam properties\n");
    printf("                        parameters:\n");
    printf("                                    energy=    photon beam energy [MeV]\n");
    printf("                                    diam=      beam diameter in target [cm]\n");
    printf("\n");
    printf("\n");
    printf("    --target            specifiy target properties\n");
    printf("                        parameters:\n");
    printf("                                    length=    target length in z-direction [cm]\n");
    printf("\n");
}

//______________________________________________________________________________
void CheckBeamParams(Char_t* params)
{
    // Check if the correct parameters were provided when specifying the
    // beam properties. 

    Char_t par[256];
    Char_t val[256];

    // check if params are empty
    if (!strcmp(TOSUtils::Trim(params), ""))
    {
        Error("CheckBeamParams", "No parameters for beam option provided!");
        return;
    }

    // parse parameters
    Char_t* p = strtok((Char_t*)params, " ");
    while (p != 0)
    {
        // try to fetch parameter
        Char_t* pos = strchr(p, '=');
        if (!pos)
        {
            Error("CheckBeamParams", "Bad parameter statement in beam option!");
            return;
        }
        strncpy(par, p, pos-p);
        par[pos-p] = '\0';

        // fetch parameter value as string
        strcpy(val, pos+1);

        // convert value if necessary (all except 'part')
        if (!strcmp(par, "energy")) gBeamEnergy = atof(val);
        else if (!strcmp(par, "diam")) gBeamDiameter = atof(val);
        else 
        {
            Error("CheckBeamParams", "Unknown parameter in beam option!");
            return;
        }

        // scan for next token
        p = strtok (0, " ");
    }
}

//______________________________________________________________________________
void CheckTargetParams(Char_t* params)
{
    // Check if the correct parameters were provided when specifying the
    // target properties. 

    Char_t par[256];
    Char_t val[256];

    // check if params are empty
    if (!strcmp(TOSUtils::Trim(params), ""))
    {
        Error("CheckTargetParams", "No parameters for target option provided!");
        return;
    }

    // parse parameters
    Char_t* p = strtok((Char_t*)params, " ");
    while (p != 0)
    {
        // try to fetch parameter
        Char_t* pos = strchr(p, '=');
        if (!pos)
        {
            Error("CheckTargetParams", "Bad parameter statement in target option!");
            return;
        }
        strncpy(par, p, pos-p);
        par[pos-p] = '\0';

        // fetch parameter value as string
        strcpy(val, pos+1);

        // convert value if necessary (all except 'part')
        if (!strcmp(par, "length")) gTargetLength = atof(val);
        else 
        {
            Error("CheckTargetParams", "Unknown parameter in target option!");
            return;
        }

        // scan for next token
        p = strtok (0, " ");
    }
}

//______________________________________________________________________________
void CheckSkipParams(Char_t* params)
{
    // Check if the correct parameters were provided when specifying the
    // skip property.

    // check if params are empty
    if (!strcmp(TOSUtils::Trim(params), ""))
    {
        Error("CheckSkipParams", "No parameters for skip option provided!");
        return;
    }

    // parse parameters
    Char_t* p = strtok((Char_t*)params, ",");
    while (p != 0)
    {
        // add index to skip list
        gSkip[gNSkip++] = atoi(TOSUtils::Trim(p));

        // scan for next token
        p = strtok (0, ",");
    }
}

//______________________________________________________________________________
Int_t DecodeOptions(Int_t argc, Char_t* argv[])
{
    // Decode command line options. Returns 0 if the configuration is ok so
    // the program can continue. Returns an error code != 0 if there were some
    // problems and the program should abort.

    Char_t params[1024] = "";
    Char_t tmp[256];

    // no argument was provided
    if (argc == 1) return EP2M_ARGCHK_NO_OPTION;

    // concatenate all arguments into one string
    for (Int_t i = 1; i < argc; i++) 
    {
        strcat(params, argv[i]);
        strcat(params, " ");
    }

    // loop over all "--" argument tokens
    Int_t start = 0;
    Int_t end = 0;
    while ((start = TOSUtils::IndexOf(params, "--", start)) >= 0)
    {
        // end is either the beginning of the next "--" token or the
        // end of the total argument string
        if ((end = TOSUtils::IndexOf(params, "--", start+1)) == -1)
        end = strlen(params);
        
        // copy the token into a separate string, terminate it
        strncpy(tmp, params+start, end-start);
        tmp[end-start] = '\0';
        
        // ----------------------------- Start parsing -----------------------------
        
        // help output
        if (TOSUtils::IndexOf(tmp, "--help") == 0) return EP2M_ARGCHK_DO_PRINT_HELP;
        // beam properties
        else if (TOSUtils::IndexOf(tmp, "--beam") == 0) CheckBeamParams(tmp+6);
        // target properties
        else if (TOSUtils::IndexOf(tmp, "--target") == 0) CheckTargetParams(tmp+8);
        // skip particle
        else if (TOSUtils::IndexOf(tmp, "--skip") == 0) CheckSkipParams(tmp+6);
        // input file name
        else if (TOSUtils::IndexOf(tmp, "--input") == 0) strcpy(gInputFile, TOSUtils::Trim(tmp+8));
        // bad option
        else return EP2M_ARGCHK_BAD_OPTION;

        // ----------------------------- End parsing -----------------------------
        
        // set the new start to the current end value
        start = end;
    }

    // check for input file
    if (!strcmp(gInputFile, "")) return EP2M_ARGCHK_NO_FILE;
    else return EP2M_ARGCHK_CONVERT_OK;
}

//______________________________________________________________________________
Int_t main(Int_t argc, Char_t* argv[])
{
    // Main method.

    // init global variables
    gInputFile[0] = '\0';
    gTargetLength = 0.;
    gBeamEnergy   = 0.;
    gBeamDiameter = 0.;
    gNSkip = 0;

    // decode command line options and check argument check return values
    switch (DecodeOptions(argc, argv))
    {
        // parameters ok, convert the file
        case EP2M_ARGCHK_CONVERT_OK:
            if (!ConvertFile())
            {
                Error("main", "An error occured during the conversion!");
                return -1;
            }
            return 0;
        // print the help screen
        case EP2M_ARGCHK_DO_PRINT_HELP:
            PrintHelp();
            return 0;
        // no command line argument was provided
        case EP2M_ARGCHK_NO_OPTION:
            PrintHelp();
            return -1;
        // bad command line argument was provided
        case EP2M_ARGCHK_BAD_OPTION:
            Error("main", "Bad argument! Check command line options using 'pluto2mkin --help'");
            return -1;
        // no input file was provided
        case EP2M_ARGCHK_NO_FILE:
            Error("main", "No input file provided. Check command line options using 'pluto2mkin --help'");
            return -1;
        default:
            Error("main", "Unknown problem!");
            return -1;
    }
    
    return 0;
}

