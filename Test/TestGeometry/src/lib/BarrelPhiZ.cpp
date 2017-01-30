#include "DD4hep/DetFactoryHelper.h"

namespace det {
static DD4hep::Geometry::Ref_t createBarrelPhiZ(DD4hep::Geometry::LCDD& aLcdd,
  DD4hep::XML::Handle_t aXmlElement,
  DD4hep::Geometry::SensitiveDetector aSensDet) {
  DD4hep::XML::DetElement x_det = aXmlElement;
  std::string name = x_det.nameStr();
  DD4hep::XML::Dimension   dim    (x_det.dimensions());
  DD4hep::XML::Dimension   pos    (x_det.position());
  DD4hep::XML::Dimension   rot    (x_det.rotation());
  DD4hep::Geometry::DetElement   det    (name,x_det.id());
  DD4hep::Geometry::Volume      det_vol(name+"_vol",
    DD4hep::Geometry::Tube(dim.rmin(),dim.rmax(),dim.dz()), aLcdd.material(x_det.materialStr()));
  DD4hep::Geometry::Volume      mother = aLcdd.pickMotherVolume(det);
  DD4hep::Geometry::Transform3D transform(DD4hep::Geometry::Rotation3D(
      DD4hep::Geometry::RotationZYX(rot.z(),rot.y(),rot.x())), DD4hep::Geometry::Position(pos.x(),pos.y(),pos.z()));

  // here add cells :
  DD4hep::XML::DetElement cell_det = aXmlElement.child("module");
  std::string cell_name = cell_det.nameStr();
  std::cout << "name: " << cell_name << std::endl;
  DD4hep::XML::Dimension  cell_dim(cell_det.dimensions());
  DD4hep::Geometry::Material cell_mat (aLcdd.material(cell_det.materialStr()));
  std::cout << "material: " << cell_det.materialStr() << std::endl;
  // get number of cells that are to be replicated in phi and in Z
  double Rmin = dim.rmin();
  double Rmax = dim.rmax();
  std::cout << "Rmin: " << Rmin << "\tRmax: " << Rmax << std::endl;
  uint numPhi = cell_dim.phiBins();
  double dPhi = 2 * M_PI / numPhi;
  double tanHalfPhi = tan( dPhi/2. );
  double dXmin = 2. * Rmin * tanHalfPhi;
  double dXmax = 2. * Rmax * tanHalfPhi / sqrt ( tanHalfPhi * tanHalfPhi + 1 );
  double dR = (dXmax - dXmin) / (2. * tanHalfPhi);
  std::cout << "dRmin: " << dXmin << "\tdRmax: " << dXmax  << "\tdR: " << dR   << "\tdPhi: " << dPhi << std::endl;
  uint numZ = dim.dz() / cell_dim.dz();
  std::cout << " Number of cells in phi: " << numPhi <<" and in Z: " << numZ << " = total of " << numPhi * numZ<< std::endl;
  // construct slice in Z of modules:
  DD4hep::Geometry::Volume slice_vol("slice_vol",
    DD4hep::Geometry::Tube(Rmin, Rmax, cell_dim.dz()),
    aLcdd.material("Air"));
  // create cell volume
  DD4hep::Geometry::Volume cell_vol("cell_vol", DD4hep::Geometry::Trapezoid(
      dXmin / 2., dXmax / 2., cell_dim.dz(), cell_dim.dz(), dR / 2.
      ), cell_mat );
  // mark as sensitive
  DD4hep::Geometry::SensitiveDetector sd = aSensDet;
  DD4hep::XML::Dimension sd_typ = x_det.child(_U(sensitive));
  sd.setType(sd_typ.typeStr());
  cell_vol.setSensitiveDetector(aSensDet);
  // and place cells in slices, rotated by phi angle (dPhi)
  double offsetPhi = 0;
  for ( uint iPhi = 0; iPhi < numPhi; iPhi++ ) {
    double phi = offsetPhi + iPhi * dPhi;
    double xPosCell = (Rmin + dR / 2.) * cos(phi);
    double yPosCell = (Rmin + dR / 2.) * sin(phi);
    std::cout << " Placing cell " << iPhi <<" at angle "<< phi <<" at ( " << xPosCell << " , " << yPosCell << " ) " << std::endl;
    DD4hep::Geometry::Transform3D transformcell(
      DD4hep::Geometry::RotationX(- M_PI / 2.) // to get trapezoid in XY plane
      * DD4hep::Geometry::RotationY(M_PI / 2. // to get trapezoid pointed towards centre
        - phi), // rotation of individual module
      DD4hep::Geometry::Position(xPosCell,yPosCell,0));
    DD4hep::Geometry::PlacedVolume cell_physvol = slice_vol.placeVolume(cell_vol,transformcell);
    cell_physvol.addPhysVolID("phi", iPhi);
  }
  // now place slices along z axis
  double offset = - dim.dz() + cell_dim.dz();
  for(uint iz=0; iz < numZ; iz++) {
    DD4hep::Geometry::Transform3D transformcell(DD4hep::Geometry::Rotation3D(
        DD4hep::Geometry::RotationZYX(0,0,0)), DD4hep::Geometry::Position(0,0,offset+iz*2.*cell_dim.dz() ));
    DD4hep::Geometry::PlacedVolume slice_physvol = det_vol.placeVolume(slice_vol,transformcell);
    slice_physvol.addPhysVolID("z", iz);
  }
  // place outer box
  DD4hep::Geometry::PlacedVolume phv = mother.placeVolume(det_vol,transform);
  det_vol.setVisAttributes(aLcdd, x_det.visStr());
  det_vol.setLimitSet(aLcdd, x_det.limitsStr());
  det_vol.setRegion(aLcdd , x_det.regionStr());
  phv.addPhysVolID("system",x_det.id());
  det.setPlacement(phv);
  return det;
}
}
// first argument is the type from the xml file
DECLARE_DETELEMENT(BarrelPhiZ, det::createBarrelPhiZ)
