#include "DD4hep/DetFactoryHelper.h"

namespace det {
static DD4hep::Geometry::Ref_t createBarrelPhiZRowLayer(DD4hep::Geometry::LCDD& aLcdd,
  DD4hep::XML::Handle_t aXmlElement,
  DD4hep::Geometry::SensitiveDetector aSensDet) {
  DD4hep::XML::DetElement x_det = aXmlElement;
  std::string name = x_det.nameStr();
  DD4hep::XML::Dimension   dim    (x_det.dimensions());
  DD4hep::XML::Dimension   pos    (x_det.position());
  DD4hep::XML::Dimension   rot    (x_det.rotation());
  DD4hep::Geometry::DetElement   det    (name,x_det.id());
  DD4hep::Geometry::Volume      det_vol(name+"_vol",
    DD4hep::Geometry::Tube(dim.rmin(),dim.rmax(),dim.dz()), aLcdd.material("Air"));
  DD4hep::Geometry::Volume      mother = aLcdd.pickMotherVolume(det);
  DD4hep::Geometry::Transform3D transform(DD4hep::Geometry::Rotation3D(
      DD4hep::Geometry::RotationZYX(rot.z(),rot.y(),rot.x())), DD4hep::Geometry::Position(pos.x(),pos.y(),pos.z()));

  // here add cells :
  DD4hep::XML::DetElement cell_det = aXmlElement.child("module");
  std::string cell_name = cell_det.nameStr();
  std::cout << "name: " << cell_name << std::endl;
  DD4hep::XML::Dimension  cell_dim(cell_det.dimensions());
  DD4hep::XML::DetElement row_det = aXmlElement.child("row");
  DD4hep::XML::Dimension  row_dim(row_det.dimensions());
  DD4hep::XML::DetElement layer_det = aXmlElement.child("layer");
  DD4hep::Geometry::Material layer_mat (aLcdd.material(layer_det.materialStr()));
  std::cout << "material: " << layer_det.materialStr() << std::endl;
  // get number of cells that are to be replicated in phi and in Z and in R
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
  uint numLayer = layer_det.repeat();
  uint numZ = dim.dz() / cell_dim.dz();
  uint numRow = cell_dim.dz() / row_dim.dz();
  std::cout << " Number of layers in a cell: " << numLayer << " and rows in cell: " << numRow << ", cells in phi: " << numPhi <<" and in Z: " << numZ << " = total of " << numLayer * numRow * numPhi * numZ << std::endl;

  // create row volume
  double offsetRow = - cell_dim.dz() + row_dim.dz();
  DD4hep::Geometry::Volume row_vol("row_vol", DD4hep::Geometry::Trapezoid(
      dXmin / 2., dXmax / 2., row_dim.dz(), row_dim.dz(), dR / 2.
      ), aLcdd.material("Air") );
  // create sensitive layer volume (smallest piece of geometry)
  double layerHeight = dR / numLayer;
  double layerIncrease = (dXmax - dXmin) / numLayer ;
  double offsetLayer = - dR / 2. + layerHeight / 2.;
  // mark as sensitive
  DD4hep::Geometry::SensitiveDetector sd = aSensDet;
  DD4hep::XML::Dimension sd_typ = x_det.child(_U(sensitive));
  sd.setType(sd_typ.typeStr());
  // place layers in a row
  double dXmin_layer = dXmin;
  double dXmax_layer = dXmin + layerIncrease;
  for ( uint iLayer = 0; iLayer < numLayer; iLayer++ ) {
    DD4hep::Geometry::Volume layer_vol("layer_vol", DD4hep::Geometry::Trapezoid(
        dXmin_layer / 2., dXmax_layer / 2., row_dim.dz(), row_dim.dz(), layerHeight / 2.
        ), layer_mat );
    layer_vol.setSensitiveDetector(aSensDet);
    std::cout << " Placing layer " << iLayer <<" inside row at z = " << offsetLayer + iLayer * layerHeight << std::endl; // z before rotation, afterwards it becomes R
    DD4hep::Geometry::Transform3D transformlayer(
      DD4hep::Geometry::Rotation3D(DD4hep::Geometry::RotationZYX(0,0,0)),
      DD4hep::Geometry::Position(0,0,offsetLayer+iLayer*layerHeight));
    DD4hep::Geometry::PlacedVolume layer_physvol = row_vol.placeVolume(layer_vol,transformlayer);
    layer_physvol.addPhysVolID("layer", iLayer);
    dXmin_layer = dXmax_layer;
    dXmax_layer = dXmin_layer + layerIncrease;
  }
  // create cell volume
  DD4hep::Geometry::Volume cell_vol("cell_vol", DD4hep::Geometry::Trapezoid(
      dXmin / 2., dXmax / 2., cell_dim.dz(), cell_dim.dz(), dR / 2.
      ), aLcdd.material("Air") );
  for ( uint iRow = 0; iRow < numRow; iRow++ ) {
    std::cout << " Placing row " << iRow <<" inside cell at y = " << offsetRow + iRow * 2. * row_dim.dz() << std::endl; // y before rotation, afterwards it becomes z
    DD4hep::Geometry::Transform3D transformrow(
      DD4hep::Geometry::Rotation3D(DD4hep::Geometry::RotationZYX(0,0,0)),
      DD4hep::Geometry::Position(0,offsetRow+iRow*2.*row_dim.dz(),0 ));
    DD4hep::Geometry::PlacedVolume row_physvol = cell_vol.placeVolume(row_vol,transformrow);
    row_physvol.addPhysVolID("row", iRow);
  }
  // construct slice in Z of cells:
  DD4hep::Geometry::Volume slice_vol("slice_vol",
    DD4hep::Geometry::Tube(Rmin, Rmax, cell_dim.dz()),
    aLcdd.material("Air"));
  // place cells (consisting of rows) into slices, rotated by phi angle (dPhi)
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
    DD4hep::Geometry::Transform3D transformslice(DD4hep::Geometry::Rotation3D(
        DD4hep::Geometry::RotationZYX(0,0,0)), DD4hep::Geometry::Position(0,0,offset+iz*2.*cell_dim.dz() ));
    std::cout << " Placing slice " << iz <<" at z = " << offset + iz * 2. * cell_dim.dz() << std::endl;
    DD4hep::Geometry::PlacedVolume slice_physvol = det_vol.placeVolume(slice_vol,transformslice);
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
DECLARE_DETELEMENT(BarrelPhiZRowLayer, det::createBarrelPhiZRowLayer)
