#ifndef FRAMEWORK_ABSTRACT_RUNHEADER_H_
#define FRAMEWORK_ABSTRACT_RUNHEADER_H_

/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include <any>
#include <functional>
#include <map>
#include <memory>
#include <string>

namespace framework {

/**
 * Abstract class used to encapsulate run information.
 */
class AbstractRunHeader {

public:
  /**
   * Constructor.
   *
   * @param[in] run_number The run number uniquely identifying this run.
   */
  AbstractRunHeader(int run_number) : run_number_(run_number){};

  /**
   * Default constructor.
   *
   * @note This exists for filling the object from a ROOT branch.
   */
  AbstractRunHeader() = default;

  /// Destructor
  virtual ~AbstractRunHeader() = default;

  /**
   * Set the name of the detector that was used in this run
   *
   * @param[in] detector_name The detector name that was used in this run.
   */
  virtual void setDetectorName(const std::string &detector_name) = 0;

  /**
   * Set a short description of the run.
   *
   * @param[in] description A short description of the run.
   */
  virtual void setDescription(const std::string &description) = 0;

  /** @return The run number. */
  virtual int getRunNumber() const { return run_number_; }

  /**
   * Set the run start time in seconds since epoch.
   *
   * @param[in] runStart the start time of the run.
   */
  virtual void setRunStart(const int run_start) { run_start_ = run_start; }

  /**
   * Get the start time of the run in seconds since epoch.
   *
   * @return The start time of the run.
   *
   */
  virtual int getRunStart() const { return run_start_; }

  /**
   * Set the end time of the run in seconds since epoch
   *
   * @param[in] runEnd the end time of the run.
   */
  virtual void setRunEnd(const int run_end) { run_end_ = run_end; };

  /**
   * Get the end time of the run in seconds since epoch.
   *
   * @return The end time of the run.
   */
  virtual int getRunEnd() const { return run_end_; }

  /**
   * Set a string parameter value.
   *
   * @param name The name of the parameter.
   * @param value The parameter value.
   */
  virtual void setStringParameter(const std::string &name,
                                  std::string value) = 0;

  /**
   * Set an int parameter value.
   *
   * @param name The name of the parameter.
   * @param value The value of the parameter.
   */
  virtual void setIntParameter(const std::string &name, int value) = 0;

  /**
   * Set a float parameter value.
   *
   * @param name The name of the parameter.
   * @param value The parameter value.
   */
  virtual void setFloatParameter(const std::string &name, float value) = 0;

  /**
   * Return a raw pointer to the underlying header object.  
   * 
   * This is only used when this object is not the main header object i.e. 
   * when this object is being used as a wrapper for another header object. 
   *
   * @return a raw pointer to the underlying header object if it exists. 
   *  By default this method returns null. 
   */
  virtual std::any get(){ return nullptr; };

private:
  /// Run start in seconds since epoch
  int run_start_{0};

  /// Run end in seconds since epoch
  int run_end_{0};

  /// The run number
  int run_number_{0};

}; // AbstractRunHeader

using createRunHeaderFunc =
    std::function<std::shared_ptr<AbstractRunHeader>(const int run_number)>;

} // namespace framework

#endif // FRAMEWORK_ABSTRACT_RUNHEADER_H_
