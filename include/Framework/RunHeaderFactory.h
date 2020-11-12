#ifndef FRAMEWORK_RUNHEADERFACTORY_H_
#define FRAMEWORK_RUNHEADERFACTORY_H_

/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include <map>
#include <memory>
#include <string>

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/AbstractRunHeader.h"

namespace framework {

class RunHeaderFactory {

public:
  /// Get the instance to this factory
  static RunHeaderFactory *getInstance();

  /// Default constructor
  ~RunHeaderFactory();

  /**
   * Create an instance of the RunHeader of the given type.
   * @param[in] type String type of the RunHeader that needs to be created.
   * @param[in] run_number The run number uniquely identifying the run.
   */
  std::shared_ptr<AbstractRunHeader> createRunHeader(const std::string &type,
                                                     const int run_number);

private:
  /// Static instance of this class
  static RunHeaderFactory *instance_;

  /// Default constructor
  RunHeaderFactory();

  /**
   * Register the RunHeader with this factory.
   *
   * This is used to map the type of the run header to the function used to
   * create it.
   *
   * @param[in] type Type of the RunHeader being registered.
   * @param[in] create Function used to create this function.
   */
  void registerRunHeader(const std::string &name, createRunHeaderFunc create);

  // Mapping between an RunHeader type and its create function
  std::map<std::string, createRunHeaderFunc> run_header_map_;
};
} // namespace framework

#endif // FRAMEWORK_RUNHEADERFACTORY_H_
