#include "Framework/EventProcessor.h"

// LDMX
#include "Framework/PluginFactory.h"
#include "Framework/Process.h"
#include "Framework/RunHeader.h"
#include "TDirectory.h"

namespace framework {

EventProcessor::EventProcessor(const std::string &name, Process &process)
    : process_{process},
      name_{name},
      histograms_{name},
      theLog_{logging::makeLogger(name)} {}

Conditions &EventProcessor::getConditions() const {
  return process_.getConditions();
}

const framework::EventHeader &EventProcessor::getEventHeader() const {
  return *(process_.getEventHeader());
}

TDirectory *EventProcessor::getHistoDirectory() {
  if (!histoDir_) {
    histoDir_ = process_.makeHistoDirectory(name_);
  }
  histoDir_->cd();  // make this the current directory
  return histoDir_;
}

void EventProcessor::setStorageHint(framework::StorageControlHint hint,
                                    const std::string &purposeString) {
  process_.getStorageController().addHint(name_, hint, purposeString);
}

int EventProcessor::getLogFrequency() const {
  return process_.getLogFrequency();
}

int EventProcessor::getRunNumber() const { return process_.getRunNumber(); }

void EventProcessor::declare(const std::string &classname, int classtype,
                             EventProcessorMaker *maker) {
  PluginFactory::getInstance().registerEventProcessor(classname, classtype,
                                                      maker);
}

void EventProcessor::createHistograms(const std::vector<framework::config::Parameters> &histos) {
  for (auto const &h : histos) {
    auto name{h.getParameter<std::string>("name")};
    auto xLabel{h.getParameter<std::string>("xlabel")};
    auto xbins{h.getParameter<std::vector<double>>("xbins")};
    auto yLabel{h.getParameter<std::string>("ylabel")};
    auto ybins{h.getParameter<std::vector<double>>("ybins", {})};
    if (ybins.empty()) {
      // assume 1D histogram
      histograms_.create(name, xLabel, xbins);
    } else {
      // assume 2D histogram
      histograms_.create(name, xLabel, xbins, yLabel, ybins);
    }
  }
}

dd4hep::Detector &EventProcessor::detector() {
  // Get the detector instance and check if a geometry has been loaded. If not,
  // throw an exception.
  dd4hep::Detector& detector{dd4hep::Detector::getInstance()};
  if (detector.state() == dd4hep::Detector::State::NOT_READY) {
    EXCEPTION_RAISE("GeometryNotLoadedException",
                    "A detector description has not been loaded.");
  }

  return detector;
}

Producer::Producer(const std::string &name, Process &process)
    : EventProcessor(name, process) {}

Analyzer::Analyzer(const std::string &name, Process &process)
    : EventProcessor(name, process) {}
}  // namespace framework
