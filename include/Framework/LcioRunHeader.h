#ifndef FRAMEWORK_LCIORUNHEADER_H_
#define FRAMEWORK_LCIORUNHEADER_H_

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/AbstractRunHeader.h"

/*~~~~~~~~~~*/
/*   LCIO   */
/*~~~~~~~~~~*/
#include "IMPL/LCRunHeaderImpl.h"

namespace framework {

class LcioRunHeader : public AbstractRunHeader {
public:
  /**
   * Constructor.
   *
   * @param runNumber The run number uniquely identifying this run
   */
  LcioRunHeader(int run_number);

  /// Destructor
  virtual ~LcioRunHeader() = default;

  /**
   * Set the name of the detector that was used in this run
   *
   * @param[in] detector_name The detector name that was used in this run.
   */
  void setDetectorName(const std::string &detector_name) final override {
    run_header->setDetectorName(detector_name);
  }

  /**
   * Set a short description of the run.
   *
   * @param[in] description A short description of the run.
   */
  void setDescription(const std::string &description) final override {
    run_header->setDescription(description);
  }

  /**
   * Set a string parameter value.
   *
   * @param name The name of the parameter.
   * @param value The parameter value.
   */
  void setStringParameter(const std::string &name,
                          std::string value) final override {
    run_header->parameters().setValue(name, value);
  }

  /**
   * Set an int parameter value.
   *
   * @param name The name of the parameter.
   * @param value The value of the parameter.
   */
  void setIntParameter(const std::string &name, int value) final override {
    run_header->parameters().setValue(name, value);
  }

  /**
   * Set a float parameter value.
   *
   * @param name The name of the parameter.
   * @param value The parameter value.
   */
  virtual void setFloatParameter(const std::string &name, float value) {
    run_header->parameters().setValue(name, value);
  }

  /**
   * Return a raw pointer to the underlying IMPL::LCRunHeader object.
   *
   * @return a raw pointer to the underlying IMPL::LCRunHeader object if it
   * exists.
   */
  std::any get() final override { return run_header.get(); }

  /**
   * Create an instance of an LcioRunHeader object.
   *
   * @param[in] run_number The run number uniquely identifying the run.
   *
   * @return An instance of an LcioRunHeader object.
   */
  static std::shared_ptr<AbstractRunHeader> create(const int run_number) {
    std::shared_ptr<AbstractRunHeader> run_header(
        new LcioRunHeader(run_number));
    return run_header;
  }

private:
  /// The run header.
  std::unique_ptr<IMPL::LCRunHeaderImpl> run_header;

}; // LcioRunHeader
} // namespace framework

#endif // FRAMEWORK_LCIORUNHEADER_H_
