
#include "Framework/RunHeader.h"

/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include <iostream>

namespace ldmx {

void RunHeader::stream(std::ostream &s) const {
  s << "RunHeader { run: " << getRunNumber()
    << ", detectorName: " << getDetectorName()
    << ", description: " << getDescription() << "\n";
  s << "  int parameters: "
    << "\n";
  for (const auto &[key, val] : int_params_)
    s << "    " << key << " = " << val << "\n";
  s << "  float parameters: "
    << "\n";
  for (const auto &[key, val] : float_params_)
    s << "    " << key << " = " << val << "\n";
  s << "  string parameters: "
    << "\n";
  for (const auto &[key, val] : string_params_)
    s << "    " << key << " = " << val << "\n";
  s << "}";
}

void RunHeader::Print() const { stream(std::cout); }

} // namespace ldmx
