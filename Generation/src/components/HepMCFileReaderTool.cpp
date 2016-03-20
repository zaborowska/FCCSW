
#include "HepMCFileReaderTool.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IEventProcessor.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(HepMCFileReader)

HepMCFileReader::HepMCFileReader(
  const std::string& type,
  const std::string& name,
  const IInterface* parent)
  : GaudiTool(type, name, parent), m_file(nullptr) {
  declareInterface<IHepMCFileReaderTool> (this);
}

HepMCFileReader::~HepMCFileReader() {;}

StatusCode HepMCFileReader::initialize() {
  StatusCode sc = GaudiTool::initialize();
  return sc;
}

StatusCode HepMCFileReader::open(const std::string& filename) {
  // check that filename is non-empty string
  if ( filename.empty() ) {
    error()   <<  "Input file name is not specified!" << endmsg;
    return StatusCode::FAILURE;
  }
  // open file using HepMC routines
  m_file = std::make_unique<HepMC::IO_GenEvent>(filename.c_str(), std::ios::in );
  // check that readable 
  if ( ( nullptr == m_file ) || ( m_file->rdstate() == std::ios::failbit ) ) {
    error()   <<  "Failure to read the file '"+filename+"'" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

HepMC::GenEvent* HepMCFileReader::readNextEvent() {
  Assert(nullptr != m_file, "Invalid input file!");
  // this function is to be used in conjunction with
  // the data service or some other memory manager
  HepMC::GenEvent* tmpEvent = new HepMC::GenEvent();
  m_file->fill_next_event(tmpEvent);
  return tmpEvent;
}


StatusCode HepMCFileReader::finalize() {
  m_file.reset();
  return GaudiTool::finalize();
}

