// SVN Info: $Id: TOMCMkinWriter.cxx 447 2009-06-12 18:32:25Z werthm $

/*************************************************************************
 * Author: Dominik Werthmueller, 2008-2009
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TOMCMkinWriter                                                       //
//                                                                      //
// Writer for MC kinematics ROOT files using the CB mkin file format.   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TOMCMkinWriter.h"

ClassImp(TOMCMkinWriter)


//______________________________________________________________________________
TOMCMkinWriter::TOMCMkinWriter(const Char_t* inOutFileName)
    : TObject()
{
    // Constructor.

    // create members
    fVertex    = new TVector3(0., 0., 0.);
    fBeamP4    = new TLorentzVector(0., 0., 1., 1.);
    fParticle  = 0;
    fNParticle = 0;
    strcpy(fOutFileName, inOutFileName);
    fFile      = 0;
    fNtuple    = 0;
    fBuffer    = 0;
    fNEvents   = 0;
}

//______________________________________________________________________________
TOMCMkinWriter::~TOMCMkinWriter()
{
    // Destructor.
 
    if (fVertex) delete fVertex;
    if (fBeamP4) delete fBeamP4;
    if (fNtuple) delete fNtuple;
    if (fBuffer) delete [] fBuffer;
    if (fFile)   delete fFile;
}

//______________________________________________________________________________
void TOMCMkinWriter::Initialize(TOMCParticle** inParticle, UInt_t inNParticle)
{
    // Open the output ROOT file and initialize the ntuple.
    // (Inspired by the implementation in the TMCGenerator class of AcquRoot)

    Char_t varNames[1024];
    Char_t name[256];
    Char_t* temp;
    Int_t g3_ID;

    // set the number of particles and the particles list
    fParticle = inParticle;
    fNParticle = inNParticle;

    // vertex and beam variables
    strcpy(varNames, "X_vtx:Y_vtx:Z_vtx:Px_bm:Py_bm:Pz_bm:Pt_bm:En_bm");
    
    // loop over final state particles
    for (UInt_t i = 0; i < fNParticle; i++)
    {
        // write particle variables
        temp = varNames + strlen(varNames);
        g3_ID = fParticle[i]->GetGeant3_ID();
        sprintf(temp, ":Px_l%02d%02d:Py_l%02d%02d:Pz_l%02d%02d:Pt_l%02d%02d:En_l%02d%02d",
                i+1, g3_ID, i+1, g3_ID, i+1, g3_ID, i+1, g3_ID, i+1, g3_ID); 
    }
    
    // create output file
    fFile = new TFile(fOutFileName, "RECREATE");
    
    // create output buffer
    fBuffer = new Float_t[8 + fNParticle * 5];

    // create ntuple
    sprintf(name, "mkin MC file created by %s", ClassName());
    fNtuple = new TNtuple("h1", name, varNames);
}

//______________________________________________________________________________
void TOMCMkinWriter::SaveEvent()
{
    // Save current content as a new event in the ntuple.
    // (Inspired by the implementation in the TMCGenerator class of AcquRoot)

    Float_t* tmp = fBuffer;
    Double_t ptot;  
    TLorentzVector* p4;

    // save vertex
    *tmp++ = fVertex->X();
    *tmp++ = fVertex->Y(); 
    *tmp++ = fVertex->Z();
    
    // save beam 4-vector
    ptot = (fBeamP4->Vect()).Mag();
    *tmp++ = fBeamP4->X() / ptot;
    *tmp++ = fBeamP4->Y() / ptot;
    *tmp++ = fBeamP4->Z() / ptot;
    *tmp++ = ptot;
    *tmp++ = fBeamP4->E();

    // save particle 4-vectors
    for (UInt_t i = 0; i < fNParticle; i++)
    {  
        p4 = fParticle[i]->GetP4();
        ptot = (p4->Vect()).Mag();
        *tmp++ = p4->X() / ptot; 
        *tmp++ = p4->Y() / ptot;
        *tmp++ = p4->Z() / ptot;
        *tmp++ = ptot;
        *tmp++ = p4->E();
    }
    
    // write event to ntuple
    fNtuple->Fill(fBuffer);

    // increment event counter
    fNEvents++;
    
    // output event number (fancy, but slower)
    if (fNEvents % 10000 == 0)
    {
        printf("\r");
        printf("%9d events saved.", fNEvents);
        fflush(stdout);
    }
}

//______________________________________________________________________________
void TOMCMkinWriter::WriteFile(TList* inList)
{
    // Write the event ntuple and all members of the list 'inList' to the output 
    // file and close the file afterwards.

    printf("\n\nWriting to %s...\n", fFile->GetName());

    // write ntuple
    printf("  => %d events\n", fNEvents);
    fNtuple->Write();
    
    // write objects in list
    if (inList)
    {
        // loop over elements
        for (Int_t i = 0; i < inList->GetSize(); i++)
        {   
            TObject* obj = (TObject*) inList->At(i);

            // check if it has a name
            if (obj->InheritsFrom("TNamed")) 
            {   
                TNamed* named = (TNamed*) obj;
                printf("  => %s of type %s\n", named->GetName(), named->ClassName());
            }
            else printf("  => Object of type %s\n", obj->ClassName());
            
            // write to file
            obj->Write();
        }
    }

    // close file and free memory
    delete fNtuple;
    fNtuple = 0;
    delete fFile;
    fFile = 0;
  
    printf("Done!\n");
}

//______________________________________________________________________________
void TOMCMkinWriter::SetVertex(Double_t inX, Double_t inY, Double_t inZ)
{
    // Set the reaction vertex.

    if (fVertex) fVertex->SetXYZ(inX, inY, inZ);
}

//______________________________________________________________________________
void TOMCMkinWriter::SetBeamP4(TLorentzVector* inBeamP4)
{
    // Set the beam 4-vector.

    if (fBeamP4) fBeamP4->SetPxPyPzE(inBeamP4->Px(), inBeamP4->Py(), 
                                     inBeamP4->Pz(), inBeamP4->E());
}

//______________________________________________________________________________
void TOMCMkinWriter::SetBeamP4(Double_t inPx, Double_t inPy, Double_t inPz, Double_t inE)
{
    // Set the beam 4-vector.

    if (fBeamP4) fBeamP4->SetPxPyPzE(inPx, inPy, inPz, inE);
}

