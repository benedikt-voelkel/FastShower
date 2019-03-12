//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file FastShowerCalorimeterSD.cxx
/// \brief Implementation of the FastShowerCalorimeterSD class
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo \n
/// Id: ExN03CalorimeterSD.cc,v 1.6 2002/01/09 17:24:12 ranjard Exp \n
/// GEANT4 tag $Name:  $
///
/// \date 06/03/2002
/// \author I. Hrivnacova; IPN, Orsay

#include "FastShowerCalorimeterSD.h"
#include "FastShowerDetectorConstruction.h"
#include "FastShowerCalorHit.h"

#include <Riostream.h>
#include <TVirtualMC.h>
#include <TLorentzVector.h>
#include <TTree.h>
#include <TMCManager.h>

/// \cond CLASSIMP
ClassImp(FastShowerCalorimeterSD)
/// \endcond

using namespace std;

//_____________________________________________________________________________
FastShowerCalorimeterSD::FastShowerCalorimeterSD(const char* name,
                                     FastShowerDetectorConstruction* detector)
  : TNamed(name, ""),
    fMC(0),
    fDetector(detector),
    fCalCollection(0),
    fAbsorberVolId(0),
    fGapVolId(0),
    fVerboseLevel(1),
    fTotEGap(0.)
{
/// Standard constructor.
/// Create hits collection and an empty hit for each layer
/// As the copy numbers may start from 0 or 1 (depending on
/// geometry model, we create one more layer for this case.)
/// \param name      The calorimeter hits collection name
/// \param detector  The detector construction

  fCalCollection = new TClonesArray("FastShowerCalorHit", 500);
  for (Int_t i=0; i<fDetector->GetNbOfLayers()+1; i++)
    new ((*fCalCollection)[i]) FastShowerCalorHit();
}

//_____________________________________________________________________________
FastShowerCalorimeterSD::FastShowerCalorimeterSD(const FastShowerCalorimeterSD& origin,
                                     FastShowerDetectorConstruction* detector)
  : TNamed(origin),
    fMC(0),
    fDetector(detector),
    fCalCollection(0),
    fAbsorberVolId(origin.fAbsorberVolId),
    fGapVolId(origin.fGapVolId),
    fVerboseLevel(origin.fVerboseLevel),
    fTotEGap(origin.fTotEGap)
{
/// Copy constructor (for clonig on worker thread in MT mode).
/// Create hits collection and an empty hit for each layer
/// As the copy numbers may start from 0 or 1 (depending on
/// geometry model, we create one more layer for this case.)
/// \param origin    The source object (on master).
/// \param detector  The detector construction

  fCalCollection = new TClonesArray("FastShowerCalorHit", 500);
  for (Int_t i=0; i<fDetector->GetNbOfLayers()+1; i++)
    new ((*fCalCollection)[i]) FastShowerCalorHit();
}

//_____________________________________________________________________________
FastShowerCalorimeterSD::FastShowerCalorimeterSD()
  : TNamed(),
    fDetector(0),
    fCalCollection(0),
    fAbsorberVolId(0),
    fGapVolId(0),
    fVerboseLevel(1),
    fTotEGap(0.)
{
/// Default constructor
}

//_____________________________________________________________________________
FastShowerCalorimeterSD::~FastShowerCalorimeterSD()
{
/// Destructor

  if (fCalCollection) fCalCollection->Delete();
  delete fCalCollection;
}

//
// private methods
//

//_____________________________________________________________________________
FastShowerCalorHit* FastShowerCalorimeterSD::GetHit(Int_t i) const
{
/// \return   The hit for the specified layer.
/// \param i  The layer number

  return (FastShowerCalorHit*)fCalCollection->At(i);
}

//_____________________________________________________________________________
void  FastShowerCalorimeterSD::ResetHits()
{
/// Reset all hits in the hits collection.

  for (Int_t i=0; i<fCalCollection->GetEntriesFast(); i++)
    GetHit(i)->Reset();
}

//
// public methods
//

//_____________________________________________________________________________
void FastShowerCalorimeterSD::Initialize()
{
/// Register hits collection in the Root manager;
/// set sensitive volumes.

  // Keep the pointer to TVirtualMC object as a data member
  // to avoid a possible performance penalty due to a frequent retrieval
  // from the thread-local storage
  if(TMCManager::Instance()) {
    TMCManager::Instance()->ConnectEnginePointer(fMC);
  } else {
    fMC = gMC;
  }

  fAbsorberVolId = fMC->VolId("ABSO");
  fGapVolId = fMC->VolId("GAPX");

  if ( fAbsorberVolId == 0 && fGapVolId == 0 ) {
    // Volume names are different in B4 detector construction
    fAbsorberVolId = fMC->VolId("Abso");
    fGapVolId = fMC->VolId("Gap");
  }
}

//_____________________________________________________________________________
Bool_t FastShowerCalorimeterSD::ProcessHits()
{
/// Account energy deposit and track lengths for each layer in its hit.

  Int_t copyNo;
  Int_t id = fMC->CurrentVolID(copyNo);

  if (id != fAbsorberVolId  &&  id != fGapVolId )
    return false;

  fMC->CurrentVolOffID(2, copyNo);
  //cout << "Got copyNo "<< copyNo << " " << fMC->CurrentVolPath() << endl;

  Double_t edep = fMC->Edep();

  Double_t step = 0.;
  if (fMC->TrackCharge() != 0.) step = fMC->TrackStep();

  if ( ! GetHit(copyNo) ) {
    std::cerr << "No hit found for layer with copyNo = " << copyNo << endl;
    return false;
  }

  if (id == fAbsorberVolId) {
    GetHit(copyNo)->AddAbs(edep,step);
  }

  if (id == fGapVolId) {
    GetHit(copyNo)->AddGap(edep,step);
  }

  return true;
}

//_____________________________________________________________________________
void FastShowerCalorimeterSD::EndOfEvent()
{
/// Print hits collection (if verbose) and reset hits afterwards.

  if (fVerboseLevel>1)  Print();

  // Reset hits collection
  ResetHits();
}

//_____________________________________________________________________________
void FastShowerCalorimeterSD::SetTotalEdepGap(Double_t sumGapHits)
{
  fTotEGap = sumGapHits;
}

//_____________________________________________________________________________
Double_t FastShowerCalorimeterSD::GetTotalEdepGap() const
{
  if(fTotEGap > 0.) {
    return fTotEGap;
  }
  Double_t totEGap=0.;
  for (Int_t i=0; i<fCalCollection->GetEntriesFast(); i++) {
    totEGap += GetHit(i)->GetEdepGap();
  }
  return totEGap;
}

//_____________________________________________________________________________
void FastShowerCalorimeterSD::Print(Option_t* /*option*/) const
{
/// Print the hits collection.

   Int_t nofHits = fCalCollection->GetEntriesFast();

   cout << "\n-------->Hits Collection: in this event: " << endl;

   for (Int_t i=0; i<nofHits; i++) (*fCalCollection)[i]->Print();
}

//_____________________________________________________________________________
void FastShowerCalorimeterSD::PrintTotal() const
{
/// Print the total values for all layers.

  Double_t totEAbs=0.;
  Double_t totLAbs=0.;
  Double_t totEGap=0.;
  Double_t totLGap=0.;

  Int_t nofHits = fCalCollection->GetEntriesFast();
  for (Int_t i=0; i<nofHits; i++) {
    totEAbs += GetHit(i)->GetEdepAbs();
    totLAbs += GetHit(i)->GetTrakAbs();
    totEGap += GetHit(i)->GetEdepGap();
    totLGap += GetHit(i)->GetTrakGap();
  }

  if(fTotEGap > 0.) {
    totEGap = fTotEGap;
  }

  cout << "   Absorber: total energy (MeV): "
       << setw(7) << totEAbs * 1.0e03
       << "       total track length (cm):  "
       << setw(7) << totLAbs
       << endl
       << "   Gap:      total energy (MeV): "
       << setw(7) << totEGap * 1.0e03
       << "       total track length (cm):  "
       << setw(7) << totLGap
       << endl;
}
