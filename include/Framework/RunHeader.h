#ifndef FRAMEWORK_RUNHEADER_H_
#define FRAMEWORK_RUNHEADER_H_

/*~~~~~~~~~~*/
/*   ROOT   */
/*~~~~~~~~~~*/
#include "TObject.h"

/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include <map>
#include <string>

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/AbstractRunHeader.h"
#include "Framework/Version.h"

namespace ldmx {

class RunHeader : public framework::AbstractRunHeader {

public:
  /**
   * Constructor.
   *
   * @param runNumber The run number uniquely identifying this run
   */
  RunHeader(int runNumber) : framework::AbstractRunHeader(runNumber){};

  // RunHeader() {}

  /// Destructor
  virtual ~RunHeader() = default;

  /** @return The name of the detector used to create the events. */
  const std::string &getDetectorName() const { return detectorName_; }

  /**
   * Set the name of the detector that was used in this run
   *
   * @param[in] detector_name The detector name that was used in this run.
   */
  void setDetectorName(const std::string &det) final override {
    detectorName_ = det;
  }

  /**
   * @return The git SHA-1 associated with the software tag used
   * to generate this file.
   */
  const std::string &getSoftwareTag() const { return softwareTag_; }

  /** @return A short description of the run. */
  const std::string &getDescription() const { return description_; }

  /**
   * Set a short description of the run.
   *
   * @param[in] description A short description of the run.
   */
  void setDescription(const std::string &des) final override {
    description_ = des;
  }

  /**
   * Set a string parameter value.
   *
   * @param name The name of the parameter.
   * @param value The parameter value.
   */
  void setStringParameter(const std::string &name, std::string value) final override {
    string_params_[name] = value;
  }
  
  /// Get a const reference to all string parameters
  const std::map<std::string, std::string> &getStringParameters() const {
    return string_params_;
  }

  /**
   * Get a string parameter value.
   *
   * @param name The name of the parameter.
   * @return value The parameter value.
   */
  std::string getStringParameter(const std::string &name) const {
    return string_params_.at(name);
  }

  /**
   * Set an int parameter value.
   *
   * @param name The name of the parameter.
   * @param value The value of the parameter.
   */
  void setIntParameter(const std::string &name, int value) final override {
    int_params_[name] = value;
  }

  /**
   * Get an int parameter value.
   *
   * @param name The name of the parameter.
   * @return The parameter value.
   */
  int getIntParameter(const std::string &name) const {
    return int_params_.at(name);
  }

  /// Get a const reference to all int parameters
  const std::map<std::string, int> &getIntParameters() const {
    return int_params_;
  }

  /**
   * Set a float parameter value.
   *
   * @param name The name of the parameter.
   * @param value The parameter value.
   */
  void setFloatParameter(const std::string &name, float value) final override {
    float_params_[name] = value;
  }

  /**
   * Get a float parameter value.
   *
   * @param name The name of the parameter.
   * @return value The parameter value.
   */
  float getFloatParameter(const std::string &name) const {
    return float_params_.at(name);
  }

  /// Get a const reference to all float parameters
  const std::map<std::string, float> &getFloatParameters() const {
    return float_params_;
  }

  /**
   * Stream this object into the input ostream
   *
   * Includes new-line characters to separate out the different parameter maps
   *
   * @param[in] s ostream to write to
   */
  void stream(std::ostream &s) const;

  /** Print a string desciption of this object. */
  void Print() const;

  /**
   * Stream this object to an output stream
   *
   * Needs to be here and labeled as friend for
   * it to be compatible with Boost logging.
   *
   * @see ldmx::RunHeader::stream
   * @param[in] s ostream to write to
   * @param[in] h RunHeader to write out
   * @return modified ostream
   */
  friend std::ostream &operator<<(std::ostream &s, const ldmx::RunHeader &h) {
    h.stream(s);
    return s;
  }

  /**
   * Create an instance of a RunHeader object.
   *
   * @param[in] run_number The run number uniquely identifying the run.
   *
   * @return An instance of a RunHeader object.
   */
  static std::shared_ptr<AbstractRunHeader> create(const int run_number) {
    std::shared_ptr<AbstractRunHeader> run_header(new RunHeader(run_number));
    return run_header;
  }

private:
  /// Map of string parameters.
  std::map<std::string, std::string> string_params_;
  
  /// Map of int parameters.
  std::map<std::string, int> int_params_;

  /// Map of float parameters. 
  std::map<std::string, float> float_params_;
  
  /** Detector name. */
  std::string detectorName_{""};

  /** Run description. */
  std::string description_{""};

  /**
   * git SHA-1 hash associated with the software tag used to generate
   * this file.
   */
  std::string softwareTag_{GIT_SHA1};

  ClassDef(RunHeader, 4);

}; // RunHeader

} // namespace ldmx

#endif // _EVENT_RUN_HEADER_H_
