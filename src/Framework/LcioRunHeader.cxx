
#include "Framework/LcioRunHeader.h" 

namespace framework { 

LcioRunHeader::LcioRunHeader(int run_number) : AbstractRunHeader(run_number) {
  // Instantiate the run header
  run_header = std::make_unique<IMPL::LCRunHeaderImpl>();
  
  // Set the run number 
  run_header->setRunNumber(run_number);  

}

}
