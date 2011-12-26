/**
 * @file  LinkedList.hh
 *
 * @brief  Thread generic double linked list header
 *
 * @author  Sylvain DAHAN (Sylvain.Dahan@lifc.univ-fcomte.fr)
 *          Kevin Coulomb (kevin.coulomb@sysfera.com)
 *
 * @section Licence
 *   |LICENCE|
 */


#ifndef _LINKEDLIST_HH_
#define _LINKEDLIST_HH_

#include <omniconfig.h>
#include <omnithread.h>
#include <sys/types.h>
#include <cassert>

/**
 * @brief This is a thread safe generic double linked list. The list must be
 * empty to be destroyed. All the methods are thread safe. When an
 * iterator is used, only the iterator can access to the linked
 * list. All the other thread which access to the linked list will
 * wait until the iterator is destroyed.
 *
 * \code
 *
 * typedef LinkedList<char> list;  // definition of a new list type.
 *
 * list l;                        // l = <>
 * char* a = new char; *a = 'a';
 * char* b = new char; *b = 'b';
 * char* c = new char; *c = 'c';
 * l.addElement(a);               // l = <a>
 * l.addElement(b);               // l = <a, b>
 * l.addElement(c);               // l = <a, b, c>
 * list::Iterator* i = l.getIterator();  // all the access to the list is now
 *                                       // booked to the iterator i.
 * i->hasCurrent();
 * char* c1 = i->getCurrent();    // c1 = a
 * i->next();
 * delete i->getCurrent();
 * i->removeCurrent();            // l = <a, c>
 * char* c2 = i->getCurrent();    // c2 = c
 * i->setCurrent(c1);             // l = <a, a>
 * i->reset();
 * i->setCurrent(c2);             // l = <c, a>
 * delete(i);                     // free the access to the linked list
 * l.emptyIt();                   // l = <>
 * \endcode
 * @class LinkedList
 */

template<class T>
class LinkedList {
private:
  /**
   * @brief This structure define each node of the list.
   */
  struct Node {
    // / next node or \c NULL if it's the last one.
    Node *next;
    // / previous node or \c NULL if it's the first one.
    Node *previous;
    // / a pointer on the element.
    T *element;
  };


  /**
   * @brief indicates the number of element in the list.
   */
  long counter;

  /**
   * @brief mutex that gives acces to the critical zone of the list.
   */
  mutable omni_mutex linkedListMutex;

  /**
   * @brief the first element of the list or \c NULL if the list is empty.
   */
  Node *first;

  /**
   * @brief the last element of the list or it is not defined if the list is
   * empty.
   */
  Node *last;

public:
  /**
   * @brief creates an new empty list.
   */
  LinkedList(): counter(0), first(NULL), last(NULL) {
  }


  /**
   * @brief creates a copy of the list given in argument. All the arguments
   * are cloned. This is not a copy of the pointer on the element but
   * a copy of the element itself which are use to clone the list.
   */
  LinkedList(const LinkedList &list) {
    list.linkedListMutex.lock();

    counter = list.counter;

    if (list.first) {
      Node *listNode = list.first;
      Node *newNode;
      newNode = new Node;
      first = newNode;
      newNode->previous = NULL;
      newNode->element = new
                         T(*listNode->element);

      while (listNode->next) {
        listNode = listNode->next;
        Node *nextNode = new Node;
        nextNode->previous = newNode;
        newNode->next = nextNode;
        newNode = nextNode;
        newNode->element = new
                           T(*listNode->element);
      }

      newNode->next = NULL;
      last = newNode;
    } else {
      first = NULL;
    }

    list.linkedListMutex.unlock();
  }


  /**
   * @brief destroyes the list. The list must be empty.
   */
  ~LinkedList() {
    assert(first == NULL);
    assert(counter == 0);
  }


  /**
   * @brief destroyes all the element of the list. The list become an empty
   * list.
   */
  void
  emptyIt() {
    linkedListMutex.lock();
    counter = 0;
    Node *nodeBuf;
    while ((nodeBuf = first) != NULL) {
      first = nodeBuf->next;
      delete nodeBuf->element;
      delete nodeBuf;
    }
    linkedListMutex.unlock();
  } /* emptyIt */


  /**
   * @brief Add an element at the end of the list. The element is not
   * duplicated, only the pointer is copied.
   *
   * @param element a pointer on the element added into the list. It
   * must be not \c NULL.
   */
  void
  addElement(T *element) {
    assert(element != NULL);

    Node *newNode = new
                    Node();
    newNode->next = NULL;
    newNode->element = element;

    linkedListMutex.lock();
    ++counter;
    if (first) {
      newNode->previous = last;
      last->next = newNode;
      last = newNode;
    } else {
      newNode->previous = NULL;
      first = newNode;
      last = newNode;
    }
    linkedListMutex.unlock();
  } /* addElement */


  /**
   * @brief returnes the first element of the list et removes it from the
   * list. If the list is empty, return a \c NULL pointer.
   */
  T *
  pop() {
    linkedListMutex.lock();
    T *elementBuf = NULL;
    if (first) {
      --counter;
      Node *nodeBuf = first;
      first = nodeBuf->next;
      elementBuf = nodeBuf->element;
      delete nodeBuf;
    }
    linkedListMutex.unlock();
    return elementBuf;
  } /* pop */


  /**
   * @brief returnes the length (number of element) of the list.
   * @return The size of the list
   */
  long
  length() const {
    linkedListMutex.lock();
    long size = counter;
    linkedListMutex.unlock();
    return size;
  }


  /**
   * @brief Append the list given in argument to the end of the list. The
   * list given in argument become en empty list.
   *
   * @param list The list which is append to the end of the current
   * list. At the end of the call, list is an empty list.
   */
  void
  append(LinkedList *list) {
    linkedListMutex.lock();
    list->linkedListMutex.lock();

    if (list->first) {
      last->next = list->first;
      list->first->previous = last;
      last = list->last;
      list->first = NULL;
      list->counter = 0;
    }

    list->linkedListMutex.unlock();
    linkedListMutex.unlock();
  } /* append */


  /***** Iteration section ***************************************************/
public:
  friend class Iterator;

  /**
   * @brief it the linked list iterator. It is created by the linked list and
   * can works only with it. When it is created, it lock the access on
   * the list. The access is realised when the iterator is destroyed.
   * @class Iterator
   */
  class Iterator {
public:
    /**
     * @brief destroy the iterator and free the access to the linked list.
     */
    ~Iterator() {
      linkedList->linkedListMutex.unlock();
    }

    /**
     * @brief The first element of the linked list controled by the iterator
     * become the current element.
     */
    inline void
    reset() {
      currentNode = linkedList->first;
    }

    /**
     * @brief returns true if there is a current element
     */
    inline bool
    hasCurrent() {
      return (currentNode != NULL);
    }

    /**
     * @brief returns true if there is a next element
     * @return true if there is a next element, false otherwise
     */
    inline bool
    hasNext() {
      return (currentNode->next != NULL);
    }

    /**
     * @brief returns true if there is a previous element
     * @return true if there is a previous element, false otherwise
     */
    inline bool
    hasPrevious() {
      return (currentNode->previous != NULL);
    }

    /**
     * @brief goes to the next element. The next element must exist. You can
     * check if it existe withe the \c hasNext() methods.
     */
    inline void
    next() {
      assert(currentNode != NULL);
      currentNode = currentNode->next;
    }

    /**
     * @brief goes to the previous element. The previous element must
     * exist. You can check if it exist with the \c hasPrevious()
     * methods.
     */
    inline void
    previous() {
      assert(currentNode != NULL);
      currentNode = currentNode->previous;
    }

    /**
     * @brief gets the current element. The current element must exist.
     * @return the current element
     */
    inline T *
    getCurrent() {
      assert(hasCurrent());
      return currentNode->element;
    }

    /**
     * @brief sets the current element. The actual current element is not
     * destroyed. The current element must exist.
     *
     * @param aElement it's the new current element. It must be not \c NULL.
     */
    inline void
    setCurrent(T *aElement) {
      assert(aElement != NULL);
      assert(hasCurrent());
      currentNode->element = aElement;
    }

    /**
     * @brief removes the current element from the list. But it does not
     * delete it. The element is not destoyed, just is reference is
     * removed from the list. The current element must exist.
     */
    void
    removeCurrent() {
      assert(hasCurrent());
      --(linkedList->counter);
      Node *currentNodeBuf = currentNode;
      currentNode = currentNodeBuf->next;
      if (currentNodeBuf->previous) {
        currentNodeBuf->previous->next = currentNodeBuf->next;
      } else {
        linkedList->first = currentNodeBuf->next;
      }
      if (currentNodeBuf->next) {
        currentNodeBuf->next->previous = currentNodeBuf->previous;
      } else {
        linkedList->last = currentNodeBuf->previous;
      }
      delete currentNodeBuf;
    } /* removeCurrent */

private:
    friend class LinkedList;

    /**
     * @brief pointer on the linkedList which is controled by the iterator.
     */
    LinkedList *linkedList;

    /**
     * @brief pointer on the current node of the linked list.
     */
    Node *currentNode;

    /**
     * @brief creates a new iterator for the linked list \c controledList.
     *
     * @param controledList pointer on the list which is controled by
     * the iterator.
     */
    explicit
    Iterator(LinkedList *controledList) {
      linkedList = controledList;
      currentNode = linkedList->first;
    }
  };

  /**
   * @brief Creates an iterator which control the linked list. All the access
   * on the list is blocked until the iterator is destroyed.
   * @return an iterator to the current list
   */
  Iterator *
  getIterator() {
    linkedListMutex.lock();
    Iterator *iteratorBuf = new
                            Iterator(this);
    return iteratorBuf;
  }
};

#endif  // _LINKEDLIST_HH_
