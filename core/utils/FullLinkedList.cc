/**
 * @file FullLinkedList.hh
 *
 * @brief Thread safe generic Double linked list with full access, implementation
 *     Warning included in FullLinkedList.hh
 *
 * @author - Gaël Le Mahec (gael.le.mahec@ens-lyon.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

template<class T>
FullLinkedList<T>::FullLinkedList() {
  this->first = NULL;
  this->last = NULL;
  this->counter = 0;
  readerCount = 0;
}

template<class T>
FullLinkedList<T>::FullLinkedList(FullLinkedList<T>& newFLL) {
  this->first = NULL;
  this->last = NULL;
  this->counter = 0;
  lockReadWrite();
  newFLL.lockRead();
  if (newFLL.first != NULL) {
    // if the new list is not empty
    // creation of the first element
    this->first = new Node();
    this->counter++;
    Node* ownNode = this->first;
    Node* newListNode = newFLL.first;
    ownNode->previous = NULL;
    // call the copy constructor of T
    ownNode->element = new T(*(newListNode->element));
    while (newListNode->next != NULL) {
      ownNode->next = new Node();
      ownNode->next->previous = ownNode;
      ownNode = ownNode->next;
      newListNode = newListNode->next;
      ownNode->element = new T(*(newListNode->element));
      this->counter++;
    }
    ownNode->next = NULL;
    this->last = ownNode;
  }
  newFLL.unlockRead();
  unlockReadWrite();
}

template<class T>
FullLinkedList<T>::~FullLinkedList() {
  this->emptyIt();
}

template<class T>
FullLinkedList<T>&
FullLinkedList<T>::operator =(FullLinkedList<T>& newFLL) {
  lockReadWrite();
  newFLL.lockRead();
  this->operatorEqualPrivate(newFLL);
  newFLL.unlockRead();
  unlockReadWrite();
  return *this;
}

template<class T>
void
FullLinkedList<T>::emptyIt() {
  lockReadWrite();
  this->emptyItPrivate();
  unlockReadWrite();
}

template<class T>
void
FullLinkedList<T>::push(T* element) {
  Iterator* it;
  it = getIterator();
  it->resetToLast();
  it->insertAfter(element);
  delete it;
}

template<class T>
void
FullLinkedList<T>::pushRef(T* element) {
  Iterator* it;
  it = getIterator();
  it->resetToLast();
  it->insertAfterRef(element);
  delete it;
}

template<class T>
void
FullLinkedList<T>::shift(T* element) {
  Iterator* it;
  it = getIterator();
  it->insertBefore(element);
  delete it;
}

template<class T>
void
FullLinkedList<T>::shiftRef(T* element) {
  Iterator* it;
  it = getIterator();
  it->insertBeforeRef(element);
  delete it;
}

template<class T>
T*
FullLinkedList<T>::pop() {
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
FullLinkedList<T>::unshift() {
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
FullLinkedList<T>::appendList(FullLinkedList<T>* list) {
  lockReadWrite();
  list->lockRead();
  this->appendListPrivate(list);
  list->unlockRead();
  unlockReadWrite();
}

template<class T>
typename FullLinkedList<T>::Iterator*
FullLinkedList<T>::getIterator() {
  lockReadWrite();
  Iterator* it = new Iterator(this);
  return it;
}

template<class T>
typename FullLinkedList<T>::ReadIterator*
FullLinkedList<T>::getReadIterator() {
  lockRead();
  ReadIterator* it = new ReadIterator(this);
  return it;
}

template<class T>
typename FullLinkedList<T>::ReadIterator*
FullLinkedList<T>::reduceWriteIterator(Iterator* rwIterator) {
  rwIterator->noReadRelease = true;
  delete rwIterator;
  // no need to lock the ReadMutex, it is still locked
  ReadIterator* it = new ReadIterator(this);
  return it;
}

template<class T>
void
FullLinkedList<T>::operatorEqualPrivate(FullLinkedList<T>& newFLL) {
  this->emptyItPrivate();
  if (newFLL.first != NULL) {
    // if the new list is not empty
    // creation of the first element
    this->first = new Node();
    this->counter++;
    Node* ownNode = this->first;
    Node* newListNode = newFLL.first;
    ownNode->previous = NULL;
    // call the copy constructor of T
    ownNode->element = new T(*(newListNode->element));
    while (newListNode->next != NULL) {
      ownNode->next = new Node();
      ownNode->next->previous = ownNode;
      ownNode = ownNode->next;
      newListNode = newListNode->next;
      ownNode->element = new T(*(newListNode->element));
      this->counter++;
    }
    ownNode->next = NULL;
    this->last = ownNode;
  }
}

template<class T>
void
FullLinkedList<T>::emptyItPrivate() {
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
FullLinkedList<T>::appendListPrivate(FullLinkedList<T>* list) {
  if (list == NULL) {
    // avoid SegFault
    return;
  }
  if (list->first != NULL) {
    // if the new list is not empty
    if (this->first == NULL) {
      // if this list is empty
      this->operatorEqualPrivate(*list);
    } else {
      // really append the list
      this->last->next = new Node();
      Node* ownNode = this->last->next;
      Node* newListNode = list->first;
      this->counter++;
      ownNode->previous = this->last;
      ownNode->element = new T(*(newListNode->element));
      while (newListNode->next != NULL) {
        ownNode->next = new Node();
        ownNode->next->previous = ownNode;
        ownNode = ownNode->next;
        newListNode = newListNode->next;
        ownNode->element = new T(*(newListNode->element));
        this->counter++;
      }
      ownNode->next = NULL;
      this->last = ownNode;
    }
  }
}


template<class T>
void
FullLinkedList<T>::lockReadWrite() {
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
FullLinkedList<T>::lockRead() {
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
FullLinkedList<T>::unlockWrite() {
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
FullLinkedList<T>::unlockRead() {
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
FullLinkedList<T>::unlockReadWrite() {
  unlockWrite();
  unlockRead();
}


/*************** ReadIterator implementation *****************/
template<class T>
FullLinkedList<T>::ReadIterator::ReadIterator(FullLinkedList<T>* controledList) {
  linkedList = controledList;
  currentNode = linkedList->first;
  noReadRelease = false;
}

template<class T>
FullLinkedList<T>::ReadIterator::~ReadIterator() {
  if (noReadRelease == false) {
    linkedList->unlockRead();
  }
  // else: keep lock to allow construction of other Iterators
  // without loosing synchronisation
}

template<class T>
inline void
FullLinkedList<T>::ReadIterator::reset() {
  currentNode = linkedList->first;
}

template<class T>
inline void
FullLinkedList<T>::ReadIterator::resetToLast() {
  currentNode = linkedList->last;
}

template<class T>
inline bool
FullLinkedList<T>::ReadIterator::hasCurrent() {
  return (currentNode != NULL);
}

template<class T>
inline bool
FullLinkedList<T>::ReadIterator::hasNext() {
  if (currentNode == NULL) {
    return false;
  } else {
    return (currentNode->next != NULL);
  }
}

template<class T>
inline bool
FullLinkedList<T>::ReadIterator::hasPrevious() {
  if (currentNode == NULL) {
    return false;
  } else {
    return (currentNode->previous != NULL);
  }
}

template<class T>
inline T*
FullLinkedList<T>::ReadIterator::getCurrent() {
  if (!FullLinkedList::ReadIterator::hasCurrent()) {
    return NULL;
  }
  T* ret = new T(*(currentNode->element));
  return ret;
}

template<class T>
inline T*
FullLinkedList<T>::ReadIterator::getCurrentRef() {
  if (!hasCurrent()) {
    return NULL;
  }
  return currentNode->element;
}

template<class T>
inline T*
FullLinkedList<T>::ReadIterator::next() {
  if (!hasCurrent()) {
    return NULL;
  }
  T* ret = new T(*(currentNode->element));
  currentNode = currentNode->next;
  return ret;
}

template<class T>
inline T*
FullLinkedList<T>::ReadIterator::nextRef() {
  if (!hasCurrent()) {
    return NULL;
  }
  T* ret = currentNode->element;
  currentNode = currentNode->next;
  return ret;
}

template<class T>
inline T*
FullLinkedList<T>::ReadIterator::previous() {
  if (!hasCurrent()) {
    return NULL;
  }
  T* ret = new T(*(currentNode->element));
  currentNode = currentNode->previous;
  return ret;
}

template<class T>
inline T*
FullLinkedList<T>::ReadIterator::previousRef() {
  if (!hasCurrent()) {
    return NULL;
  }
  T* ret = currentNode->element;
  currentNode = currentNode->previous;
  return ret;
}

template<class T>
inline unsigned int
FullLinkedList<T>::ReadIterator::length() {
  return linkedList->counter;
}


/*************** (write)Iterator implementation *****************/

template<class T>
FullLinkedList<T>::Iterator::Iterator(FullLinkedList* controledList)
  : ReadIterator(controledList) {
  // nothing to do
}

template<class T>
FullLinkedList<T>::Iterator::~Iterator() {
  // unlockRead() is called by parent
  FullLinkedList::ReadIterator::linkedList->unlockWrite();
}

template<class T>
void
FullLinkedList<T>::Iterator::removeCurrent() {
  T* el;
  el = removeAndGetCurrent();
  if (el != NULL) {
    delete el;
  }
}

template<class T>
T*
FullLinkedList<T>::Iterator::removeAndGetCurrent() {
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
  if (node->previous == NULL) {
    // we are removing the first element
    FullLinkedList::ReadIterator::linkedList->first = node->next;
    // this one works, as there are more than two
    // node in the list and we are the first one:
    node->next->previous = NULL;
    FullLinkedList::ReadIterator::currentNode = node->next;
  } else if (node->next == NULL) {
    // we are removing the last element
    FullLinkedList::ReadIterator::linkedList->last = node->previous;
    // this works as there are more than two
    // nodes in the list and we are the last one:
    node->previous->next = NULL;
    FullLinkedList::ReadIterator::currentNode = node->previous;
  } else {
    // we are removing a middle element
    node->previous->next = node->next;
    node->next->previous = node->previous;
    FullLinkedList::ReadIterator::currentNode = node->next;
  }
  FullLinkedList::ReadIterator::linkedList->counter--;
  return node->element;
}

template<class T>
void
FullLinkedList<T>::Iterator::insertAfter(T* element) {
  // copy element on the fly and insert as reference
  insertAfterRef(new T(*element));
}

template<class T>
void
FullLinkedList<T>::Iterator::insertAfterRef(T* element) {
  Node* node = new Node();
  node->element = element;
  node->next = NULL;
  node->previous = NULL;
  FullLinkedList::ReadIterator::linkedList->counter++;

  if (FullLinkedList::ReadIterator::linkedList->first == NULL) {
    // build up new list ?
    FullLinkedList::ReadIterator::linkedList->first = node;
    FullLinkedList::ReadIterator::linkedList->last = node;
    FullLinkedList::ReadIterator::currentNode = node;
  } else {
    // insert in existing list
    if (FullLinkedList::ReadIterator::currentNode->next == NULL) {
      // insert at the end ?
      FullLinkedList::ReadIterator::currentNode->next = node;
      node->previous = FullLinkedList::ReadIterator::currentNode;
      FullLinkedList::ReadIterator::linkedList->last = node;
    } else {
      // insert in the middle
      node->next = FullLinkedList::ReadIterator::currentNode->next;
      node->previous = FullLinkedList::ReadIterator::currentNode;
      node->next->previous = node;
      node->previous->next = node;
    }
  }
}

template<class T>
void
FullLinkedList<T>::Iterator::insertBefore(T* element) {
  // copy on the fly and insert as Ref
  insertBeforeRef(new T(*element));
}

template<class T>
void
FullLinkedList<T>::Iterator::insertBeforeRef(T* element) {
  Node* node = new Node();
  node->element = element;
  node->next = NULL;
  node->previous = NULL;
  FullLinkedList::ReadIterator::linkedList->counter++;

  if (FullLinkedList::ReadIterator::linkedList->first == NULL) {
    // build up new list ?
    FullLinkedList::ReadIterator::linkedList->first = node;
    FullLinkedList::ReadIterator::linkedList->last = node;
    FullLinkedList::ReadIterator::currentNode = node;
  } else {
    // insert in existing list
    if (FullLinkedList::ReadIterator::currentNode->previous == NULL) {
      // insert at the beginning ?
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

