#include <random>
#include <vector>
#include <functional>

#include <iostream>

#include "VMCFastSim/FastSim.h"


class FastShower : public vmcfastsim::base::FastSim<FastShower>
{
  public:
    FastShower(double mean, double sigma, std::function<void(double)> f)
      : FastSim(vmcfastsim::base::EKernelMode::kHITS, "FastShower"), mDistribution(mean, sigma), mStoreHit(f)
    {
    }
    virtual ~FastShower() = default;

    virtual bool Process() override final
    {
      if(GetCurrentParticle()->GetPdgCode() == 2212) {
        std::cerr << "Role the dice" << std::endl;
        mStoreHit(mDistribution(mGenerator));
      }
      return true;
    }
    virtual void Stop() override final
    {
      // Nothing to be done yet
    }

  private:
    /// The bin edges
    std::default_random_engine mGenerator;
    std::normal_distribution<double> mDistribution;
    std::function<void(double)> mStoreHit;
};
