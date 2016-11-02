#include "BuildCaloTowersTool.h"

DECLARE_TOOL_FACTORY(BuildCaloTowersTool)

BuildCaloTowersTool::BuildCaloTowersTool(const std::string& type,const std::string& name, const IInterface* parent) 
  : GaudiTool(type, name, parent)
{
  declareInterface<IBuildCaloTowersTool>(this);
  declareProperty("deltaEtaTower", m_deltaEtaTower=0.01);
  declareProperty("deltaPhiTower", m_deltaPhiTower=0.01);
}

BuildCaloTowersTool::~BuildCaloTowersTool()
{
}

StatusCode BuildCaloTowersTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  return sc;
}

std::unordered_map<int, float>  BuildCaloTowersTool::buildTowers(const fcc::CaloHitCollection& aCells) {
  //Loop through a vector with calorimeter cells and build calo towers

  for (const auto &ecell : aCells) {
    double energy = ecell.core().energy;
    uint64_t cellId = ecell.core().cellId;
  }

  std::unordered_map<int, float> caloTowers;

  return caloTowers;

}

StatusCode BuildCaloTowersTool::finalize() {
  StatusCode sc = GaudiTool::finalize();
  return sc;
}
