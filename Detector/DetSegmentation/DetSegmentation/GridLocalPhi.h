#ifndef DETSEGMENTATION_GRIDLOCALPHI_H
#define DETSEGMENTATION_GRIDLOCALPHI_H

#include "DDSegmentation/Segmentation.h"

/* #include "DDSegmentation/SegmentationUtil.h" */
#include "TVector3.h"
#include <cmath>

/** GridLocalPhi Detector/DetSegmentation/DetSegmentation/GridLocalPhi.h GridLocalPhi.h
 *
 *  Segmentation in phi of volume.
 *  Local phi coordinate is calculated as a difference between the azimuthal position of the volume
 *  and the azimuthal angle of the energy deposit in the global coordinates.
 *  The radial axis from the world's centre to the volume's centre is the border between localPhi ID=0 and ID=1
 *  (unlike other segmentations, e.g. CarthesianGridXY where x and y axes are in the middle of the bin with ID=0).
 *  In particular, if 'grid_size_phi' is larger than the half of the radial size of the volume,
 *  this segmentation divides the volume into two cells.
 *
 *  @author    Anna Zaborowska
 */

namespace DD4hep {
namespace DDSegmentation {
class GridLocalPhi: public Segmentation {
public:
  /// default constructor using an arbitrary type
  GridLocalPhi(const std::string& aCellEncoding);
  /// destructor
  virtual ~GridLocalPhi() = default;

  /**  Determine the global position based on the cell ID.
   *   @warning This segmentation has no knowledge of radius, so radius = 1 is taken into calculations.
   *   @param[in] aCellId ID of a cell.
   *   return Position (radius = 1).
   */
  virtual Vector3D position(const CellID& aCellID) const;
  /**  Determine the cell ID based on the position.
   *   @param[in] aLocalPosition (not used).
   *   @param[in] aGlobalPosition position in the global coordinates.
   *   @param[in] aVolumeId ID of a volume.
   *   return Cell ID.
   */
  virtual CellID cellID(const Vector3D& aLocalPosition, const Vector3D& aGlobalPosition, const VolumeID& aVolumeID) const;
  /**  Determine the azimuthal angle based on the cell ID.
   *   @param[in] aCellId ID of a cell.
   *   return Phi.
   */
  double phi(const CellID& aCellID) const;
  /**  Get the grid size in phi.
   *   return Grid size in phi.
   */
  inline double gridSizePhi() const {
    return m_gridSizePhi;
  }
  /**  Get the coordinate offset in azimuthal angle.
   *   return The offset in phi.
   */
  inline double offsetPhi() const {
    return m_offsetPhi;
  }
  /**  Get the field name for azimuthal angle.
   *   return The field name for phi.
   */
  inline std::string fieldNamePhi() const {
    return m_phiID;
  }
  /**  Set the grid size in azimuthal angle.
   *   @param[in] aCellSize Cell size in phi.
   */
  void setGridSizePhi(double aCellSize) {
    m_gridSizePhi = aCellSize;
  }
  /**  Set the coordinate offset in azimuthal angle.
   *   @param[in] aOffset Offset in phi.
   */
  inline void setOffsetPhi(double offset) {
    m_offsetPhi = offset;
  }
  /**  Set the field name used for azimuthal angle.
   *   @param[in] aFieldName Field name for phi.
   */
  inline void setFieldNamePhi(const std::string& fieldName) {
    m_phiID = fieldName;
  }
  /// calculates the Cartesian position from spherical coordinates (r, phi, eta)
  inline Vector3D positionFromREtaPhi(double ar, double aeta, double aphi) const {
    return Vector3D(ar * std::cos(aphi), ar * std::sin(aphi), ar * std::sinh(aeta));
  }
  /// from SegmentationUtil
  /// to be removed once SegmentationUtil can be included w/o linker error
  /// calculates the azimuthal angle phi from Cartesian coordinates
  inline double phiFromXYZ(const Vector3D& aposition) const {
    return std::atan2(aposition.Y, aposition.X);
  }

private:
  /// determine the azimuthal angle phi based on the current cell ID
  double phi() const;
  /// the number of bins in phi
  double m_gridSizePhi;
  /// the coordinate offset in phi
  double m_offsetPhi;
  /// the field name used for phi
  std::string m_phiID;
};
}
}
#endif /* DETSEGMENTATION_GRIDLOCALPHI_H */
