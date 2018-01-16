#include "DD4hep/Factories.h"
#include "DD4hep/detail/SegmentationsInterna.h"

namespace {
template <typename T>
dd4hep::SegmentationObject* create_segmentation(dd4hep::BitField64* decoder) {
  return new dd4hep::SegmentationWrapper<T>(decoder);
}
}

#include "DetSegmentation/GridEta.h"
DECLARE_SEGMENTATION(GridEta, create_segmentation<dd4hep::DDSegmentation::GridEta>)
