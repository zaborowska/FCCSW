#ifndef SIMG4COMPONENTS_G4FTFPBERT_H
#define SIMG4COMPONENTS_G4FTFPBERT_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimG4Interface/IG4PhysicsList.h"

/** @class G4FtfpBert SimG4Components/src/G4FtfpBert.h G4FtfpBert.h
 *
 *  FTFP_BERT physics list tool.
 *  Property \b"useCoupledTransportation" can be set from the job options file. Default: false.
 *
 *  @author Anna Zaborowska
 */

class G4FtfpBert: public GaudiTool, virtual public IG4PhysicsList {
public:
  G4FtfpBert(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~G4FtfpBert();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /** Get the physics list.
   *  @return pointer to G4VModularPhysicsList
   */
  virtual G4VModularPhysicsList* getPhysicsList();
private:
  /// Flag indicating if coupled transportation should be used. Set by job options.
  bool m_coupledTransport;

};

#endif /* SIMG4COMPONENTS_G4FTFPBERT_H */
