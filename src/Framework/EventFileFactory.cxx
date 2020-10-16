
#include "Framework/EventFileFactory.h"

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/Exception/Exception.h" 

/*~~~~~~~~~~~~~*/
/*   SimCore   */
/*~~~~~~~~~~~~~*/
#include "Framework/LcioEventFile.h"

namespace framework {

EventFileFactory *EventFileFactory::instance_ = nullptr;

EventFileFactory *EventFileFactory::getInstance() {
  // If an EventFileFactory hasn't been instantiated, create and return it.
  // Otherwise, just return the existing one.
  if (!instance_)
    instance_ = new EventFileFactory;
  return instance_;
}

EventFileFactory::EventFileFactory() {
  // Register the event file types
  // TODO: At some point the registration should be dictated by the python
  // config.
  registerEventFile("lcio", &LcioEventFile::create);
}

void EventFileFactory::registerEventFile(const std::string &name,
                                         createFunc create) {
  event_file_map[name] = create;
}

std::unique_ptr<EventFile> EventFileFactory::createEventFile(
    const std::string &type, const std::string &filename, EventFile *parent,
    bool is_output_file, bool is_single_output, int compression_level) {

  // Check if the event file type exists.  If so, create the EventFile.
  // Otherwise, just throw an exception.
  auto it{event_file_map.find(type)};
  if (it != event_file_map.end())
    return it->second(filename, parent, is_output_file, is_single_output,
                      compression_level);
  EXCEPTION_RAISE("FileTypeDoesNotExists",
                  "The provided file type does not exists.");
}

} // namespace framework
