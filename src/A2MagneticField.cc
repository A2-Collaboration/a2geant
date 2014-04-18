#include <fstream>
#include "A2MagneticField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"

//______________________________________________________________________________________________________
// G4FieldManager*  A2MagneticField::GetGlobalFieldManager()
// {
//   return G4TransportationManager::GetTransportationManager()->GetFieldManager();
// }

//______________________________________________________________________________________________________
A2MagneticField::A2MagneticField()
{
  fFieldMap = NULL;
  
  //
//   GetGlobalFieldManager()->SetDetectorField(this);
//   GetGlobalFieldManager()->CreateChordFinder(this);
}

//______________________________________________________________________________________________________
A2MagneticField::~A2MagneticField()
{
  // Deallocate memory
  if(fFieldMap)
  {
    for(int i=0; i<fPointsN[0]; ++i)
    {
      for(int j=0; j<fPointsN[1]; ++j)
      {
	delete [] fFieldMap[i][j];
      }
      delete [] fFieldMap[i];
    }
    delete [] fFieldMap;
  }
}

//______________________________________________________________________________________________________
// Read field map from a field_map.dat file and fill fFieldMap array
G4bool A2MagneticField::ReadFieldMap(const G4String &nameFileMap)
{
  // Print info
  G4cout.setf(std::ios_base::unitbuf);
  G4cout << "A2MagneticField::ReadFieldMap() Reading target magnetic field map...";
  
  // Open input stream
//   G4String nameFile = static_cast<G4String>(getenv("G4WORKDIR"))+"/A2/data/Target/field_map.dat";
  std::ifstream fin(nameFileMap);
  
  // In case of an error return FALSE
  if(!fin.good())
  {
    G4cout << " **ERROR** - File " << nameFileMap << " not found." << G4endl;
    return false;
  }
  
  // Read Xmin, Xmax, dX, Ymin, Ymax, dY, Zmin, Zmax, dZ and calculate number of points
  for(G4int i=0; i<3; ++i)
  {
    fin >> fPointMin[i] >> fPointMax[i] >> fPointStep[i];
    fPointStep[i] *= CLHEP::cm;
    fPointMin[i]   = fPointMin[i]*CLHEP::cm - fPointStep[i]/2.;
    fPointMax[i]   = fPointMax[i]*CLHEP::cm + fPointStep[i]/2.;
    fPointsN[i]    = ceil((fPointMax[i] - fPointMin[i])/fPointStep[i]);
  }
  
  // Allocate memory for fFieldMap
  fFieldMap = new A2Bvector**[fPointsN[0]];
  for(int i=0; i<fPointsN[0]; ++i)
  {
    fFieldMap[i] = new A2Bvector*[fPointsN[1]];
    for(int j=0; j<fPointsN[1]; ++j)
    {
      fFieldMap[i][j] = new A2Bvector[fPointsN[2]];
    }
  }
  
  // Read the magnetic field map
  G4double p[3], b[3];
  G4int iPoint[3];
  while(fin >> p[0] >> p[1] >> p[2] >> b[0] >> b[1] >> b[2])
  {
    // Calculate x,y,z indexes of the point
    iPoint[0] = GetPointIndex(p[0]*CLHEP::cm,fPointMin[0],fPointStep[0]); // index x
    iPoint[1] = GetPointIndex(p[1]*CLHEP::cm,fPointMin[1],fPointStep[1]); // index y
    iPoint[2] = GetPointIndex(p[2]*CLHEP::cm,fPointMin[2],fPointStep[2]); // index z
    
    // Fill the fFiledMap array
    fFieldMap[iPoint[0]][iPoint[1]][iPoint[2]][0] = b[0]*CLHEP::gauss; // Bx
    fFieldMap[iPoint[0]][iPoint[1]][iPoint[2]][1] = b[1]*CLHEP::gauss; // By
    fFieldMap[iPoint[0]][iPoint[1]][iPoint[2]][2] = b[2]*CLHEP::gauss; // Bz
  }
  
  // Close the field map file
  fin.close();
  
  // OK => return true
  G4cout << " OK" << G4endl;
  return true;
}

//______________________________________________________________________________________________________
// Get the magnetic induction vector value for the given point
void A2MagneticField::GetFieldValue(const G4double point[4], G4double *field) const
{
  // Set default magnetic field
  field[0] = field[1] = field[2] = 0.;
  
  // Check bounds
  if(point[0] < fPointMin[0] || point[0] > fPointMax[0]) return; // x
  if(point[1] < fPointMin[1] || point[1] > fPointMax[1]) return; // y
  if(point[2] < fPointMin[2] || point[2] > fPointMax[2]) return; // z
  
  // Get indexes of fFieldMap array which correspond to the point
  G4int iPoint[3] = {
                     GetPointIndex(point[0],fPointMin[0],fPointStep[0]), // x
                     GetPointIndex(point[1],fPointMin[1],fPointStep[1]), // y
                     GetPointIndex(point[2],fPointMin[2],fPointStep[2])  // z
                    };
  
  // Set field
  field[0] = fFieldMap[iPoint[0]][iPoint[1]][iPoint[2]][0];
  field[1] = fFieldMap[iPoint[0]][iPoint[1]][iPoint[2]][1];
  field[2] = fFieldMap[iPoint[0]][iPoint[1]][iPoint[2]][2];
}

//______________________________________________________________________________________________________
// Get index of the point "q" in an arithmetic progression with the first element "q0" and step "d"
// inline G4int A2MagneticField::GetPointIndex(const G4double &q, const G4double &q0, const G4double &d) const
// {
//   return (q-q0)/d;
// }
