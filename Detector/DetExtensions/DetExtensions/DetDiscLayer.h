//
//  DetDiscLayer.h
//
//
//  Created by Julia Hrdinka on 07/01/15.
//
//

#ifndef DET_DETDISCLAYER
#define DET_DETDISCLAYER

#include "DetExtensions/IDetExtension.h"

namespace dd4hep {
namespace Geometry {
class DetElement;
}
}

namespace Det {

class DetDiscLayer : public IDetExtension {

public:
  DetDiscLayer() {}

  DetDiscLayer(const DetDiscLayer&, const dd4hep::DetElement&) {}

  virtual ~DetDiscLayer() {}

  virtual ExtensionType type() { return ExtensionType::DiscLayer; }
};
}

#endif  // DET_DETDISCLAYER
