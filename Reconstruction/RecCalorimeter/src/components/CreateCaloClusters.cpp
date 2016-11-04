#include "CreateCaloClusters.h"

#include <unordered_map>
#include <utility>
#include <boost/functional/hash.hpp>

DECLARE_ALGORITHM_FACTORY(CreateCaloClusters)

CreateCaloClusters::CreateCaloClusters(const std::string& name, ISvcLocator* svcLoc)
  : GaudiAlgorithm(name, svcLoc)
{
  declareInput("cells", m_cells,"cells");
  declareOutput("clusters", m_clusters,"clusters");

  declareProperty("buildTowersTool",m_buildTowersTool);
  declarePrivateTool(m_buildTowersTool,"BuildCaloTowersTool");
 
}

CreateCaloClusters::~CreateCaloClusters()
{
}



StatusCode CreateCaloClusters::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  info() << "CreateCaloClusters initialized" << endmsg;
  info() << "Cluster algorithm : " << "Sliding window" << endmsg;
 
  
  //Initialization of various tools
  //Build calo towers
  if (!m_buildTowersTool.retrieve()) {
    error()<<"Unable to retrieve the build towers tool tool!!!"<<endmsg;
    return StatusCode::FAILURE;
  }
  
  return sc;
}

StatusCode CreateCaloClusters::execute() {
   
  //Get the input collection with Geant4 hits
  const fcc::CaloHitCollection* cells = m_cells.get();
  debug() << "Input Hit collection size: " << cells->size() << endmsg;

  std::unordered_map<std::pair<int,int>, float, boost::hash<std::pair<int, int>>> caloTowers = m_buildTowersTool->buildTowers(*cells);

  /*
  TODO: 
  1/ sliding window above threshold
  2/ overlapping windows removal
  3/ position calculation
  4/ final cluster formation
  */

  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClusters::finalize() {  
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}

