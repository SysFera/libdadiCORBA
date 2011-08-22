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
#include "FullLinkedList.cc"

#endif // _FULLLINKEDLIST_HH_
