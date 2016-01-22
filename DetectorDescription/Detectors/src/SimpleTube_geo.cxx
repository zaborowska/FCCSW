#include "DD4hep/DetFactoryHelper.h"
#include "DetExtensions/DetCylinderVolume.h"

namespace det {

static DD4hep::Geometry::Ref_t createSimpleTube(DD4hep::Geometry::LCDD& aLcdd,
                                                DD4hep::XML::Handle_t aXmlElement,
                                                DD4hep::Geometry::SensitiveDetector aSensDet) {
  DD4hep::XML::DetElement x_det = aXmlElement;
  std::string det_name = x_det.nameStr();
  //Make DetElement
  DD4hep::Geometry::DetElement tube(det_name, x_det.id());
  //Make Volume
  DD4hep::XML::Dimension x_det_dim(x_det.dimensions());
  DD4hep::Geometry::Tube tube_shape(x_det_dim.rmin(),x_det_dim.rmax(),x_det_dim.z());
  DD4hep::Geometry::Volume tube_vol(det_name,tube_shape,aLcdd.material(x_det.materialStr()));
  tube_vol.setVisAttributes(aLcdd, x_det_dim.visStr());
  //Place Volume
  DD4hep::Geometry::Volume mother_vol = aLcdd.pickMotherVolume(tube);
  DD4hep::Geometry::PlacedVolume placedTube = mother_vol.placeVolume(tube_vol);
  placedTube.addPhysVolID(det_name,tube.id());
  tube.setPlacement(placedTube);

  return tube;
}
}

DECLARE_DETELEMENT(SimpleTube, det::createSimpleTube)
















