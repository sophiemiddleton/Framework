
#ifndef FRAMEWORK_LCIOEVENT_H_
#define FRAMEWORK_LCIOEVENT_H_

#include "Framework/Event.h"

#include "IMPL/LCEventImpl.h" 

namespace framework {

class LcioEvent : public Event {
public:
  /**
   * Class constructor.
   * @param pass_name The default pass name for adding event data. This isn't 
   *  used when generating LCIO files.
   */
  LcioEvent(const std::string &pass_name) : Event(pass_name) {};  

  /// Destructor
  ~LcioEvent() = default;

  /**
   * Go to the next event.  
   * 
   * This method will crate a new EVENT::LCEvent for this event.  
   */
  bool nextEvent() final override;

  /// @return A pointer to the underlying IMPL::LCEventImpl object.
  std::any get() final override { return event_; }

 private: 
  /// The LCIO event
  IMPL::LCEventImpl *event_{new IMPL::LCEventImpl()};  

}; // LcioEvent
} // namespace framewor
#endif // FRAMEWORK_LCIOEVENT_H_
