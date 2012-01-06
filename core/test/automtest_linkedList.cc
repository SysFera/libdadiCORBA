/**
 * @file automtest_linkedList.cpp
 * @brief This file implements the libdadi tests for linked lists
 * @author Kevin Coulomb (kevin.coulomb@sysfera.com)
 * @section Licence
 *  |LICENCE|
 */

#include "LinkedList.hh"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(test_suite)


using namespace std;

BOOST_AUTO_TEST_CASE(constructorSize)
{
  LinkedList<int> l = LinkedList<int>();
  BOOST_REQUIRE(l.length()==0);
}

BOOST_AUTO_TEST_CASE(addSize)
{
  LinkedList<int> l = LinkedList<int>();
  int* i = (int *)malloc (sizeof (int));
  *i = 1;
  l.addElement(i);
  BOOST_REQUIRE(l.length()==1);
  l.emptyIt();
}

BOOST_AUTO_TEST_CASE(copyConst)
{
  LinkedList<int> l = LinkedList<int>();
  int* i = (int *)malloc (sizeof (int));
  *i = 1;
  l.addElement(i);
  LinkedList<int>* l2 = new LinkedList<int>(l);
  BOOST_REQUIRE(l2->length()==1);
  l2->emptyIt();
  l.emptyIt();
  delete l2;
}


BOOST_AUTO_TEST_CASE(empty)
{
  LinkedList<int>* l = new LinkedList<int>();
  int* i = (int *)malloc (sizeof (int));
  *i = 1;
  l->addElement(i);
  l->emptyIt();
  BOOST_REQUIRE(l->length()==0);
  delete l;
}

BOOST_AUTO_TEST_CASE(pop)
{
  LinkedList<int>* l = new LinkedList<int>();
  int* i = (int *)malloc (sizeof (int));
  *i = 1;
  l->addElement(i);
  l->pop();
  BOOST_REQUIRE(l->length()==0);
  delete l;
}

BOOST_AUTO_TEST_CASE(append)
{
  LinkedList<int>* l = new LinkedList<int>();
  LinkedList<int>* l2 = new LinkedList<int>();
  int* i = (int *)malloc (sizeof (int));
  *i = 1;
  int* j = (int *)malloc (sizeof (int));
  *j = 2;
  l->addElement(i);
  l2->addElement(j);
  l->append(l2);
  BOOST_REQUIRE(l->length()==2);
  l->emptyIt();
  l2->emptyIt();
  delete l;
  delete l2;
}

BOOST_AUTO_TEST_CASE(getIter)
{
  LinkedList<int>* l = new LinkedList<int>();
  LinkedList<int>::Iterator* it = l->getIterator();
  BOOST_REQUIRE(it!=NULL);
}


BOOST_AUTO_TEST_CASE(hasCurrentT)
{
  LinkedList<int>* l = new LinkedList<int>();
  int* i = (int *)malloc (sizeof (int));
  *i = 1;
  l->addElement(i);
  LinkedList<int>::Iterator* it = l->getIterator();
  BOOST_REQUIRE(it->hasCurrent());
  delete it;
  l->emptyIt();
  delete l;
}

BOOST_AUTO_TEST_CASE(hasCurrentF)
{
  LinkedList<int>* l = new LinkedList<int>();
  LinkedList<int>::Iterator* it = l->getIterator();
  BOOST_REQUIRE(it->hasCurrent()==false);
  delete it;
  l->emptyIt();
  delete l;
}


BOOST_AUTO_TEST_CASE(hasNextF)
{
  LinkedList<int>* l = new LinkedList<int>();
  int* i = (int *)malloc (sizeof (int));
  *i = 1;
  l->addElement(i);
  LinkedList<int>::Iterator* it = l->getIterator();
  BOOST_REQUIRE(it->hasNext()==false);
  delete it;
  l->emptyIt();
  delete l;
}

BOOST_AUTO_TEST_CASE(hasNextT)
{
  LinkedList<int>* l = new LinkedList<int>();
  int* i = (int *)malloc (sizeof (int));
  *i = 1;
  int* j = (int *)malloc (sizeof (int));
  *j = 2;
  l->addElement(i);
  l->addElement(j);
  LinkedList<int>::Iterator* it = l->getIterator();
  BOOST_REQUIRE(it->hasNext());
  delete it;
  l->emptyIt();
  delete l;
}

BOOST_AUTO_TEST_CASE(next)
{
  LinkedList<int>* l = new LinkedList<int>();
  int* i = (int *)malloc (sizeof (int));
  *i = 1;
  int* j = (int *)malloc (sizeof (int));
  *j = 1;
  l->addElement(i);
  l->addElement(j);
  LinkedList<int>::Iterator* it = l->getIterator();
  it->next();
  delete it;
  l->emptyIt();
  delete l;
}



BOOST_AUTO_TEST_CASE(hasPrevF)
{
  LinkedList<int>* l = new LinkedList<int>();
  int* i = (int *)malloc (sizeof (int));
  *i = 1;
  l->addElement(i);
  LinkedList<int>::Iterator* it = l->getIterator();
  BOOST_REQUIRE(it->hasPrevious()==false);
  delete it;
  l->emptyIt();
  delete l;
}

BOOST_AUTO_TEST_CASE(hasPrevT)
{
  LinkedList<int>* l = new LinkedList<int>();
  int* i = (int *)malloc (sizeof (int));
  *i = 1;
  int* j = (int *)malloc (sizeof (int));
  *j = 2;
  l->addElement(i);
  l->addElement(j);
  LinkedList<int>::Iterator* it = l->getIterator();
  it->next();
  BOOST_REQUIRE(it->hasPrevious());
  delete it;
  l->emptyIt();
  delete l;
}

BOOST_AUTO_TEST_CASE(prev)
{
  LinkedList<int>* l = new LinkedList<int>();
  int* i = (int *)malloc (sizeof (int));
  *i = 1;
  int* j = (int *)malloc (sizeof (int));
  *j = 1;
  l->addElement(i);
  l->addElement(j);
  LinkedList<int>::Iterator* it = l->getIterator();
  it->next();
  it->previous();
  delete it;
  l->emptyIt();
  delete l;
}

BOOST_AUTO_TEST_CASE(getCurrent)
{
  LinkedList<int>* l = new LinkedList<int>();
  int* i = (int *)malloc (sizeof (int));
  *i = 1;
  l->addElement(i);
  LinkedList<int>::Iterator* it = l->getIterator();
  BOOST_REQUIRE(*(it->getCurrent())==1);
  delete it;
  l->emptyIt();
  delete l;
}

BOOST_AUTO_TEST_CASE(setCurrent)
{
  LinkedList<int>* l = new LinkedList<int>();
  int* i = (int *)malloc (sizeof (int));
  *i = 1;
  int* j = (int *)malloc (sizeof (int));
  *j = 2;
  l->addElement(i);
  LinkedList<int>::Iterator* it = l->getIterator();
  it->setCurrent(j);
  BOOST_REQUIRE(*(it->getCurrent())==2);
  delete it;
  l->emptyIt();
  delete l;
}

BOOST_AUTO_TEST_CASE(delCurrent)
{
  LinkedList<int>* l = new LinkedList<int>();
  int* i = (int *)malloc (sizeof (int));
  *i = 1;
  l->addElement(i);
  LinkedList<int>::Iterator* it = l->getIterator();
  it->removeCurrent();
  delete it;
  BOOST_REQUIRE(l->length()==0);
  l->emptyIt();
  delete l;
}

BOOST_AUTO_TEST_SUITE_END()

// THE END
