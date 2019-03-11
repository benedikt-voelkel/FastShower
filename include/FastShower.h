#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <vector>

#include "VMCFastSim/FastSim.h"


class FastShower : public vmcfastsim::base::FastSim<FastShower>
{
  public:
    FastShower(const std::vector<double>& edges, double particleMass) : FastSim(vmcfastsim::base::EKernelMode::kSTEPS)
    {
      mBinEdges.clear();
      mBinEdges.resize(edges.size(), 0.);
      for(int i = 0; i < edges.size(); i++) {
        mBinEdges[i] = edges[i];
      }
      // Set seeds
      srand(time(NULL));
    }
    virtual ~FastShower() = default;

    virtual bool Process() override final
    {
      // Get random double between 0. and 1.
      double d = static_cast<double>(rand()) / RAND_MAX;
      int nParticles;
      // Find the upper edge off the bin whose number will correspond to the number of particles to be created
      for(nParticles = 0; nParticles < mBinEdges.size(); nParticles++) {
        if(d <= mBinEdges[nParticles]) {
          break;
        }
      }
      // Push binNumber electrons
      //for(int i = 0; i < nParticles; i++) {
      //  PushTrack(1, fCurrentParticleStatus->fParentId, 11, 1., 1., 1., 1., 0., 0., 0., 1., 1., 0., 0., TMCProcess, 1., 1);
      //}
      return true;
    }
    virtual void Stop() override final
    {
      // Do nothing for now
    }

  private:
    /// The bin edges
    std::vector<double> mBinEdges;
    /// Particle mass
    double mParticleMass;
};
