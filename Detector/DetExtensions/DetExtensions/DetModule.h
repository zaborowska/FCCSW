//
//  DetModule.h
//
//
//  Created by Julia Hrdinka on 16/12/14.
//
//

#ifndef DET_DETMODULE_H
#define DET_DETMODULE_H

#include "DetExtensions/IDetExtension.h"

namespace dd4hep {
namespace Geometry {
class DetElement;
}
}

namespace Det {

class DetModule : public IDetExtension {

public:
  DetModule() {}

  DetModule(const DetModule&, const dd4hep::DetElement&) {}

  virtual ~DetModule() {}

  virtual ExtensionType type() { return ExtensionType::Module; }
};
}
#endif  // DET_MODULE_H
