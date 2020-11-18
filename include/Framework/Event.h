
#ifndef FRAMEWORK_EVENT_H_
#define FRAMEWORK_EVENT_H_

#include "EventDef.h"
#include "Framework/EventHeader.h"
#include "Framework/ProductTag.h"
#include "Framework/Exception/Exception.h"

#include "TTree.h"
#include "TBranchElement.h"
#include <any>
#include <set>
#include <variant>

namespace framework {

/**
 * @class Event
 * @brief Implements an event buffer system for storing event data
 *
 * @note
 * Event data is stored in ROOT trees and branches, which can be added
 * on the fly.
 */
class Event {

public:
  /**
   * Class constructor.
   * @param passName The default pass name for adding event data.
   */
  Event(const std::string &passName) : passName_(passName) {};

  /**
   * Class destructor.
   */
  ~Event(){};

  /**
   * Get the event header.
   * @return A reference to the event header.
   */
  ldmx::EventHeader &getEventHeader() { return eventHeader_; }

  /**
   * Get the event header as a pointer
   * @return A const pointer to the event header.
   */
  const ldmx::EventHeader *getEventHeaderPtr() { return &eventHeader_; }

  /**
   * Go to the next event by incrementing the entry index.
   * @return Hard-coded to return true.
   */
  virtual bool nextEvent(){};

  virtual void Clear(){};

  virtual void onEndOfFile(){};

  /**
   * Adds an object to the event bus
   * @param collectionName
   * @param obj in ROOT dictionary to add
   */
  template <typename T> void add(const std::string &collectionName, T &obj) {
    if (collectionName.find('_') != std::string::npos) {
      EXCEPTION_RAISE("IllegalName",
                      "The product name '" + collectionName +
                          "' is illegal as it contains an underscore.");
    }

    std::string branchName;
    if (collectionName == ldmx::EventHeader::BRANCH)
      branchName = collectionName;
    else
      branchName = makeBranchName(collectionName);

    if (branchesFilled_.find(branchName) != branchesFilled_.end()) {
      EXCEPTION_RAISE("ProductExists",
                      "A product named '" + collectionName +
                          "' already exists in the event (has been loaded by a "
                          "previous producer in this process).");
    }
    branchesFilled_.insert(branchName);
    // MEMORY add is leaking memory when given a vector (possible upon
    // destruction of Event?) MEMORY add is 'conditional jump or move depends on
    // uninitialised values' for all types of objects
    //  TTree::BranchImpRef or TTree::BronchExec
    if (passengers_.find(branchName) == passengers_.end()) {
      // create a new branch for this collection
      // TODO check if input type is listed as an EventBusPassenger?
      passengers_[branchName] = EventBusPassenger(obj);
      T *passengerAddress = std::get_if<T>(&passengers_[branchName]);
      std::string tname =
          typeid(obj)
              .name(); // type name (want to use branch element if possible)
      if (outputTree_ != 0 and not shouldDrop(branchName)) {
        TBranchElement *outBranch = dynamic_cast<TBranchElement *>(
            outputTree_->GetBranch(branchName.c_str()));
        if (outBranch) {
          // branch already exists, just reset branch object
          outBranch->SetObject(passengerAddress);
        } else {
          // branch doesnt exist, make new one
          outBranch = dynamic_cast<TBranchElement *>(outputTree_->Branch(
              branchName.c_str(), passengerAddress, 100000, 3));
        }
        newBranches_.push_back(outBranch);
        // get type name from branch if possible, otherwise use compiler level
        // type name (above)
        tname = outBranch->GetClassName();
      } // output tree exists or not
      products_.emplace_back(collectionName, passName_, tname);
      branchNames_.push_back(branchName);
      knownLookups_.clear(); // have to invalidate this cache
    }

    // copy input contents into bus passenger
    EventBusPassenger toAdd(obj);
    if (toAdd.index() == passengers_[branchName].index()) {
      std::visit(sortPassenger(), toAdd); // sort before copying over
      passengers_[branchName] = toAdd;
    } else {
      EXCEPTION_RAISE("TypeMismatch",
                      "Attempting to add an object whose type '" +
                          std::string(typeid(obj).name()) +
                          "' doesn't match the type stored in the collection.");
    }

    return;
  }

  /**
   * Make a branch name from a collection and pass name.
   * @param collectionName The collection name.
   * @param passName The pass name.
   */
  std::string makeBranchName(const std::string &collectionName,
                             const std::string &passName) const {
    return collectionName + "_" + passName;
  }
            /**
             * Make a branch name from a collection and the default(current) pass name.
             * @param collectionName The collection name.
             */
            std::string makeBranchName(const std::string& collectionName) const {
                return makeBranchName(collectionName, passName_);
            }

  /**
   * Get a collection (std::vector) of objects from the event bus
   */
  template <typename T>
  const std::vector<T> getCollection(const std::string &collectionName,
                                     const std::string &passName) const {
    return getObject<std::vector<T>>(collectionName, passName);
  }

  /**
   * Get a collection (std::vector) of objects from the event bus when you don't
   * care about the pass
   */
  template <typename T>
  const std::vector<T> getCollection(const std::string &collectionName) const {
    return getCollection<T>(collectionName, "");
  }
  /**
   * Get a general object from the event bus when you don't care about the pass
   */
  template <typename T>
  const T getObject(const std::string &collectionName) const {
    return getObject<T>(collectionName, "");
  }
  /**
   * Get a general object from the event bus
   */
  template <typename T>
  const T getObject(const std::string &collectionName,
                    const std::string &passName) const {
    return getImpl<T>(collectionName, passName);
  }


  /**
   * Get a map (std::map) of objects from the event bus
   */
  template <typename Key, typename Val>
  const std::map<Key, Val> getMap(const std::string &collectionName,
                                  const std::string &passName) const {
    return getObject<std::map<Key, Val>>(collectionName, passName);
  }

  /**
   * Get a map of objects from the event bus when you don't care about the pass
   */
  template <typename Key, typename Val>
  const std::map<Key, Val> getMap(const std::string &collectionName) const {
    return getMap<Key, Val>(collectionName, "");
  }
  /**
   * Return a raw pointer to the underlying event object.
   *
   * This is only used when this object is not the main event object i.e.
   * when this object is being used as a wrapper for another event object.
   *
   * @return a raw pointer to the underlying event object if it exists.
   *  By default this method returns null.
   */
  virtual std::any get() { return nullptr; };
            /**
             * Check if collection should be dropped.
             *
             * @param collName name of collection
             * @return true if the collection should be dropped (i.e. NOT saved)
             */
            bool shouldDrop(const std::string &collName) const {};
            /**
             * @class sortPassenger
             * Sorting of passenger event objects.
             *
             * This method allows for the collections to be sorted by
             * the content's defined comparison operator <.
             *
             * @note If the operator < is not defined, std::sort implicitly
             * converts the object to a more basic object that has an included
             * less than operator. If this implicit conversion doesn't work,
             * then a complicated compile error is thrown.
             *
             * @note More specific sorting methods can be input here if you wish.
             * When templating, be aware that the order does matter. std uses
             * the first function that matches the input type. (From top down,
             * so input a specific sorting method at the top).
             */
            class sortPassenger {
                public:
                    /**
                     * Sort vectors using the std::sort method.
                     */
                    template <typename T>
                    void operator()(std::vector<T> &vec) const { std::sort(vec.begin(),vec.end()); }
        
                    /**
                     * Don't sort the other objects.
                     *
                     * The unused attribute requires the compiler to be gcc!
                     */
                    template <typename T>
                    void operator()(__attribute__((unused)) T &obj) const { /*Nothing on purpose*/ return; }
        
            };

  /**
   * Get an event passenger from the event bus (actual implementation)
   * @param collectionName name of collection you want
   * @param passName name of pass you want
   */
  template <typename T>
  T getImpl(const std::string &collectionName,
            const std::string &passName) const {

    // get branch name
    std::string branchName;
    if (collectionName == ldmx::EventHeader::BRANCH)
      branchName = collectionName;
    else
      branchName = makeBranchName(collectionName, passName);

    // if no passName, then find branchName by looking over known branches
    if (passName.empty() && collectionName != ldmx::EventHeader::BRANCH) {
      auto itKL = knownLookups_.find(collectionName);
      if (itKL != knownLookups_.end())
        branchName = itKL->second;
      else {
        // this collecitonName hasn't been found before
        std::vector<std::vector<std::string>::const_iterator> matches;
        branchName = collectionName + "_";
        for (auto itBN = branchNames_.begin(); itBN != branchNames_.end();
             itBN++) {
          if (!itBN->compare(0, branchName.size(), branchName))
            matches.push_back(itBN);
        }
        if (matches.empty()) {
          // no matches found
          EXCEPTION_RAISE("ProductNotFound",
                          "No product found for name '" + collectionName + "'");
        } else if (matches.size() > 1) {
          // more than one branch found
          std::string names;
          for (auto strs : matches) {
            if (!names.empty())
              names += ", ";
            names += *strs;
          }
          EXCEPTION_RAISE("ProductAmbiguous",
                          "Multiple products found for name '" +
                              collectionName +
                              "' without specified pass name (" + names + ")");
        } else {
          // exactly one branch found
          branchName = *matches.front();
          knownLookups_[collectionName] = branchName;
        }
      }
    }

    // get iterators to branch and collection
    auto itBranch = branches_.find(branchName);
    auto itPassenger = passengers_.find(branchName);

    if (itPassenger != passengers_.end()) {
      if (itBranch != branches_.end()) {
        // passenger and branch found
        itBranch->second->GetEntry(ientry_);
        // reading branches from input tree need to be manually updated
        passengers_[branchName] = *((T *)(itBranch->second->GetObject()));
      }
      return std::get<T>(itPassenger->second);
    } else if (inputTree_ == 0) {
      // not found in loaded branches and there is no inputTree,
      // so no hope of finding an unloaded object
      EXCEPTION_RAISE("ProductNotFound", "No product found for name '" +
                                             collectionName + "' and pass '" +
                                             passName_ + "'");
    }

    // find the active branch and update if necessary
    if (itBranch != branches_.end()) {

      // update buffers if needed
      if (itBranch->second->GetReadEntry() != ientry_) {
        itBranch->second->GetEntry(ientry_, 1);
      }

      // check the objects map
      if (itPassenger != passengers_.end())
        return std::get<T>(itPassenger->second);

      // this case is hard (impossible?) to achieve
      EXCEPTION_RAISE(
          "ProductNotFound",
          "A branch mis-match occurred. I'm not sure how I got here!");
    } else {

      // ok, maybe we've not loaded this yet, look for a branch
      TBranchElement *branch = dynamic_cast<TBranchElement *>(
          inputTree_->GetBranch(branchName.c_str()));
      if (branch == 0) {
        // inputTree doesn't have that branch
        EXCEPTION_RAISE("ProductNotFound", "No product found for name '" +
                                               collectionName + "' and pass '" +
                                               passName_ + "'");
      }
      // ooh, new branch!
      // load in the current entry
      branch->SetStatus(1); // overrides any 'ignore' rules
      branch->GetEntry((ientry_ < 0) ? (0) : (ientry_));

      // insert into maps of loaded branches and passengers
      passengers_[branchName] = *(
          (T *)(branch->GetObject())); // this will fail if wrong type is passed
      branches_[branchName] = branch;

      return std::get<T>(passengers_.at(branchName));
    }
  } // getImpl
  /**
   * Print event bus
   *
   * Only prints passengers that have been loaded into the member object.
   * This means what is printed depends on when this method is called.
   * For example, if this method is called after the Clear method, then
   * everything will be an empty object.
   *
   * @param verbosity integer flag to determine how verbose you want to be.
   */
  void Print(int verbosity) const {};
protected: 
  /**
   * Map of names to branches.
   */
  mutable std::map<std::string, TBranchElement *> branches_;
  /**
   * The input tree for reading existing data.
   */
  TTree *inputTree_{nullptr};
  /**
   * Current entry in the tree.
   */
  Long64_t ientry_{-1};

  /**
   * Number of entries in the tree.
   */
  Long64_t entries_{-1};
            /**
             * List of all the event products
             */
            std::vector<ldmx::ProductTag> products_;
            /**
             * The default pass name.
             */
            std::string passName_;
            /**
             * The output tree for writing a new file.
             */
            TTree* outputTree_{nullptr};
            /**
             * Map of names to passengers.
             */
            mutable std::map<std::string, EventBusPassenger > passengers_; 
            /**
             * Names of all branches.
             */
            std::vector<std::string> branchNames_;

  /**
   * Names of branches filled during this event.
   */
  std::set<std::string> branchesFilled_;
            /**
             * Efficiency cache for empty pass name lookups.
             */
            mutable std::map<std::string, std::string> knownLookups_;
private:
  /**
   * The event header object.
   */
  ldmx::EventHeader eventHeader_;

            /**
             * List of new branches added.
             */
            std::vector<TBranchElement*> newBranches_;


};
} // namespace framework

#endif /* FRAMEWORK_EVENT_H_ */
