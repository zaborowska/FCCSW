#ifndef SIMG4FAST_G4PARTICLESMEARTKLAYOUT_H
#define SIMG4FAST_G4PARTICLESMEARTKLAYOUT_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
class IRndmGenSvc;

//ROOT
#include "TGraph.h"

// FCCSW
#include "SimG4Interface/IG4ParticleSmearTool.h"

/** @class G4ParticleSmearTklayout SimG4Fast/src/components/G4ParticleSmearTklayout.h G4ParticleSmearTklayout.h
 *
 *  Tklayout particle smearing tool.
 *  Smears the momentum/energy of the particle following the Gaussian distribution.
 *  TODO about where resolutions come from
 *
 *  @author Anna Zaborowska
 */

class G4ParticleSmearTklayout: public GaudiTool, virtual public IG4ParticleSmearTool {
public:
  explicit G4ParticleSmearTklayout(const std::string& type , const std::string& name,
    const IInterface* parent);
  virtual ~G4ParticleSmearTklayout();

  /**  Initialize the tool and a random number generator.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;

  /**  Smear the momentum of the particle
   *   @param aMom Particle momentum to be smeared.
   *   @param[in] aPdg Particle PDG code.
   *   @return status code
   */
  virtual StatusCode smearMomentum(CLHEP::Hep3Vector& aMom, int aPdg = 0) final;

  /**  Smear the energy of the particle
   *   @param aEn Particle energy to be smeared.
   *   @param[in] aPdg Particle PDG code.
   *   @return status code
   */
  virtual StatusCode smearEnergy(double& aEn, int aPdg = 0) final;
  StatusCode readResolutions();
private:
  std::map<double, TGraph> m_momentumResolutions;
  /// Random Number Service
  IRndmGenSvc* m_randSvc;
  /// Gaussian random number generator used for smearing with a constant resolution (m_sigma)
  Rndm::Numbers m_gauss;
};

#endif /* SIMG4FAST_G4PARTICLESMEARSIMPLE_H */