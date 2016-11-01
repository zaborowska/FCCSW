#include "BuildCaloTowersTool.h"

DECLARE_TOOL_FACTORY(BuildCaloTowersTool)

BuildCaloTowersTool::BuildCaloTowersTool(const std::string& type,const std::string& name, const IInterface* parent) 
  : GaudiTool(type, name, parent)
{
  declareInterface<IBuildCaloTowersTool>(this);
  //declareProperty("invSamplingFraction", m_invSamplingFraction=1.0);
}

BuildCaloTowersTool::~BuildCaloTowersTool()
{
}

StatusCode BuildCaloTowersTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  return sc;
}

std::unordered_map<int, float>  BuildCaloTowersTool::buildTowers(std::vector<fcc::CaloHit*>& aCells) {
  //Loop through a vector with calorimeter cells and build calo towers
  /*
  for (auto& ecells : aHits) {
    ecells->Core().Energy *= m_invSamplingFraction;
  }
  */

  std::unordered_map<int, float> caloTowers;

  return caloTowers;

}

StatusCode BuildCaloTowersTool::finalize() {
  StatusCode sc = GaudiTool::finalize();
  return sc;
}
