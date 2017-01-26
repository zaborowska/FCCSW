#ifndef DETSEGMENTATION_GRIDRPHIETA_H
#define DETSEGMENTATION_GRIDRPHIETA_H

// FCCSW
#include "DetSegmentation/GridPhiEta.h"

/** GridRPhiEta Detector/DetSegmentation/DetSegmentation/GridRPhiEta.h GridRPhiEta.h
 *
 *  Segmentation in R, eta and phi.
 *  Based on GridPhiEta, addition of radial coordinate.
 *  This segmentation returns global position of the cell based on the cellID.
 *  It requires 'export LD_PRELOAD_PATH=$LD_PRELOAD_PATH:build.$BINARY_TAG/lib/libDetSegmentation.so'
 *
 *  @author    Anna Zaborowska
 */

namespace DD4hep {
namespace DDSegmentation {
class GridRPhiEta: public GridPhiEta {
public:
  /// default constructor using an arbitrary type
  GridRPhiEta(const std::string& aCellEncoding);
  /// destructor
  virtual ~GridRPhiEta() = default;

  /**  Determine the global position based on the cell ID.
   *   @param[in] aCellId ID of a cell.
   *   return Position.
   */
  virtual Vector3D position(const CellID& aCellID) const;
  /**  Determine the cell ID based on the position.
   *   @param[in] aLocalPosition (not used).
   *   @param[in] aGlobalPosition position in the global coordinates.
   *   @param[in] aVolumeId ID of a volume.
   *   return Cell ID.
   */
  virtual CellID cellID(const Vector3D& aLocalPosition, const Vector3D& aGlobalPosition, const VolumeID& aVolumeID) const;
  /**  Determine the radius based on the cell ID.
   *   @param[in] aCellId ID of a cell.
   *   return Radius.
   */
  double r(const CellID& aCellID) const;
  /**  Get the grid size in radial distance from the detector centre.
   *   return Grid size in radial distance.
   */
  inline double gridSizeR() const {
    return m_gridSizeR;
  }
  /**  Get the coordinate offset in radial distance.
   *   return The offset in R.
   */
  inline double offsetR() const {
    return m_offsetR;
  }
  /**  Get the field name for radial distance.
   *   return The field name for radial distance.
   */
  inline std::string fieldNameR() const {
    return m_rID;
  }
  /**  Set the grid size in radial distance.
   *   @param[in] aCellSize Cell size in radial distance.
   */
  void setGridSizeR(double aCellSize) {
    m_gridSizeR = aCellSize;
  }
  /**  Set the coordinate offset in radial distance.
   *   @param[in] aOffset Offset in radial distance.
   */
  inline void setOffsetR(double offset) {
    m_offsetR = offset;
  }
  /**  Set the field name used for radial distance.
   *   @param[in] aFieldName Field name for R.
   */
  inline void setFieldNameR(const std::string& fieldName) {
    m_rID = fieldName;
  }

private:
  /// determine the radial distance R based on the current cell ID
  double r() const;

  /// the grid size in r
  double m_gridSizeR;
  /// the coordinate offset in r
  double m_offsetR;
  /// the field name used for r
  std::string m_rID;
};
}
}
#endif /* DETSEGMENTATION_GRIDRPHIETA_H */
