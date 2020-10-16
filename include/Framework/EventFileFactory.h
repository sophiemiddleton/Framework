#ifndef FRAMEWORK_EVENTFILEFACTORY_H_
#define FRAMEWORK_EVENTFILEFACTORY_H_

/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include <map>

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/EventFile.h"

namespace framework {

/**
 * Factory that creates an EventFile object of the given type.
 *
 * The following EventFile types are supported:
 *  type | class
 *  ------------
 *  root | RootEventFile
 *  lcio | LcioEventFile
 *
 * The type is set through the python config file and passed to the constructor
 * of this class via a parameter.
 */
class EventFileFactory {

public:
  /// Get the instance to this factory
  static EventFileFactory *getInstance();

  /// Default constructor
  ~EventFileFactory();

  /**
   * Create an instance of the EventFile of the given type.
   *
   * @param[in] type String type of the EventFile that needs to be created.
   * @param[in] filename The name of the file that will be read in or created.
   * @param[in] parent A pointer to the EventFile that will be cloned.
   * @param[in] is_output_file True if this is an output file, false otherwise.
   * @param[in] is_single_output True if only a single output file is being
   *  written.
   * @param[in] compression_level The compression level of the file.
   *
   * @return[out] A smart pointer to an EventFile of the given type.
   *
   * @throw FileTypeDoesNotExists Thrown if the given event file type has not 
   *  been registered. 
   */
  std::unique_ptr<EventFile>
  createEventFile(const std::string &type, const std::string &filename,
                  EventFile *parent, bool is_output_file, bool is_single_output,
                  int compression_level);

private:
  /// Static instance of this class
  static EventFileFactory *instance_;

  /// Default constructor
  EventFileFactory();

  /**
   * Register the EventFile with this factory.
   *
   * This is used to map the name of the EventFIle to the function used to
   * create it.
   *
   * @param[in] name Name of the EventFile being registered.
   * @param[in] create Function used to create this function.
   */
  void registerEventFile(const std::string &name, createFunc create);

  // Mapping between an EventFile type and its create function
  std::map<std::string, createFunc> event_file_map;

}; // EventFileFactory
} // namespace framework
#endif // FRAMEWORK_EVENTFILEFACTORY_H_
