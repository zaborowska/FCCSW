#include "DetSegmentation/GridLocalPhi.h"

namespace DD4hep {
namespace DDSegmentation {

/// default constructor using an encoding string
GridLocalPhi::GridLocalPhi(const std::string& cellEncoding) :
  Segmentation(cellEncoding) {
  // define type and description
  _type = "GridLocalPhi";
  _description = "Phi segmentation in the local coordinates";

  // register all necessary parameters
  registerParameter("grid_size_phi", "Cell size in phi", m_gridSizePhi, 0.1, SegmentationParameter::AngleUnit, true);
  registerParameter("offset_phi", "Angular offset in phi", m_offsetPhi, 0., SegmentationParameter::AngleUnit, true);
  registerIdentifier("identifier_phi", "Cell ID identifier for phi", m_phiID, "localPhi");
}

/// determine the local based on the cell ID
Vector3D GridLocalPhi::position(const CellID& cID) const {
  // REDOOOOOO !!!
  _decoder->setValue(cID);
  return Vector3D(phi(), 0. , 0);
}

/// determine the cell ID based on the position
CellID GridLocalPhi::cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& vID) const {
  _decoder->setValue(vID);
  Vector3D radius( globalPosition.x() - localPosition.x(),
    globalPosition.y() - localPosition.y(),
    globalPosition.z() - localPosition.z());
  double phiRad = atan2( radius.y(), radius.x() );
  double phiGlob = atan2( globalPosition.y(), globalPosition.x() );
  double phiLoc = phiGlob - phiRad;
  if( phiLoc < - M_PI) {
    phiLoc += 2 * M_PI;
  } else if ( phiLoc > M_PI ) {
    phiLoc -= 2 * M_PI;
  }
  (*_decoder)[m_phiID] = positionToBin(phiLoc, m_gridSizePhi, m_offsetPhi - m_gridSizePhi / 2.);
  return _decoder->getValue();
}

/// determine the azimuthal angle phi based on the current cell ID
double GridLocalPhi::phi() const {
  CellID phiValue = (*_decoder)[m_phiID].value();
  return binToPosition(phiValue, m_gridSizePhi, m_offsetPhi - m_gridSizePhi / 2.);
}

/// determine the azimuthal angle phi based on the cell ID
double GridLocalPhi::phi(const CellID& cID) const {
  _decoder->setValue(cID);
  CellID phiValue = (*_decoder)[m_phiID].value();
  return binToPosition(phiValue, m_gridSizePhi, m_offsetPhi - m_gridSizePhi / 2.);
}
REGISTER_SEGMENTATION(GridLocalPhi)
}
}

