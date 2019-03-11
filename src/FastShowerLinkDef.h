//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file  exampleEMELinkDef.h
/// \brief The CINT link definitions for example EME classes

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class  FastShower+;
#pragma link C++ class  FastShowerMCApplication+;
#pragma link C++ class  FastShowerMCStack+;
#pragma link C++ class  FastShowerDetectorConstruction+;
#pragma link C++ class  FastShowerCalorHit+;
#pragma link C++ class  FastShowerCalorimeterSD+;
#pragma link C++ class  FastShowerPrimaryGenerator+;
#pragma link C++ class  std::stack<TParticle*,deque<TParticle*> >+;

#endif
