
#include "Framework/RunHeaderFactory.h"

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/Exception/Exception.h"

/*~~~~~~~~~~~~~*/
/*   SimCore   */
/*~~~~~~~~~~~~~*/
#include "Framework/LcioRunHeader.h"
#include "Framework/RunHeader.h" 

namespace framework {

RunHeaderFactory *RunHeaderFactory::instance_ = nullptr;

RunHeaderFactory *RunHeaderFactory::getInstance() {
  // If an RunHeaderFactory hasn't been instantiated, create and return it.
  // Otherwise, just return the existing one.
  if (!instance_)
    instance_ = new RunHeaderFactory;
  return instance_;
}

RunHeaderFactory::RunHeaderFactory() {
  // Register the event file types
  // TODO: At some point the registration should be dictated by the python
  // config.
  registerRunHeader("lcio", &LcioRunHeader::create);
  registerRunHeader("root", &ldmx::RunHeader::create);
}

void RunHeaderFactory::registerRunHeader(const std::string &name,
                                         createRunHeaderFunc create) {
  run_header_map_[name] = create;
}

std::shared_ptr<AbstractRunHeader>
RunHeaderFactory::createRunHeader(const std::string &type,
                                  const int run_number) {

  // Check if the event file type exists.  If so, create the RunHeader.
  // Otherwise, just throw an exception.
  auto it{run_header_map_.find(type)};
  if (it != run_header_map_.end())
    return it->second(run_number);
  EXCEPTION_RAISE("RunHeaderTypeDoesNotExist",
                  "The provided run header type does not exists.");
}
} // namespace framework
