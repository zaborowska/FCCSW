#include "NoiseCaloCellsTool.h"

DECLARE_TOOL_FACTORY(NoiseCaloCellsTool)

NoiseCaloCellsTool::NoiseCaloCellsTool(const std::string& type,const std::string& name, const IInterface* parent)
  : GaudiTool(type, name, parent)
{
  declareInterface<INoiseCaloCellsTool>(this);
  //uniform noise per cell in GeV
  declareProperty("cellNoise", m_cellNoise = 0.003);
  //remove cells with energy bellow filterThreshold (threshold is multiplied by a cell noise sigma)
  declareProperty("filterNoiseThreshold", m_filterThreshold = 3);
}

NoiseCaloCellsTool::~NoiseCaloCellsTool()
{
}

StatusCode NoiseCaloCellsTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  //Initialize random service
  if(service( "RndmGenSvc" , m_randSvc ).isFailure()) {
    error() << "Couldn't get RndmGenSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  m_gauss.initialize(m_randSvc, Rndm::Gauss(0,m_cellNoise));

  info() << "Sigma of the cell noise: " <<  m_cellNoise * 1.e3 << " MeV" << endmsg;
  info() << "Filter noise threshold: " <<  m_filterThreshold << "*sigma" << endmsg;
  return sc;
}

void NoiseCaloCellsTool::createRandomCellNoise(std::unordered_map<uint64_t, double>& aCells) {
  // double randomNoise = 0;
  // for (auto& cell : aCells) {
  //   randomNoise = m_gauss.shoot();
  //   cell.second += randomNoise;
  // }
  std::for_each( aCells.begin(), aCells.end(), [this](std::pair<const uint64_t , double>& p) {p.second += m_gauss.shoot();} );
}

void NoiseCaloCellsTool::filterCellNoise(std::unordered_map<uint64_t, double>& aCells) {
  //Erase a cell if it has energy bellow a threshold
  double threshold = m_filterThreshold * m_cellNoise;
  auto it = aCells.begin();
  while ((it = std::find_if(it, aCells.end(),
        [&threshold](std::pair<const uint64_t,double>& p){return bool(p.second < threshold);})
      ) != aCells.end())
    aCells.erase(it++);
}


StatusCode NoiseCaloCellsTool::finalize() {
  return GaudiTool::finalize();
}
