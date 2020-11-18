#ifndef FRAMEWORK_LCIOEVENTFILE_H_
#define FRAMEWORK_LCIOEVENTFILE_H_

/*~~~~~~~~~~*/
/*   LCIO   */
/*~~~~~~~~~~*/
#include "IO/LCWriter.h"

#include "Framework/EventFile.h"

using namespace ldmx;

namespace framework {

class LcioEventFile : EventFile {
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
  LcioEventFile(const std::string &filename, EventFile *parent,
                bool is_output_file, bool is_single_output,
                int compression_level);

  /**
   * Class destructor.
   */
  ~LcioEventFile() = default;

  /**
   * Setup the current event. 
   *
   * When dealing with LCIO files, this is simply used for setting the local 
   * pointer to the event so it can be written once it has been filled.
   */
  void setupEvent(Event *event) final override; 

  /**
   * Get the Event object containing the event data.
   * @return The Event object containing event data.
   */
  // Event *getEvent() { /*return event_;*/ };

  /**
   * Prepare the next event.
   * @return If event was prepared/read successfully.
   */
  bool nextEvent(bool store_current_event = true) override final;

  /**
   * Close the file, writing the tree to disk if creating an output file.
   *
   * @throw Exception if run tree already exists in output file.
   */
  void close() override final{ writer_->close(); };

  /** 
   * Write the run header to the file.
   *
   * @param runHeader The run header to write into the map
   */
  void writeRunHeader(std::shared_ptr<AbstractRunHeader> run_header) final override;

  /**
   * Create an instance of this EventFile.
   *
   *
   * @return An instance of an LcioEventFile object.
   */
  static std::unique_ptr<EventFile>
  create(const std::string &filename, EventFile *parent, bool is_output_file,
         bool is_single_output, int compression_level) {
    std::unique_ptr<EventFile> event_file(new LcioEventFile(
        filename, parent, is_output_file, is_single_output, compression_level));
    return event_file;
  }

private:

  // The event container

  /// The LCIO writer
  IO::LCWriter *writer_{nullptr};



}; // LcioEventFile

} // namespace framework

#endif // FRAMEWORK_LCIOEVENTFILE_H_
