#ifndef RECINTERFACE_ITOWERTOOL_H
#define RECINTERFACE_ITOWERTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// datamodel
#include "datamodel/CaloCluster.h"

/** @class ITowerTool RecInterface/RecInterface/ITowerTool.h ITowerTool.h
 *
 *  Abstract interface to tower building tool
 *
 *  @author Anna Zaborowska
 */

class ITowerTool: virtual public IAlgTool {
public:
  DeclareInterfaceID(ITowerTool, 1, 0);

  virtual std::array<uint,2> prepareTowers(uint m_nEtaFinal, uint m_nPhiFinal) = 0;
  virtual uint buildTowers(std::vector<std::vector<float>> & aTowers) = 0;
  virtual void matchCells(float, float, uint, uint, fcc::CaloCluster& aEdmCluster) = 0;
  virtual float radiusForPosition() const = 0;/**  Get the tower IDs in eta.
   *   @param[in] aEta Position of the calorimeter cell in eta
   *   @return ID (eta) of a tower
   */
  virtual uint idEta(float aEta) const = 0;
  /**  Get the tower IDs in phi.
   *   @param[in] aPhi Position of the calorimeter cell in phi
   *   @return ID (phi) of a tower
   */
  virtual uint idPhi(float aPhi) const = 0;
  /**  Get the eta position of the centre of the tower.
   *   @param[in] aIdEta ID (eta) of a tower
   *   @return Position of the centre of the tower
   */
  virtual float eta(int aIdEta) const = 0;
  /**  Get the phi position of the centre of the tower.
   *   @param[in] aIdPhi ID (phi) of a tower
   *   @return Position of the centre of the tower
   */
  virtual float phi(int aIdPhi) const = 0;
};

#endif /* RECINTERFACE_ITOWERTOOL_H */
