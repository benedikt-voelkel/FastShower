//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file FastShowerMCApplication.cxx
/// \brief Implementation of the FastShowerMCApplication class
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo
///
/// \date 06/03/2002
/// \author I. Hrivnacova; IPN, Orsay

#include "FastShowerMCApplication.h"
#include "FastShowerMCStack.h"
#include "FastShowerPrimaryGenerator.h"

#include <TMCManager.h>

#include <TROOT.h>
#include <TFile.h>
#include <TH1D.h>
#include <TInterpreter.h>
#include <TTimer.h>
#include <TVirtualMC.h>
#include <TRandom.h>
#include <TPDGCode.h>
#include <TVector3.h>
#include <Riostream.h>
#include <TGeoManager.h>
#include <TGeoUniformMagField.h>
#include <TVirtualGeoTrack.h>
#include <TParticle.h>

#include <TLorentzVector.h>

using namespace std;

/// \cond CLASSIMP
ClassImp(FastShowerMCApplication)
/// \endcond

//_____________________________________________________________________________
FastShowerMCApplication::FastShowerMCApplication(const char *name, const char *title,
                                     Bool_t isMulti, Bool_t splitSimulation)
  : TVirtualMCApplication(name,title),
    fPrintModulo(1),
    fEventNo(0),
    fVerbose(0),
    fStack(0),
    fDetConstruction(0),
    fCalorimeterSD(0),
    fPrimaryGenerator(0),
    fMagField(0),
    fOldGeometry(kFALSE),
    fIsControls(kFALSE),
    fIsMaster(kTRUE),
    fIsMultiRun(isMulti),
    fSplitSimulation(splitSimulation),
    fG3Id(-1),
    fG4Id(-1),
    mStepsX("histStepsX", "histStepsX", 100, -10., 10.),
    mStepsY("histStepsY", "histStepsY", 50, -6., 6.),
    mStepsZ("histStepsZ", "histStepsZ", 50, -6., 6.),
    mPVElectronsX("histPVElectronsX", "histPVElectronsX", 50, -10., 10.),
    mPVElectronsY("histPVElectronsY", "histPVElectronsY", 50, -6., 6.),
    mPVElectronsZ("histPVElectronsZ", "histPVElectronsZ", 50, -6., 6.),
    mPMomElectronsX("histMomElectronsX", "histMomElectronsX", 50, 0., 0.005),
    mPMomElectronsY("histMomElectronsY", "histMomElectronsY", 50, 0., 0.005),
    mPMomElectronsZ("histMomElectronsZ", "histMomElectronsZ", 50, 0., 0.005)
{
/// Standard constructor
/// \param name   The MC application name
/// \param title  The MC application description

  if(splitSimulation && !isMulti) {
    Fatal("FastShowerMCApplication",
          "Cannot split simulation between engines without \"isMulti\" being switched on");
  }

  // Create a user stack
  fStack = new FastShowerMCStack(1000);

  // Create detector construction
  fDetConstruction = new FastShowerDetectorConstruction();

  // Create a calorimeter SD
  fCalorimeterSD = new FastShowerCalorimeterSD("Calorimeter", fDetConstruction);

  // Create a primary generator
  fPrimaryGenerator = new FastShowerPrimaryGenerator(fStack);

  // Constant magnetic field (in kiloGauss)
  fMagField = new TGeoUniformMagField();

  if(isMulti) {
    RequestMCManager();
    fMCManager->SetUserStack(fStack);
  }
}

//_____________________________________________________________________________
FastShowerMCApplication::FastShowerMCApplication(const FastShowerMCApplication& origin)
  : TVirtualMCApplication(origin.GetName(),origin.GetTitle()),
    fPrintModulo(origin.fPrintModulo),
    fEventNo(0),
    fVerbose(origin.fVerbose),
    fStack(0),
    fDetConstruction(origin.fDetConstruction),
    fCalorimeterSD(0),
    fPrimaryGenerator(0),
    fMagField(0),
    fOldGeometry(origin.fOldGeometry),
    fIsMaster(kFALSE),
    fIsMultiRun(origin.fIsMultiRun),
    fSplitSimulation(origin.fSplitSimulation),
    fG3Id(origin.fG3Id),
    fG4Id(origin.fG4Id)
{
/// Copy constructor for cloning application on workers (in multithreading mode)
/// \param origin   The source MC application

  // Create new user stack
  fStack = new FastShowerMCStack(1000);

  // Create a calorimeter SD
  fCalorimeterSD
    = new FastShowerCalorimeterSD(*(origin.fCalorimeterSD), fDetConstruction);

  // Create a primary generator
  fPrimaryGenerator
    = new FastShowerPrimaryGenerator(*(origin.fPrimaryGenerator), fStack);

  // Constant magnetic field (in kiloGauss)
  fMagField = new TGeoUniformMagField(origin.fMagField->GetFieldValue()[0],
                                      origin.fMagField->GetFieldValue()[1],
                                      origin.fMagField->GetFieldValue()[2]);
  if(fIsMultiRun) {
    RequestMCManager();
    fMCManager->SetUserStack(fStack);
  }
}

//_____________________________________________________________________________
FastShowerMCApplication::FastShowerMCApplication()
  : TVirtualMCApplication(),
    fPrintModulo(1),
    fEventNo(0),
    fStack(0),
    fDetConstruction(0),
    fCalorimeterSD(0),
    fPrimaryGenerator(0),
    fMagField(0),
    fOldGeometry(kFALSE),
    fIsControls(kFALSE),
    fIsMaster(kTRUE),
    fIsMultiRun(kFALSE),
    fSplitSimulation(kFALSE),
    fG3Id(-1),
    fG4Id(-1)
{
/// Default constructor
}

//_____________________________________________________________________________
FastShowerMCApplication::~FastShowerMCApplication()
{
/// Destructor

  //cout << "FastShowerMCApplication::~FastShowerMCApplication " << this << endl;

  delete fStack;
  if ( fIsMaster) delete fDetConstruction;
  delete fCalorimeterSD;
  delete fPrimaryGenerator;
  delete fMagField;
  if(!fIsMultiRun) {
    delete fMC;
  }

  //cout << "Done FastShowerMCApplication::~FastShowerMCApplication " << this << endl;
}

//
// private methods
//

//_____________________________________________________________________________
void FastShowerMCApplication::RegisterStack() const
{
/// Register stack in the Root manager.

}

//
// public methods
//

//_____________________________________________________________________________
void FastShowerMCApplication::InitMC(const char* setup)
{
/// Initialize MC.
/// The selection of the concrete MC is done in the macro.
/// \param setup The name of the configuration macro

  fVerbose.InitMC();

  if ( TString(setup) != "" ) {
    gROOT->LoadMacro(setup);
    gInterpreter->ProcessLine("Config()");
    if ( ! fMC ) {
      Fatal("InitMC",
            "Processing Config() has failed. (No MC is instantiated.)");
    }
  }

  fMC->SetStack(fStack);
  fMC->SetRootGeometry();
  fMC->SetMagField(fMagField);
  fMC->Init();
  fMC->BuildPhysics();

  //RegisterStack();

  Info("InitMC", "Single run initialised");
}

//_____________________________________________________________________________
void FastShowerMCApplication::InitMC(std::initializer_list<const char*> setupMacros)
{
/// Initialize MC.
/// The selection of the concrete MC is done in the macro.
/// \param setup The name of the configuration macro

  fVerbose.InitMC();

  if(!fIsMultiRun) {
    Fatal("InitMC",
          "Initialisation of multiple engines not supported in single run");
  }

  if ( setupMacros.size() > 0 ) {
    for(const char* setup : setupMacros) {
      gROOT->LoadMacro(setup);
      gInterpreter->ProcessLine("Config()");
      if ( ! fMC ) {
        Fatal("InitMC",
              "Processing Config() has failed. (No MC is instantiated.)");
      }
      //gInterpreter->UnloadAllSharedLibraryMaps();
      gInterpreter->UnloadFile(setup);
    }
  }

  fMCManager->Init([this](TVirtualMC* mc){
                                      mc->SetRootGeometry();
                                      mc->SetMagField(this->fMagField);
                                      mc->Init();
                                      mc->BuildPhysics();
                                    });
  //RegisterStack();

  Info("InitMC", "Multi run initialised");
  fG3Id = fMCManager->GetEngineId("TGeant3TGeo");
  fG4Id = fMCManager->GetEngineId("TGeant4");
  std::cout << "Engine IDs\n"
            << "TGeant3TGeo: " << fG3Id << "\n"
            << "TGeant4: " << fG4Id << std::endl;
}

//_____________________________________________________________________________
void FastShowerMCApplication::RunMC(Int_t nofEvents)
{
/// Run MC.
/// \param nofEvents Number of events to be processed

  fVerbose.RunMC(nofEvents);

  // Prepare a timer
  TTimer timer;

  if(!fIsMultiRun) {
    Info("RunMC", "Start single run");
    std::cout << "Simulation entirely done with engine "
              << fMC->GetName() << std::endl;
    timer.Start();
    fMC->ProcessRun(nofEvents);
  } else {
    Info("RunMC", "Start multi run");
    if(fSplitSimulation) {
      std::cout << "GAPX simulated with engine "
                << fMCManager->GetEngine(0)->GetName() << "\n"
                << "ABSO simulated with engine "
                << fMCManager->GetEngine(1)->GetName() << std::endl;
    } else {
      std::cout << "Simulation entirely done with engine "
                << fMCManager->GetCurrentEngine()->GetName() << std::endl;
    }
    timer.Start();
    fMCManager->Run(nofEvents);
  }
  timer.Stop();
  Info("RunMC", "Transport finished.");
  timer.Print();
  FinishRun();
}

//_____________________________________________________________________________
void FastShowerMCApplication::FinishRun()
{
/// Finish MC run.

  fVerbose.FinishRun();
}

//_____________________________________________________________________________
void FastShowerMCApplication::ExportGeometry(const char* path) const
{
  if(!gGeoManager || !gGeoManager->IsClosed()) {
    Warning("ExportGeometry", "TGeoManager not existing or geometry not closed yet.");
    return;
  }
  Info("ExportGeometry", "Export geometry to %s.", path);
  gGeoManager->Export(path);
}

//_____________________________________________________________________________
TVirtualMCApplication* FastShowerMCApplication::CloneForWorker() const
{
  return new FastShowerMCApplication(*this);
}

//_____________________________________________________________________________
void FastShowerMCApplication::InitForWorker() const
{
  //cout << "FastShowerMCApplication::InitForWorker " << this << endl;

  // Set data to MC
  fMC->SetStack(fStack);
  fMC->SetMagField(fMagField);

  //RegisterStack();
}

//_____________________________________________________________________________
void FastShowerMCApplication::FinishWorkerRun() const
{
  //cout << "FastShowerMCApplication::FinishWorkerRun: " << endl;
}

//_____________________________________________________________________________
void FastShowerMCApplication::ReadEvent(Int_t i)
{
/// Read \em i -th event and prints hits.
/// \param i The number of event to be read

  //RegisterStack();
}

//_____________________________________________________________________________
void FastShowerMCApplication::ConstructGeometry()
{
/// Construct geometry using detector contruction class.
/// The detector contruction class is using TGeo functions or
/// TVirtualMC functions (if oldGeometry is selected)

  fVerbose.ConstructGeometry();

  if ( fOldGeometry ) {
    Fatal("ConstructGeometry", "Cannot run with old geomtry");
  }
  fDetConstruction->ConstructMaterials();
  fDetConstruction->ConstructGeometry();
  //TGeoManager::Import("geometry.root");
  //fMC->SetRootGeometry();
}

//_____________________________________________________________________________
void FastShowerMCApplication::InitGeometry()
{
/// Initialize geometry

  fVerbose.InitGeometry();

  fDetConstruction->SetCuts();

  if ( fIsControls )
    fDetConstruction->SetControls();

  fCalorimeterSD->Initialize();
}

//_____________________________________________________________________________
void FastShowerMCApplication::AddParticles()
{
/// Example of user defined particle with user defined decay mode

  fVerbose.AddParticles();

  // Define the 2 body  phase space decay  for He5
  Int_t mode[6][3];
  Float_t bratio[6];

  for (Int_t kz = 0; kz < 6; kz++) {
     bratio[kz] = 0.;
     mode[kz][0] = 0;
     mode[kz][1] = 0;
     mode[kz][2] = 0;
  }
  bratio[0] = 100.;
  mode[0][0] = kNeutron;    // neutron (2112)
  mode[0][1] = 1000020040 ; // alpha

  // Overwrite a decay mode already defined in MCs
  // Kaon Short: 310 normally decays in two modes
  // pi+, pi-  68.61 %
  // pi0, pi0  31.39 %
  // and we force only the mode pi0, pi0

  Int_t mode2[6][3];
  Float_t bratio2[6];

  for (Int_t kz = 0; kz < 6; kz++) {
     bratio2[kz] = 0.;
     mode2[kz][0] = 0;
     mode2[kz][1] = 0;
     mode2[kz][2] = 0;
  }
  bratio2[0] = 100.;
  mode2[0][0] = kPi0;   // pi0 (111)
  mode2[0][1] = kPi0 ;  // pi0 (111)

  // Define particle
  fMC->DefineParticle(1000020050, "He5", kPTHadron,
                      5.03427 , 2.0, 0.002 ,
                      "Ion", 0.0, 0, 1, 0, 0, 0, 0, 0, 5, kFALSE);
  fMC->SetDecayMode(1000020050 ,bratio,mode);
  fMC->SetDecayMode(kK0Short, bratio2, mode2);
}

//_____________________________________________________________________________
void FastShowerMCApplication::AddIons()
{
/// Example of user defined ion

  fVerbose.AddIons();
  fMC->DefineIon("MyIon", 34, 70, 12, 0.);

}

//_____________________________________________________________________________
void FastShowerMCApplication::GeneratePrimaries()
{
/// Fill the user stack (derived from TVirtualMCStack) with primary particles.

  fVerbose.GeneratePrimaries();

  TVector3 origin(fDetConstruction->GetWorldSizeX(),
                  fDetConstruction->GetCalorSizeYZ(),
                  fDetConstruction->GetCalorSizeYZ());

  fPrimaryGenerator->GeneratePrimaries(origin);
}

//_____________________________________________________________________________
void FastShowerMCApplication::BeginEvent()
{
/// User actions at beginning of event

  fVerbose.BeginEvent();

  // Clear TGeo tracks (if filled)
  if (   !fIsMultiRun &&
         TString(fMC->GetName()) == "TGeant3TGeo" &&
         gGeoManager->GetListOfTracks() &&
         gGeoManager->GetTrack(0) &&
       ((TVirtualGeoTrack*)gGeoManager->GetTrack(0))->HasPoints() ) {

       gGeoManager->ClearTracks();
       //if (gPad) gPad->Clear();
  }

  fEventNo++;
  if (fEventNo % fPrintModulo == 0) {
    cout << "\n---> Begin of event: " << fEventNo << endl;
    // ??? How to do this in VMC
    // HepRandom::showEngineStatus();
  }
}

//_____________________________________________________________________________
void FastShowerMCApplication::BeginPrimary()
{
/// User actions at beginning of a primary track.
/// If test for user defined decay is activated,
/// the primary track ID is printed on the screen.

  fVerbose.BeginPrimary();

  if ( fPrimaryGenerator->GetUserDecay() ) {
    cout << "   Primary track ID = "
         << fStack->GetCurrentTrackNumber() << endl;
  }
}

//_____________________________________________________________________________
void FastShowerMCApplication::PreTrack()
{
/// User actions at beginning of each track
/// If test for user defined decay is activated,
/// the decay products of the primary track (K0Short)
/// are printed on the screen.

  fVerbose.PreTrack();

  TParticle* particle = fStack->GetCurrentTrack();
  if(particle->GetPdgCode() == 11) {
    mPVElectronsX.Fill(particle->Vx());
    mPVElectronsY.Fill(particle->Vy());
    mPVElectronsZ.Fill(particle->Vz());

    mPMomElectronsX.Fill(particle->Px());
    mPMomElectronsY.Fill(particle->Py());
    mPMomElectronsZ.Fill(particle->Pz());

  }

  // print info about K0Short decay products
  if ( fPrimaryGenerator->GetUserDecay() ) {
    Int_t parentID = fStack->GetCurrentParentTrackNumber();

    if ( parentID >= 0 &&
         fStack->GetParticle(parentID)->GetPdgCode() == kK0Short  &&
         fStack->GetCurrentTrack()->GetUniqueID() == kPDecay ) {
         // The production process is saved as TParticle unique ID
         // via FastShowerMCStack

      cout << "      Current track "
           << fStack->GetCurrentTrack()->GetName()
           << "  is a decay product of Parent ID = "
           << fStack->GetCurrentParentTrackNumber() << endl;
    }
  }
}

//_____________________________________________________________________________
void FastShowerMCApplication::Stepping()
{
/// User actions at each step

  // Work around for Fluka VMC, which does not call
  // MCApplication::PreTrack()
  //
  //cout << "FastShowerMCApplication::Stepping" << this << endl;
  static Int_t trackId = 0;
  if ( TString(fMC->GetName()) == "TFluka" &&
       fMC->GetStack()->GetCurrentTrackNumber() != trackId ) {
    fVerbose.PreTrack();
    trackId = fMC->GetStack()->GetCurrentTrackNumber();
  }

  fVerbose.Stepping();

  fCalorimeterSD->ProcessHits();

  TLorentzVector pos;
  TLorentzVector mom;

  fMC->TrackPosition(pos);
  fMC->TrackMomentum(mom);


  if(fVerbose.GetLevel() > 0) {

    std::cout << "Current engine " << fMC->GetName()
              << " and current volume name " << fMC->CurrentVolName()
              << "\n"
              << "Track ID=" << fStack->GetCurrentTrackNumber() << "\n"
              << "Step number=" << fMC->StepNumber() << "\n"
              << "Track length=" << fMC->TrackLength() << "\n"
              << "Position\n"
              << "T=" << pos.T() << ", X=" << pos.X() << ", Y=" << pos.Y()
              << ", Z=" << pos.Z() << "\n"
              << "Momentum\n"
              << "E=" << mom.T() << ", Px=" << mom.X() << ", Py=" << mom.Y()
              << ", Pz=" << mom.Z() << std::endl;
  }

  // Count pdg steps
  int pdg = fMC->TrackPid();
  if(mStepsPerPdg.find(pdg) == mStepsPerPdg.end()) {
    mStepsPerPdg[pdg] = 0;
  }
  mStepsPerPdg[pdg]++;

  mStepsX.Fill(pos.X());
  mStepsY.Fill(pos.Y());
  mStepsZ.Fill(pos.Z());

  // Now transfer track
  if(fSplitSimulation) {
    Int_t targetId = -1;
    if(fMC->GetId() == 0 && strcmp(fMC->CurrentVolName(), "ABSO") == 0)
    {
      targetId = 1;
    }
    else if(fMC->GetId() == 1 && strcmp(fMC->CurrentVolName(), "GAPX") == 0)
    {
      targetId = 0;
    }
    if(targetId > -1) {
      if(fVerbose.GetLevel() > 0) {
        Info("Stepping", "Transfer track");
      }
      fMCManager->TransferTrack(targetId);
    }
  }
}

//_____________________________________________________________________________
void FastShowerMCApplication::PostTrack()
{
/// User actions after finishing of each track
  fVerbose.PostTrack();
}

//_____________________________________________________________________________
void FastShowerMCApplication::FinishPrimary()
{
/// User actions after finishing of a primary track

  fVerbose.FinishPrimary();

  if ( fPrimaryGenerator->GetUserDecay() ) {
    cout << endl;
  }
}

//_____________________________________________________________________________
void FastShowerMCApplication::FinishEvent()
{
/// User actions after finishing of an event

  fVerbose.FinishEvent();

  // Geant3 + TGeo
  // (use TGeo functions for visualization)
  if ( !fIsMultiRun && TString(fMC->GetName()) == "TGeant3TGeo") {

     // Draw volume
     gGeoManager->SetVisOption(0);
     gGeoManager->SetTopVisible();
     gGeoManager->GetTopVolume()->Draw();

     // Draw tracks (if filled)
     // Available when this feature is activated via
     // fMC->SetCollectTracks(kTRUE);
     if ( gGeoManager->GetListOfTracks() &&
          gGeoManager->GetTrack(0) &&
        ((TVirtualGeoTrack*)gGeoManager->GetTrack(0))->HasPoints() ) {

       gGeoManager->DrawTracks("/*");  // this means all tracks
    }
  }


  if (fEventNo % fPrintModulo == 0)
    fCalorimeterSD->PrintTotal();

  fCalorimeterSD->EndOfEvent();

  insertIntoVector(mNElectrons, fStack->GetNumberOfParticles(11));
  insertIntoVector(mNPositrons, fStack->GetNumberOfParticles(-11));
  insertIntoVector(mNPhotons, fStack->GetNumberOfParticles(22));

  fStack->Reset();
}

void FastShowerMCApplication::WriteHistograms(const std::string& fileName) const
{
  TFile file(fileName.c_str(), "RECREATE");
  TH1D histNElectrons("histNElectrons", "histNElectrons", mNElectrons.size(), 0., mNElectrons.size() + 1);
  vectorToHistogram(mNElectrons, histNElectrons);
  TH1D histNPositrons("histNPositrons", "histNPositrons", mNPositrons.size(), 0., mNPositrons.size() + 1);
  vectorToHistogram(mNPositrons, histNPositrons);
  TH1D histNPhotons("histNPhotons", "histNPhotons", mNPhotons.size(), 0., mNPhotons.size() + 1);
  vectorToHistogram(mNPhotons, histNPhotons);
  TH1D histStepsPerPDG("histStepsPerPDG", "histStepsPerPDG", 1, 0., 1.);
  std::unordered_map<int, int> stepsPerPDGTmp;

  // Do not store crazy things
  for(auto& iter : mStepsPerPdg) {
    if(std::abs(iter.first) <= 3000) {
      stepsPerPDGTmp[iter.first] = iter.second;
    }
  }

  mapToHistogram(stepsPerPDGTmp, histStepsPerPDG);
  file.WriteTObject(&mStepsX);
  file.WriteTObject(&mStepsY);
  file.WriteTObject(&mStepsZ);

  file.WriteTObject(&mPVElectronsX);
  file.WriteTObject(&mPVElectronsY);
  file.WriteTObject(&mPVElectronsZ);

  file.WriteTObject(&mPMomElectronsX);
  file.WriteTObject(&mPMomElectronsY);
  file.WriteTObject(&mPMomElectronsZ);

  file.Write();
  file.Close();
}
