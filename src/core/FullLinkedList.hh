/****************************************************************************/
/* Thread safe generic Double linked list with full access                  */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id$
 * $Log$
 * Revision 1.2  2006/06/01 16:13:47  rbolze
 * change to be able to compile with gcc-4
 * Thanks to Abdelkader Amar who has done the work.
 *
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ****************************************************************************/

#ifndef _FULLLINKEDLIST_HH_
#define _FULLLINKEDLIST_HH_

#include <omnithread.h>
#include <sys/types.h>
#include <assert.h>

/****************************************************************************
 * This is a thread safe generic double linked list. It offers ReadWrite
 * and Readonly access to the list with the two iterators Iterator and
 * ReadIterator. Several Readers can exist at a time, while writers have
 * exclusive access to the list. Other iterators created in this time
 * will be blocked until the active iterator is deleted, so don't keep
 * your iterators to long. Write iterators have the possibility to
 * release their write-rights and become a read iterator without
 * releasing their read lock.
 *
 * The current implementation of this list does not distinguish between
 * readers and writers, but the behaviour can be implemented by changing
 * the functions lockReadWrite, lockRead, unlockRead and unlockWrite.
 *
 * This list deals with a copy of all the elements (T) but you can also use
 * some special functions to deal with references, but use them carrefully.
 */
template<class T>class FullLinkedList
{
  friend class ReadIterator;
  friend class Iterator;
/*****************************************************************************
 * PRIVATE FIELDS
 ****************************************************************************/
private:
  /**
   * This structure defines a node of the list.
   */
  struct Node {
    /**
     * next node or \c NULL if it's the last one.
     */
    Node* next;
    /**
     * previous node or \c NULL if it's the first one.
     */
    Node* previous;
    /**
     * a pointer on the element.
     */
    T* element;
  };

  /**
   * indicates the number of element in the list.
   */
  long counter;

  /**
   * the first element of the list or \c NULL if the list is empty.
   */
  Node* first;

  /**
   * the last element of the list or NULL if the list is
   * empty.
   */
  Node* last;

  /**
   * Reader/Writer synchronisation. Will be locked by writers till
   * the writeLock is released.
   */
  mutable omni_mutex writerMutex;

  /**
   * Reader/Writer synchronisation. This variable contains the number
   * of active readers.
   */
  int readerCount;

  /**
   * Reader/Writer synchronisation. Controls the access to readCount; 
   */
  mutable omni_mutex readerCountMutex;

  /**
   * Reader/Writer synchronisation. This Mutex is locked if there are
   * readers and can be used to wait for the readerCount to be empty.
   */
  mutable omni_mutex readersExistMutex;


/*****************************************************************************
 * PUBLIC METHODS
 ****************************************************************************/
public:
  class ReadIterator;
  class Iterator;

  /**
   * creates an new empty list.
   */
  FullLinkedList();

  /**
   * Copy constructor
   * Element T must be have a copy constructor
   */
  FullLinkedList(FullLinkedList<T>& newFLL);

  /**
   * Destroyes the list. The list will be emptied.
   * All elements will be freed.
   */
  ~FullLinkedList();

  /**
   * For affecting a list
   */
  FullLinkedList<T>&
  operator =(FullLinkedList<T>& newFLL);

  /**
   * destroyes all the element of the list. The list becomes an empty
   * list. The Elements of the nodes are freed.
   * Acquires write lock.
   */
  void
  emptyIt();

  /**
   * Push an element on the list. The Element is stored at the
   * end of the list.
   * A copy of the element is done using the copy constructor
   *
   * @param element a pointer on the element to be added
   */
  void
  push(T* element);

  /**
   * Push an element on the list. The Element is stored at the
   * end of the list.
   * !! Only the reference to the element is stored !!
   * Be very carrefull when using this function !
   *
   * @param element a pointer on the element to be added
   */
  void
  pushRef(T* element);

  /**
   * Shift an element in the list. The Element is stored at the
   * begin of the list.
   * A copy of the element is done using the copy constructor
   *
   * @param element a pointer on the element to be added
   */
  void
  shift(T* element);

  /**
   * Shift an element in the list. The Element is stored at the
   * begin of the list.
   * !! Only the reference to the element is stored !!
   * Be very carrefull when using this function !
   *
   * @param element a pointer on the element to be added
   */
  void
  shiftRef(T* element);

  /**
   * pop an element from the list. The element is removed from
   * the end of the list and is returned. Returns NULL if the list
   * is empty
   *
   * @returns The element that has been removed
   */
  T*
  pop();

  /**
   * unshift an element from the list. The element is removed from
   * the begin of the list and is returned. Returns NULL if the list
   * is empty
   *
   * @returns The element that has been removed
   */
  T*
  unshift();

  /**
   * Append the list given in argument to the end of the list. The
   * list given in argument is copied using the defaut construcotr
   * by copy of its element T.
   *
   * @param list The list which is append to the end of the current
   * list. The list will be consumed by the call
   */
  void
  appendList(FullLinkedList<T>* list);

  /**
   * Creates an iterator which controls the linked list. All the access
   * on the list is blocked until the iterator is destroyed. Provides
   * read and write access. Can be reduced to a ReadIterator
   *
   * @returns an iterator that exclusively accesses the list until
   * it is destroyed
   */
  Iterator*
  getIterator();

  /**
   * Creates a iterator which has only readaccess to the list. All
   * write access to the list is blocked until the iterator is
   * destroyed.
   *
   * @returns an iterator that provides readaccess to the list until
   * it is destroyed
   */
  ReadIterator*
  getReadIterator();

  /**
   * Releases the write lock of an readWrite iterator, making it an
   * ReadIterator. The read lock stays all the time. The original
   * iterator will be destroyed and cannot be used any more after this
   * operation.
   *
   * @param rwIterator the iterator to release
   * @returns an iterator with readonly access to the list
   */
  ReadIterator*
  reduceWriteIterator(Iterator* rwIterator);

/*****************************************************************************
 * PRIVATE METHODS
 ****************************************************************************/
private:
  void
  operatorEqualPrivate(FullLinkedList<T>& newFLL);

  void
  emptyItPrivate();

  void
  appendListPrivate(FullLinkedList<T>* list);

  /**
   * Locks the List for ReadWrite access.
   * Unlock with unlockWrite() and unlockRead() (in this
   * order) or with unlockReadWrite().
   */
  void
  lockReadWrite();

  /**
   * Locks the list for Read access.
   * Unlock with unlockRead();
   */
  void
  lockRead();

  /**
   * Unlocks the write mutex only. Read access is
   * still given.
   */
  void
  unlockWrite();

  /**
   * Unlocks the read access. The write lock must already
   * be released.
   */
  void
  unlockRead();

  /**
   * Completely unlocks the write access. Composition of
   * functions unlockWrite() and unlockRead().
   */
  void
  unlockReadWrite();


/*****************************************************************************
 * ITERATOR SECTION - INNER CLASSES
 ****************************************************************************/
public:
  /**
   * Allows readaccess to the linked list. Several readers
   * can exist parallel, but no writer will disturb the
   * reading. The iterators lock on the list will be removed
   * when the iterator is deleted.
   */
  class ReadIterator
  {
    // these two are necessary for the private constructors
    friend class FullLinkedList;
  /***************************************************************************
   * PUBLIC METHODS
   **************************************************************************/
  public:
    /**
     * Releases all locks that this mutex has acquired.
     * If the internal variable noReadRelease is set to true, the
     * readLock is not releases. This allows the reduceWriteIterator
     * in the FullLinkedList to delete an Iterator without loosing
     * the readLock.
     */
    virtual
    ~ReadIterator();

    /**
     * Set the currentElement of the iterator to the first element
     * of the list
     */
    inline void
    reset();

    /**
     * Set the currentElement of the iterator to the last element
     * of the list
     */
    inline void
    resetToLast();

    /**
     * returns true if there is a current element
     */
    inline bool
    hasCurrent();

    /**
     * returns true if there is a next element. If the
     * current element does not exist, false is returned.
     */
    inline bool
    hasNext();

    /**
     * returns true if there is a previous element. If the
     * current element does not exist, false is returned.
     */
    inline bool
    hasPrevious();

    /**
     * Gets the current element.
     * Return NULL if there is no current element.
     * A copy of the current element is returned.
     */
    inline T*
    getCurrent();

    /**
     * Gets the current element.
     * Return NULL if there is no current element.
     * A reference of the current element is returned.
     * This reference is only valid while the iterator exists.
     * !! Be very carrefull when using this method !!
     */
    inline T*
    getCurrentRef();

    /**
     * Iterates to the next element if the possible.
     * A copy of the current element is returned.
     */
    inline T*
    next();

    /**
     * Iterates to the next element if the possible.
     * A reference of the current element is returned.
     * !! Be very carrefull when using this method !!
     */
    inline T*
    nextRef();

    /**
     * Iterates to the previous element if the possible.
     * A copy of the current element is returned.
     */
    inline T*
    previous();

    /**
     * Iterates to the previous element if the possible.
     * A reference of the current element is returned.
     * !! Be very carrefull when using this method !!
     */
    inline T*
    previousRef();

    /**
     * Gets the current length of the list.
     */
    inline unsigned int length();

  /***************************************************************************
   * PROTECTED METHODS
   **************************************************************************/
  protected:
    /**
     * Creates a new ReadIterator. Can only be invoked by
     * functions of FullLinkedList. Assumes the readMutex is
     * already locked. Does not lock the readMutex itself
     */
    ReadIterator(FullLinkedList* controlledList);

  /***************************************************************************
   * PROTECTED FIELDS
   **************************************************************************/
  protected:
    /**
     * contains the linked list this iterator controlls
     */
    FullLinkedList* linkedList;

    /**
     * the current Node of the iterator
     */
   // FullLinkedList::Node* currentNode;
     Node* currentNode;

    /**
     * Controlls the behaviour of unlocking the blocked
     * mutexes when releaseLocks() is called. This is
     * initialized to false, as usually all locks should
     * be released. This value is set to true by the
     * reduceWriteIterator() to allow the creation of a
     * new Iterator without releasing the readlock
     */
    bool noReadRelease;

  };  // End inner class ReadIterator


  /**
   * Allows full access on the linked list. This includes write
   * access as specified in the parentclass ReadIterator as well
   * as functions to add and remove elements. Can be reduced to
   * a ReadIterator by the FullLinkedList.reduceWriteIterator() to
   * release its writeLock.
   */
  class Iterator: public FullLinkedList::ReadIterator
  {
    friend class FullLinkedList;
  /***************************************************************************
   * PUBLIC METHODS
   **************************************************************************/
  public:
    /**
     * Releases the read- and writeLock of this list. If noReadRelease
     * in the parent class is set to true, only the writeLock is released.
     * This allows the construction of a ReadIterator without loosing
     * synchronisation.
     */
    virtual
    ~Iterator();

    /**
     * removes the current element from the list.
     * If the current element doesn't exist, nothing is done.
     * The new current Element will be the next element in the list,
     * and NULL if the removed element was the last element of the
     * list.
     */
    void
    removeCurrent();

    /**
     * removes the current element from the list and return the current
     * element. If the current element doesn't exist, nothing is done
     * and NULL is return.
     * The new current Element will be the next element in the list,
     * and NULL if the removed element was the last element of the
     * list.
     *
     * @returns A pointer to a copy of the element that has been removed
     */
    T*
    removeAndGetCurrent();

    /**
     * inserts a new element after the current element. The
     * current element is unchanged. If the list is empty, a
     * new list is created. The current element will be the
     * new element.
     * If there is no current element, nothing is done.
     * A copy of the element is done through the copy constructor of the
     * element T.
     *
     * @param element A pointer to the element that is added
     */
    void insertAfter(T* element);

    /**
     * inserts a new element after the current element. The
     * current element is unchanged. If the list is empty, a
     * new list is created. The current element will be the
     * new element.
     * If there is no current element, nothing is done.
     * !! Only a reference is copied, use it with special care !!
     *
     * @param element A pointer to the element that is added
     */
    void insertAfterRef(T* element);

    /**
     * inserts a new element before the current element.
     * The current element is unchanged. If the list is empty,
     * a new list is created, the current element is set to the
     * new element.
     * If there is no current element, nothing is done.
     * A copy of the element is done through the copy constructor of the
     * element T.
     *
     * @param element A pointer to the element that is added
     */
    void insertBefore(T* element);

    /**
     * inserts a new element before the current element.
     * The current element is unchanged. If the list is empty,
     * a new list is created, the current element is set to the
     * new element.
     * If there is no current element, nothing is done.
     * !! Only a reference is copied, use it with special care !!
     *
     * @param element A pointer to the element that is added
     */
    void insertBeforeRef(T* element);

  /***************************************************************************
   * PRIVATE METHODS
   **************************************************************************/
  private:
    /**
     * Creates a new Iterator. Can only be invoked by
     * functions of FullLinkedList. Assumes the readWriteMutex is
     * already locked. Does not lock the readWriteMutex itself
     */
    Iterator(FullLinkedList* controlledList);

  };  // end of inner class Iterator

}; // end of class FullLinkedList

/**
 * Include the implementation for this template. This seems to
 * work only like this.
 */
//#include "FullLinkedList.cc"


template<class T>
FullLinkedList<T>::FullLinkedList()
{
  this->first = NULL;
  this->last = NULL;
  this->counter = 0;
  readerCount = 0;
}

template<class T>
FullLinkedList<T>::FullLinkedList(FullLinkedList<T>& newFLL)
{
  this->first = NULL;
  this->last = NULL;
  this->counter = 0;
  lockReadWrite();
  newFLL.lockRead();
  if (newFLL.first != NULL) { // if the new list is not empty
    // creation of the first element
    this->first = new Node();
    this->counter++;
    Node* ownNode = this->first;
    Node* newListNode = newFLL.first;
    ownNode->previous = NULL;
    // call the copy constructor of T
    ownNode->element = new T(*(newListNode->element));
    while (newListNode->next != NULL) {
      ownNode->next = new Node(); // create a new node
      ownNode->next->previous = ownNode; // fix the previous of the new node
      ownNode = ownNode->next; // go to the new node
      newListNode = newListNode->next; // got to the new node
      ownNode->element = new T(*(newListNode->element)); // copy the element
      this->counter++;
    }
    ownNode->next = NULL;
    this->last = ownNode;
  }
  newFLL.unlockRead();
  unlockReadWrite();
}

template<class T>
FullLinkedList<T>::~FullLinkedList()
{
  this->emptyIt();
}

template<class T>
FullLinkedList<T>&
FullLinkedList<T>::operator =(FullLinkedList<T>& newFLL)
{
  lockReadWrite();
  newFLL.lockRead();
  this->operatorEqualPrivate(newFLL);
  newFLL.unlockRead();
  unlockReadWrite();
  return *this;
}

template<class T>
void
FullLinkedList<T>::emptyIt()
{
  lockReadWrite();
  this->emptyItPrivate();
  unlockReadWrite();
}

template<class T>
void
FullLinkedList<T>::push(T* element)
{
  Iterator* it;
  it = getIterator();
  it->resetToLast();
  it->insertAfter(element);
  delete it;
}

template<class T>
void
FullLinkedList<T>::pushRef(T* element)
{
  Iterator* it;
  it = getIterator();
  it->resetToLast();
  it->insertAfterRef(element);
  delete it;
}

template<class T>
void
FullLinkedList<T>::shift(T* element)
{
  Iterator* it;
  it = getIterator();
  it->insertBefore(element);
  delete it;
}

template<class T>
void
FullLinkedList<T>::shiftRef(T* element)
{
  Iterator* it;
  it = getIterator();
  it->insertBeforeRef(element);
  delete it;
}

template<class T>
T*
FullLinkedList<T>::pop()
{
  T* ret;
  Iterator* it = getIterator();
  it->resetToLast();
  if (it->hasCurrent()) {
    ret = it->removeAndGetCurrent();
  } else {
    ret = NULL;
  }
  delete it;
  return ret;
}

template<class T>
T*
FullLinkedList<T>::unshift()
{
  T* ret;
  Iterator* it = getIterator();
  if (it->hasCurrent()) {
    ret = it->removeAndGetCurrent();
  } else {
    ret = NULL;
  }
  delete it;
  return ret;
}

template<class T>
void
FullLinkedList<T>::appendList(FullLinkedList<T>* list)
{
  lockReadWrite();
  list->lockRead();
  this->appendListPrivate(list);
  list->unlockRead();
  unlockReadWrite();
}

template<class T>
typename FullLinkedList<T>::Iterator*
FullLinkedList<T>::getIterator()
{
  lockReadWrite();
  Iterator* it = new Iterator(this);
  return it;
}

template<class T>
typename FullLinkedList<T>::ReadIterator*
FullLinkedList<T>::getReadIterator()
{
  lockRead();
  ReadIterator* it = new ReadIterator(this);
  return it;
}

template<class T>
typename FullLinkedList<T>::ReadIterator*
FullLinkedList<T>::reduceWriteIterator(Iterator* rwIterator)
{
  rwIterator->noReadRelease = true;
  delete rwIterator;
  // no need to lock the ReadMutex, it is still locked
  ReadIterator* it = new ReadIterator(this); 
  return it;
}

template<class T>
void
FullLinkedList<T>::operatorEqualPrivate(FullLinkedList<T>& newFLL)
{
  this->emptyItPrivate();
  if (newFLL.first != NULL) { // if the new list is not empty
    // creation of the first element
    this->first = new Node();
    this->counter++;
    Node* ownNode = this->first;
    Node* newListNode = newFLL.first;
    ownNode->previous = NULL;
    // call the copy constructor of T
    ownNode->element = new T(*(newListNode->element));
    while (newListNode->next != NULL) {
      ownNode->next = new Node(); // create a new node
      ownNode->next->previous = ownNode; // fix the previous of the new node
      ownNode = ownNode->next; // go to the new node
      newListNode = newListNode->next; // got to the new node
      ownNode->element = new T(*(newListNode->element)); // copy the element
      this->counter++;
    }
    ownNode->next = NULL;
    this->last = ownNode;
  }
}

template<class T>
void
FullLinkedList<T>::emptyItPrivate()
{
  if (this->first != NULL) {
    Node* node;
    while (this->first != NULL) {
      delete this->first->element;
      node = this->first;
      this->first = this->first->next;
      delete node;
    }
  }
  this->first = NULL;
  this->last = NULL;
  this->counter = 0;
}

template<class T>
void
FullLinkedList<T>::appendListPrivate(FullLinkedList<T>* list)
{
  if (list == NULL) { // avoid SegFault
    return;
  }
  if (list->first != NULL) { // if the new list is not empty
    if (this->first == NULL) { // if this list is empty
      this->operatorEqualPrivate(*list);
    } else { // really append the list
      this->last->next = new Node();
      Node* ownNode = this->last->next;
      Node* newListNode = list->first;
      ownNode->previous = this->last;
      ownNode->element = new T(*(newListNode->element)); // copy the element
      while (newListNode->next != NULL) {
        ownNode->next = new Node(); // create a new node
        ownNode->next->previous = ownNode; // fix the previous of the new node
        ownNode = ownNode->next; // go to the new node
        newListNode = newListNode->next; // got to the new node
        ownNode->element = new T(*(newListNode->element)); // copy the element
        this->counter++;
      }
      ownNode->next = NULL;
      this->last = ownNode;
    }
  }
}


template<class T>
void
FullLinkedList<T>::lockReadWrite()
{
  // new R / RW locks will block here.
  // this call also guarantes that all active readers
  // are registered in the readerCount
  writerMutex.lock();

  // make sure that all readers are gone:
  readersExistMutex.lock();

  // normally we should use:
  /* readersExistMutex.unlock();
   * readerCountMutex.lock();
   * if readerCount == 0
   *   readersExistMutex.lock();
   * readerCount ++;
   * readerCountMutex.unlock(); */

  // but as we are the only user we can reduce it to:

  readerCount = 1;

  // there exists a reader after this operation, but that's us
}

template<class T>
void
FullLinkedList<T>::lockRead()
{
  // make sure that we
  // - do not disturb writers
  // - no new writer/reader interrupts our acquire
  writerMutex.lock();

  // now make sure we access the readerCount synchronised
  readerCountMutex.lock();
  if (readerCount == 0) {
    // there exist readers now
    readersExistMutex.lock();    // this must not block !
  }
  readerCount++;
  readerCountMutex.unlock();

  // okay, we're finished. open this lock to allow
  // - other readers to connect
  // - other writers to connect (and block)
  writerMutex.unlock();
}

template<class T>
void
FullLinkedList<T>::unlockWrite()
{
  writerMutex.unlock();
  // we're still registered as a reader 
  // nothing more to do...

  /**
   * Note: If there are several Readers and Writers waiting, it
   * would be best to first wake the Writers and then the Readers,
   * but we cannot guarantee this.
   */
}

template<class T>
void
FullLinkedList<T>::unlockRead()
{
  // access readerCount synchronised
  readerCountMutex.lock();
  readerCount--;
  if (readerCount == 0) {
    readersExistMutex.unlock();  // this should not block
  }
  readerCountMutex.unlock();
  // thats it
}

template<class T>
void
FullLinkedList<T>::unlockReadWrite()
{
  unlockWrite();
  unlockRead();
}


/*************** ReadIterator implementation *****************/

template<class T>
FullLinkedList<T>::ReadIterator::ReadIterator(FullLinkedList<T>* controledList)
{
  linkedList = controledList;
  currentNode = linkedList->first;
  noReadRelease = false;
}

template<class T>
FullLinkedList<T>::ReadIterator::~ReadIterator()
{
  if (noReadRelease == false) {
    linkedList->unlockRead();
  }
  // else: keep lock to allow construction of other Iterators
  // without loosing synchronisation
}

template<class T>
inline void
FullLinkedList<T>::ReadIterator::reset()
{
  currentNode = linkedList->first;
}

template<class T>
inline void
FullLinkedList<T>::ReadIterator::resetToLast()
{
  currentNode = linkedList->last;
}

template<class T>
inline bool
FullLinkedList<T>::ReadIterator::hasCurrent()
{
  return (currentNode != NULL);
}

template<class T>
inline bool
FullLinkedList<T>::ReadIterator::hasNext()
{
  if (currentNode == NULL) {
    return false;
  } else {
    return (currentNode->next != NULL);
  }
}

template<class T>
inline bool
FullLinkedList<T>::ReadIterator::hasPrevious()
{
  if (currentNode == NULL) {
    return false;
  } else {
    return (currentNode->previous != NULL);
  }
}

template<class T>
inline T*
FullLinkedList<T>::ReadIterator::getCurrent()
{
  if (!FullLinkedList::ReadIterator::hasCurrent()) {
    return NULL;
  }
  T* ret = new T(*(currentNode->element));
  return ret;
}

template<class T>
inline T*
FullLinkedList<T>::ReadIterator::getCurrentRef()
{
  if (!hasCurrent()) {
    return NULL;
  }
  return currentNode->element;
}

template<class T>
inline T*
FullLinkedList<T>::ReadIterator::next()
{
  if (!hasCurrent()) {
    return NULL;
  }
  T* ret = new T(*(currentNode->element));
  currentNode = currentNode->next;
  return ret;
}

template<class T>
inline T*
FullLinkedList<T>::ReadIterator::nextRef()
{
  if (!hasCurrent()) {
    return NULL;
  }
  T* ret = currentNode->element;
  currentNode = currentNode->next;
  return ret;
}

template<class T>
inline T*
FullLinkedList<T>::ReadIterator::previous()
{
  if (!hasCurrent()) {
    return NULL;
  }
  T* ret = new T(*(currentNode->element));
  currentNode = currentNode->previous;
  return ret;
}

template<class T>
inline T*
FullLinkedList<T>::ReadIterator::previousRef()
{
  if (!hasCurrent()) {
    return NULL;
  }
  T* ret = currentNode->element;
  currentNode = currentNode->previous;
  return ret;
}

template<class T>
inline unsigned int
FullLinkedList<T>::ReadIterator::length()
{
  return linkedList->counter;
}


/*************** (write)Iterator implementation *****************/

template<class T>
FullLinkedList<T>::Iterator::Iterator(FullLinkedList* controledList):
  ReadIterator(controledList)
{
  // nothing to do
}

template<class T>
FullLinkedList<T>::Iterator::~Iterator()
{
  // unlockRead() is called by parent
  FullLinkedList::ReadIterator::linkedList->unlockWrite();
}

template<class T>
void
FullLinkedList<T>::Iterator::removeCurrent()
{
  T* el;
  el = removeAndGetCurrent();
  if (el != NULL) { 
    delete el;
  }
}

template<class T>
T*
FullLinkedList<T>::Iterator::removeAndGetCurrent()
{
  Node* node = FullLinkedList::ReadIterator::currentNode;

  // make sure we have a current element to remove
  if (!FullLinkedList::ReadIterator::hasCurrent()) {
    return NULL;
  }

  // Is this the only element ?
  if (FullLinkedList::ReadIterator::linkedList->counter == 1) {
    FullLinkedList::ReadIterator::linkedList->first = NULL;
    FullLinkedList::ReadIterator::linkedList->last = NULL;
    FullLinkedList::ReadIterator::linkedList->counter = 0;
    FullLinkedList::ReadIterator::currentNode = NULL;
    return node->element;   // node is already initialised
  }

  // there are remaining elements
  // set internal links for list correctly
  if (node->previous == NULL) {         // we are removing the first element
    FullLinkedList::ReadIterator::linkedList->first = node->next;
    // this one works, as there are more than two
    // node in the list and we are the first one:
    node->next->previous = NULL;
    FullLinkedList::ReadIterator::currentNode = node->next;
  } else if (node->next == NULL) {      // we are removing the last element
    FullLinkedList::ReadIterator::linkedList->last = node->previous;
    // this works as there are more than two 
    // nodes in the list and we are the last one:
    node->previous->next = NULL;
    FullLinkedList::ReadIterator::currentNode = node->previous;
  } else {                             // we are removing a middle element
    node->previous->next = node->next;
    node->next->previous = node->previous;
    FullLinkedList::ReadIterator::currentNode = node->next;
  }
  FullLinkedList::ReadIterator::linkedList->counter--;
  return node->element;
}

template<class T>
void
FullLinkedList<T>::Iterator::insertAfter(T* element)
{
  // copy element on the fly and insert as reference
  insertAfterRef(new T(*element));
}

template<class T>
void
FullLinkedList<T>::Iterator::insertAfterRef(T* element)
{
  Node* node = new Node();
  node->element = element;
  node->next = NULL;
  node->previous = NULL;
  FullLinkedList::ReadIterator::linkedList->counter++;

  if (FullLinkedList::ReadIterator::linkedList->first == NULL) {   // build up new list ?
    FullLinkedList::ReadIterator::linkedList->first = node;
    FullLinkedList::ReadIterator::linkedList->last = node;
    FullLinkedList::ReadIterator::currentNode = node;
  } else {                           // insert in existing list
    if (FullLinkedList::ReadIterator::currentNode->next == NULL) { // insert at the end ?
      FullLinkedList::ReadIterator::currentNode->next = node;
      node->previous = FullLinkedList::ReadIterator::currentNode;
      FullLinkedList::ReadIterator::linkedList->last = node;
    } else {                         // insert in the middle
      node->next = FullLinkedList::ReadIterator::currentNode->next;
      node->previous = FullLinkedList::ReadIterator::currentNode;
      node->next->previous = node;
      node->previous->next = node;
    }
  }
}

template<class T>
void
FullLinkedList<T>::Iterator::insertBefore(T* element)
{
  // copy on the fly and insert as Ref
  insertBeforeRef(new T(*element));
}

template<class T>
void
FullLinkedList<T>::Iterator::insertBeforeRef(T* element)
{
  Node* node = new Node();
  node->element = element;
  node->next = NULL;
  node->previous = NULL;
  FullLinkedList::ReadIterator::linkedList->counter++;

  if (FullLinkedList::ReadIterator::linkedList->first == NULL) {        // build up new list ?
    FullLinkedList::ReadIterator::linkedList->first = node;
    FullLinkedList::ReadIterator::linkedList->last = node;
    FullLinkedList::ReadIterator::currentNode = node;
  } else {                                // insert in existing list
    if (FullLinkedList::ReadIterator::currentNode->previous == NULL) {  // insert at the beginning ?
      FullLinkedList::ReadIterator::currentNode->previous = node;
      node->next = FullLinkedList::ReadIterator::currentNode;
      FullLinkedList::ReadIterator::linkedList->first = node;
    } else {                              // insert in the middle
      node->next = FullLinkedList::ReadIterator::currentNode;
      node->previous = FullLinkedList::ReadIterator::currentNode->previous;
      node->next->previous = node;
      node->previous->next = node;
    }
  }
}



#endif // _FULLLINKEDLIST_HH_
