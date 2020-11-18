#include "Framework/LcioEventFile.h"

#include "Framework/Exception/Exception.h" 

/*~~~~~~~~~~*/
/*   LCIO   */
/*~~~~~~~~~~*/
#include "EVENT/LCIO.h"
#include "EVENT/LCRunHeader.h"
#include "IOIMPL/LCFactory.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCEventImpl.h" 
#include "EVENT/LCEvent.h" 

namespace framework {

LcioEventFile::LcioEventFile(const std::string &filename, EventFile *parent,
                             bool is_output_file, bool is_single_output,
                             int compression_level) {

  // Instantiate the file
  writer_ = IOIMPL::LCFactory::getInstance()->createLCWriter();

  // Set the compression level.
  // level < 0: default compression
  // level == 0: no compression
  // level > 0: compression level (typically 1 (fastest) - 9 (best compression))
  // Needs to be called before open()
  writer_->setCompressionLevel(compression_level);

  // Open a new file to write data to
  try {
    writer_->open(filename, EVENT::LCIO::WRITE_NEW);
  } catch (IO::IOException &e) {
    EXCEPTION_RAISE("FileError",
                    "[ LcioEventFile::LcioEventFile ]: Output file '" +
                        filename + "' is not writable.");
  }
}

void LcioEventFile::writeRunHeader(
    std::shared_ptr<AbstractRunHeader> run_header) {
  std::cout << "Writing run header." << std::endl;
  auto lc_run_header{static_cast<EVENT::LCRunHeader*>(
      std::any_cast<IMPL::LCRunHeaderImpl*>(run_header->get()))}; 
  writer_->writeRunHeader(lc_run_header); 
}

void LcioEventFile::setupEvent(Event *event) { 
  event_ = event;
}

bool LcioEventFile::nextEvent(bool store_current_event) {
  
  auto lcio_event{std::any_cast<IMPL::LCEventImpl*>(event_->get())};
  writer_->writeEvent(static_cast<EVENT::LCEvent*>(lcio_event)); 
  writer_->flush();

  event_->nextEvent();  

}

} // namespace framework
