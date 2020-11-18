
#ifndef FRAMEWORK_ROOTEVENT_H_
#define FRAMEWORK_ROOTEVENT_H_

// ROOT
#include "TBranchElement.h"
#include "TTree.h"

#include "EventDef.h"

// LDMX
#include "Framework/Exception/Exception.h"
#include "Framework/ProductTag.h"
#include "Framework/Event.h"

// STL
#include <algorithm>
#include <iostream>
#include <map>
#include <regex.h>
#include <set>
#include <string>
#include <variant>

namespace framework {

/**
 * @class RootEvent
 * @brief Implements an event buffer system for storing event data
 *
 * @note
 * Event data is stored in ROOT trees and branches, which can be added
 * on the fly.
 */
class RootEvent : public Event {

public:
  /**
   * Class constructor.
   * @param passName The default pass name for adding event data.
   */
  RootEvent(const std::string &passName);

  /**
   * Class destructor.
   */
  ~RootEvent();

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
   * Get the event number.
   * @return the event index/number
   */
  int getEventNumber() const { return eventHeader_.getEventNumber(); }

  /**
   * Get the event weight
   * @return weight from the EventHeader
   */
  double getEventWeight() const { return eventHeader_.getWeight(); }


  /**
   * Check the existence of one-and-only-one object with the
   * given name (excluding the pass) in the event.
   * @param name Name (label, not class name) given to the object when it was
   * put into the event.
   * @return True if the object or collection exists in the event.
   */
  bool exists(const std::string &name) const { return exists(name, ""); }

  /**
   * Check for the existence of an object or collection with the
   * given name and pass name in the event.
   * @param name Name (label, not class name) given to the object when it was
   * put into the event.
   * @param passName The process pass label which was in use when this object
   * was put into the event, such as "sim" or "rerecov2".
   * @return True if the object or collection *uniquely* exists in the event.
   */
  bool exists(const std::string &name, const std::string &passName) const {
    return (searchProducts(name, passName, "").size() == 1);
  }

  /**
   * Add a drop rule to the list of regex expressions to drop.
   *
   * If a collection name matches one of the stored regex expressions, it will
   * be stored as a passenger but not added to output tree.
   *
   * @param exp regex to match
   */
  void addDrop(const std::string &exp);

  /**
   * Get a list of products which match the given POSIX-Extended,
   * case-insenstive regular-expressions. An empty argument is interpreted as
   * ".*", which matches everything.
   * @param namematch Regular expression to compare with the product name
   * @param passmatch Regular expression to compare with the pass name
   * @param typematch Regular expression to compare with the type name
   */
  std::vector<ldmx::ProductTag> searchProducts(const std::string &namematch,
                                         const std::string &passmatch,
                                         const std::string &typematch) const;



protected:

public:
  /** ********* Functionality for storage  ********** **/

  /**
   * Set the input data tree.
   * @param tree The input data tree.
   */
  void setInputTree(TTree *tree);

  /**
   * Set the output data tree.
   * @param tree The output data tree.
   */
  void setOutputTree(TTree *tree);

  /**
   * Create the output data tree.
   * @return The output data tree.
   */
  TTree *createTree();

  /**
   * Get a list of the data products in the event
   */
  const std::vector<ldmx::ProductTag> &getProducts() const { return products_; }

  /**
   * Go to the next event by incrementing the entry index.
   * @return Hard-coded to return true.
   */
  virtual bool nextEvent();

  /**
   * Action to be executed before the tree is filled.
   */
  void beforeFill();

  /**
   * Clear this object's data (including passengers).
   */
  void Clear();

  /**
   * Perform end of event action (doesn't do anything right now).
   */
  void onEndOfEvent();

  /**
   * Perform end of file action.
   *
   * Clears buffer objects and resets output branch addresses.
   * This prepares the event bus for a new input file (with new addresses).
   */
  void onEndOfFile();

  /**
   * Get the current/default pass name.
   * @return The current/default pass name.
   */
  std::string getPassName() { return passName_; }

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
            bool shouldDrop(const std::string &collName) const;

private:
  /**
   * @class clearPassenger
   * Clearing of event objects.
   *
   * This is necessary, so if a producer skips an event, then
   * the last object added won't be filled into event tree another time.
   */
  class clearPassenger {
  public:
    /**
     * All vector passengers can be cleared in the same way.
     */
    template <typename T> void operator()(std::vector<T> &vec) const {
      vec.clear();
    }

    /**
     * All map passengers can be cleared in the same way.
     */
    template <typename Key, typename Val>
    void operator()(std::map<Key, Val> &m) const {
      m.clear();
    }

    /**
     * Right now all other event objects have a Clear method defined.
     *
     * @note Notice the difference in capitalization.
     * This is a artefact of inheriting from TObject.
     */
    template <typename T> void operator()(T &obj) const { obj.Clear(); }
  };

  /**
   * @class printPassenger
   * Printing of event objects.
   *
   * This method requires all event objects to have a Print method defined.
   */
  class printPassenger {
  public:
    /**
     * Constructor
     *
     * Sets verbosity
     */
    printPassenger(int verbosity) : verbosity_(verbosity) {}

    /**
     * Prints size and contents of all vectors depending on verbosity.
     */
    template <typename T> void operator()(const std::vector<T> &vec) const {
      if (verbosity_ > 1) {
        std::cout << "Size: " << vec.size() << std::endl;
      }
      if (verbosity_ > 2) {
        std::cout << "Contents:" << std::endl;
        for (const T &obj : vec) {
          std::cout << "    ";
          obj.Print();
        }
        std::cout << std::endl;
      }
    }

    /**
     * Prints size and contents of all maps depending on verbosity.
     */
    template <typename Key, typename Val>
    void operator()(const std::map<Key, Val> &m) const {
      if (verbosity_ > 1) {
        std::cout << "Size: " << m.size() << std::endl;
      }
      if (verbosity_ > 2) {
        std::cout << "Contents:" << std::endl;
        for (const auto &keyVal : m) {
          std::cout << "    " << keyVal.first << " -> ";
          keyVal.second.Print();
        }
        std::cout << std::endl;
      }
    }

    /**
     * Just prints the object if verbosity is nonzero.
     */
    template <typename T> void operator()(const T &obj) const {
      if (verbosity_ > 1)
        obj.Print();
    }

  private:
    /** Flag for how much to print */
    int verbosity_;
  };

private:
  /**
   * The event header object.
   */
  ldmx::EventHeader eventHeader_;





  /**
   * Regex of collection names to *not* store in event.
   */
  std::vector<regex_t> regexDropCollections_;
};
} // namespace framework

#endif /* FRAMEWORK_ROOTEVENT_H_ */
