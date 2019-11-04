# A custom user implementation based on `TVirtualMC`

**Disclaimer:** This is not fast simulation in the real sense but serves as a proof-of-concept implementation using `TVirtualMC` as a base class for a custom user implementation of a VMC to be injected into a detector simulation.

The geometry and setup is adapted from the [GEANT4_VMC example E03](https://github.com/vmc-project/geant4_vmc/tree/master/examples/E03).

## Installation

The small package is set up using CMake in a dedicated build directory via

```bash
mkdir $BUILD_DIR
cd $BUILD_DIR
cmake $SOURCE_DIR \
      -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
      -DROOT_DIR=$ROOT_CMAKE_CONFIG_DIR \
      -DVMC_DIR=$VMC_CMAKE_CONFIG_DIR
      -Dpythia6_DIR="$PYTHIA6_INSTALL_DIR" \
      -DGeant3_DIR="$GEANT3VMC_CMAKE_CONFIG_DIR" \
      -DGeant4_DIR="$*GEANT4_CMAKE_CONFIG_DIR" \
      -DGeant4VMC_DIR="$GEANT4VMC_CMAKE_CONFIG_DIR" \
      -DVMCFastSim_DIR="$VMCFastSim_INSTALL_DIR" \
      -DVGM_DIR="$VGM_CMAKE_CONFIG_DIR"
make && make install
```

## Brief description

The `FastShower` class derives from the base class provided in [VMCFastSim](https://github.com/benedikt-voelkel/VMCFastSim), specifically the one to be found [here](https://github.com/benedikt-voelkel/VMCFastSim/blob/master/include/VMCFastSim/FastSim.h).

It is an extremely brief implementation so it is directly referred to the source code of [`FastSim` class](https://github.com/benedikt-voelkel/FastShower/blob/master/include/FastShower.h).

An executable is built from [this](https://github.com/benedikt-voelkel/FastShower/blob/master/src/runFastShower.cxx). The executable provides some verbosity, command line arguments and help messages 

Some further description of what this executable does can be found this [CHEP contribution](https://indico.cern.ch/event/773049/contributions/3474744)

## A possible run scenario

1. `FastShower/bin/runFastShower --mode single --out histograms_full.root --nevents 100000`
1. `FastShower/bin/runFastShower --mode mixed-fast --in histograms_full.root --out histograms_fast.root --nevents 100000`

The first line runs a GEANT4 full simulation extracting a fit for the total energy deposit in a basic sampling calorimeter. A Gaussian is fitted to the energy distribution. The second line takes the corresponding ROOT file as an input, reads the fit and draws the energy deposit from this distribution as soon as the particle hits the calorimeter.
