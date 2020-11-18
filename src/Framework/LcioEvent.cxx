
#include "Framework/LcioEvent.h" 

namespace framework {

bool LcioEvent::nextEvent() {
  std::cout << "Going to next event." << std::endl;
  // If an instance of an LCIO currently exists, delete it and prepare a new
  // one for the next event.
  if (event_) {
    std::cout << "Deleteing event." << std::endl;
    delete event_; 
    event_ = new IMPL::LCEventImpl(); 
  }
}

}
