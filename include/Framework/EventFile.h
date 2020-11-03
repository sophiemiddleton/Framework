
#ifndef FRAMEWORK_EVENTFILE_H_
#define FRAMEWORK_EVENTFILE_H_

/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include <functional>
#include <string>

// LDMX
#include "Framework/Event.h"
#include "Framework/RunHeader.h"

using namespace ldmx;

namespace framework {

/**
 * @class EventFile
 * @brief Manages a file of events
 */
class EventFile {
public:
  /**
   * Constructor to make a general file.
   *
   * This is not used directly, but it is called in the more
   * specialised constructors. This method is mainly focused on
   * reducing code copying.
   *
   * @param[in] filename the name of the file to read/write
   * @param[in] parent a pointer to the parent file to copy
   * @param[in] isOutputFile true if this file is written out
   * @param[in] isSingleOutput true if only one output file is being written to
   * @param[in] compressionSetting the compression setting for the TFile
   * (100*algo+level)
   */
  // EventFile(const std::string &filename, EventFile *parent, bool
  // isOutputFile,
  //          bool isSingleOutput, int compressionSetting);

  /// Destructor
  virtual ~EventFile() = default;

  void addDrop(const std::string &rule){};

  /**
   * Set an Event object containing the event data to work with this file.
   * @param evt The Event object with event data.
   */
  virtual void setupEvent(Event *evt) = 0;

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
   * @return If event was prepared/read successfully.
   */
  virtual bool nextEvent(bool storeCurrentEvent = true) = 0;

  /**
   * Close the file, writing the tree to disk if creating an output file.
   *
   * Deletes any RunHeaders that this instance of EventFile owns.
   *
   * @throw Exception if run tree already exists in output file.
   */
  virtual void close() = 0;

  /**
   * Write the run header into the run map
   *
   * Any RunHeader passed here is not owned or cleaned up
   * by this EventFile instance.
   *
   * @param runHeader The run header to write into the map
   * @throw Exception if run number is already in run map
   */
  void writeRunHeader(RunHeader &runHeader){};

  /**
   * Get the RunHeader for a given run, if it exists in the input file.
   * @param runNumber The run number.
   * @return The RunHeader from the input file.
   * @throw Exception if there is no RunHeader in the map with the given run
   * number.
   */
  RunHeader &getRunHeader(int runNumber){};

  const std::string &getFileName() { return filename_; }

private:
  /**
   * Fill the internal map of run numbers to RunHeader objects from the input
   * file.
   *
   * If this file is an output file and parent_ and parent_->file_ are valid
   * pointers, then the run headers are imported from parent_->file_.
   *
   * Otherwise, we try to import run headers from file_.
   *
   * Does not check if any run headers are getting overwritten!
   *
   * Any RunHeaders read in from this function are owned by this instance
   * of EventFile and are deleted in close().
   *
   * @note This function does nothing if parent_->file_ and file_ are nullptrs.
   */
  // void importRunHeaders();

protected:
  /// The name of the output file
  std::string filename_;

}; // EventFile

using createFunc = std::function<std::unique_ptr<EventFile>(
    const std::string &filename, EventFile *parent, bool is_output_file,
    bool is_single_output, int compression_level)>;
} // namespace framework

#endif // FRAMEWORK_EVENTFILE_H_
