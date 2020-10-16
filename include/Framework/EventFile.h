
#ifndef FRAMEWORK_EVENTFILE_H_
#define FRAMEWORK_EVENTFILE_H_

/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include <functional>

// LDMX
#include "Framework/Event.h"

// STL
#include <string>

using namespace ldmx;

namespace framework {

class EventFile {

public:

  /// Destructor
  virtual ~EventFile() {};

  /**
   * Set an Event object containing the event data to work with this file.
   * @param evt The Event object with event data.
   */
  virtual void setupEvent(Event *evt) = 0;

  void addDrop(const std::string &rule){};

  /**
   * Get the RunHeader for a given run, if it exists in the input file.
   * @param runNumber The run number.
   * @return The RunHeader from the input file.
   * @throw Exception if there is no RunHeader in the map with the given run
   * number.
   */
  const RunHeader &getRunHeader(int runNumber){};

  /**
   * Change pointer to different parent file.
   * @param parent pointer to new parent file
   */
  void updateParent(EventFile *parent){};

  /**
   * Get the Event object containing the event data.
   * @return The Event object containing event data.
   */
  // Event *getEvent() { return event_; };

  /**
   * Prepare the next event.
   *
   * @param store_current_event Flag denoting whether the current event should
   *  be stored.
   *
   * @return true If event was prepared/read successfully.
   */
  virtual bool nextEvent(bool store_current_event = true) = 0;

  /// Close the file and write it to disk if it's an output file.
  virtual void close() = 0;

  /// @return The name of this file.
  const std::string &getFileName() { return filename_; }

  virtual void writeRunHeader(RunHeader &header) {}; 

protected:
  /// The name of the output file
  std::string filename_{""};
};

using createFunc = std::function<std::unique_ptr<EventFile>(
    const std::string &filename, EventFile *parent, bool is_output_file,
    bool is_single_output, int compression_level)>;

} // namespace framework

#endif // FRAMEWORK_EVENTFILE_H_
