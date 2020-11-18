#include "Framework/RootEvent.h"

namespace framework {

RootEvent::RootEvent(const std::string &thePassName) : Event(passName_) {}

RootEvent::~RootEvent() {
  for (regex_t &reg : regexDropCollections_) {
    regfree(&reg);
  }
}

/*
void RootEvent::Print(int verbosity) const {
  for (const auto &keyVal : passengers_) {
    if (verbosity > 1)
      std::cout << keyVal.first << std::endl;
    std::visit(printPassenger(verbosity), keyVal.second);
  }
}*/

void RootEvent::addDrop(const std::string &exp) {
  regex_t reg;
  if (!regcomp(&reg, exp.c_str(), REG_EXTENDED | REG_ICASE | REG_NOSUB)) {
    regexDropCollections_.push_back(reg);
  } else {
    EXCEPTION_RAISE("InvalidRegex", "The passed drop rule regex '" + exp +
                                        "' is not a valid regex.");
  }
}

std::vector<ldmx::ProductTag>
RootEvent::searchProducts(const std::string &namematch,
                          const std::string &passmatch,
                          const std::string &typematch) const {
  std::vector<ldmx::ProductTag> retval;

  regex_t reg_name, reg_pass, reg_type;

  if (!regcomp(&reg_name, (namematch.empty() ? (".*") : (namematch.c_str())),
               REG_EXTENDED | REG_ICASE | REG_NOSUB)) {
    if (!regcomp(&reg_pass, (passmatch.empty() ? (".*") : (passmatch.c_str())),
                 REG_EXTENDED | REG_ICASE | REG_NOSUB)) {
      if (!regcomp(&reg_type,
                   (typematch.empty() ? (".*") : (typematch.c_str())),
                   REG_EXTENDED | REG_ICASE | REG_NOSUB)) {
        // all passed expressions are valid regular expressions
        const std::vector<ldmx::ProductTag> &products = getProducts();
        for (std::vector<ldmx::ProductTag>::const_iterator i = products.begin();
             i != products.end(); i++) {
          if (!regexec(&reg_name, i->name().c_str(), 0, 0, 0) &&
              !regexec(&reg_pass, i->passname().c_str(), 0, 0, 0) &&
              !regexec(&reg_type, i->type().c_str(), 0, 0, 0))
            retval.push_back(*i);
        }

        regfree(&reg_type);
      } else {
        EXCEPTION_RAISE("InvalidRegex",
                        "The passed type regex '" + typematch +
                            "' is not a valid regular expression");
      }
      regfree(&reg_pass);
    } else {
      EXCEPTION_RAISE("InvalidRegex",
                      "The passed name regex '" + namematch +
                          "' is not a valid regular expression");
    }
    regfree(&reg_name);
  } else {
    EXCEPTION_RAISE("RegexErr", "The passed passname regex '" + passmatch +
                                    "' is not a valid regular expression.");
  }

  return retval;
}

TTree *RootEvent::createTree() {
  outputTree_ = new TTree("LDMX_Events", "LDMX Events");

  return outputTree_;
}

void RootEvent::setOutputTree(TTree *tree) { outputTree_ = tree; }

void RootEvent::setInputTree(TTree *tree) {
  inputTree_ = tree;
  entries_ = inputTree_->GetEntriesFast();

  // in some cases, setInputTree is called more than once,
  // so reset branch listing before starting
  products_.clear();
  branchNames_.clear();
  branches_.clear();
  passengers_.clear();

  // put in EventHeader (only one without pass name)
  products_.emplace_back(ldmx::EventHeader::BRANCH, "", "ldmx::EventHeader");

  // find the names of all the existing branches
  TObjArray *branches = inputTree_->GetListOfBranches();
  for (int i = 0; i < branches->GetEntriesFast(); i++) {
    std::string brname = branches->At(i)->GetName();
    if (brname != ldmx::EventHeader::BRANCH) {
      size_t j = brname.find("_");
      std::string iname = brname.substr(0, j);
      std::string pname = brname.substr(j + 1);
      std::string tname =
          dynamic_cast<TBranchElement *>(branches->At(i))->GetClassName();
      products_.emplace_back(iname, pname, tname);
    }
    branchNames_.push_back(brname);
  }
}

bool RootEvent::nextEvent() {
  ientry_++;
  eventHeader_ = getObject<ldmx::EventHeader>(ldmx::EventHeader::BRANCH);
  return true;
}

void RootEvent::beforeFill() {
  if (inputTree_ == 0 &&
      branchesFilled_.find(ldmx::EventHeader::BRANCH) == branchesFilled_.end()) {
    // Event Header not copied from input and hasn't been added yet, need to put
    // it in
    add(ldmx::EventHeader::BRANCH, eventHeader_);
  }
}

void RootEvent::Clear() {
  // clear the event objects
  branchesFilled_.clear();
  for (auto passenger : passengers_) {
    std::visit(clearPassenger(), passenger.second);
  }
}

void RootEvent::onEndOfEvent() {}

void RootEvent::onEndOfFile() {
  passengers_.clear(); // reset event bus
  branches_.clear();   // reset branches
  if (outputTree_)
    outputTree_->ResetBranchAddresses(); // reset addresses for output branch
  if (inputTree_)
    inputTree_ = nullptr; // detach old inputTree (owned by EventFile)
  ientry_ = -1;           // reset current entry and total entries
  entries_ = -1;
}

bool RootEvent::shouldDrop(const std::string &branchName) const {
  for (const regex_t &exp : regexDropCollections_) {
    if (!regexec(&exp, branchName.c_str(), 0, 0, 0))
      return true;
  }
  return false;
}

} // namespace framework
