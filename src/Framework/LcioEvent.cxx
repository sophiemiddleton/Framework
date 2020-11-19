
#include "Framework/LcioEvent.h" 

namespace framework {

bool LcioEvent::nextEvent() {
  // If an instance of an LCIO currently exists, delete it and prepare a new
  // one for the next event.
  if (event_) {
    delete event_; 
    event_ = new IMPL::LCEventImpl(); 
  }
}

}
