#include "CreateCaloClusters.h"

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

  /*
  //Final vector of cells
  std::vector<fcc::CaloHit*> edmFinalCellsVector;
  */
  /*
  //Copy information from vector to CaloHitCollection
  fcc::CaloHitCollection* edmCellsCollection = new fcc::CaloHitCollection();
  for (auto ecells : edmFinalCellsVector) {
    fcc::CaloHit newCell = edmCellsCollection->create();
      newCell.Core().Energy = ecells->Core().Energy;     
      newCell.Core().Time = ecells->Core().Time;
      newCell.Core().Bits = ecells->Core().Bits;
      newCell.Core().Cellid = ecells->Core().Cellid;
  }
  debug() << "Output Cell collection size: " << edmCellsCollection->size() << endmsg;
  
  //Push the CaloHitCollection to event store
  m_cells.put(edmCellsCollection);
  */

  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClusters::finalize() {  
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}

