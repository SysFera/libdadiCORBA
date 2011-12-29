/**
 * @file automtest_fulllinkedList.cpp
 * @brief This file implements the libdadicorba tests for linked lists
 * @author Kevin Coulomb (kevin.coulomb@sysfera.com)
 * @section Licence
 *  |LICENCE|
 */

#include <boost/test/unit_test.hpp>
#include "FullLinkedList.hh"

BOOST_AUTO_TEST_SUITE( test_suite )


using namespace std;

BOOST_AUTO_TEST_CASE( getReadIter )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  FullLinkedList<int>::ReadIterator* it = l->getReadIterator();
  BOOST_REQUIRE(it!=NULL);
}

BOOST_AUTO_TEST_CASE( constructorSize )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  FullLinkedList<int>::ReadIterator* it = l->getReadIterator();
  BOOST_REQUIRE(it->length()==0);
}

BOOST_AUTO_TEST_CASE( push )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  int i = 1;
  l->push(&i);
  FullLinkedList<int>::ReadIterator* it = l->getReadIterator();
  BOOST_REQUIRE(it->length()==1);
}

BOOST_AUTO_TEST_CASE( pushRef )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  int i = 1;
  l->pushRef(&i);
  FullLinkedList<int>::ReadIterator* it = l->getReadIterator();
  BOOST_REQUIRE(it->length()==1);
}

BOOST_AUTO_TEST_CASE( copyConst )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  int i = 1;
  l->push(&i);
  FullLinkedList<int>* l2 = new FullLinkedList<int>(*l);
  FullLinkedList<int>::ReadIterator* it = l2->getReadIterator();
  BOOST_REQUIRE(it->length()==1);
}


BOOST_AUTO_TEST_CASE( empty )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  int i = 1;
  l->push(&i);
  l->emptyIt();
  FullLinkedList<int>::ReadIterator* it = l->getReadIterator();
  BOOST_REQUIRE(it->length()==0);
}

BOOST_AUTO_TEST_CASE( pop )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  int i = 1;
  l->push(&i);
  l->pop();
  FullLinkedList<int>::ReadIterator* it = l->getReadIterator();
  BOOST_REQUIRE(it->length()==0);
}

BOOST_AUTO_TEST_CASE( append )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  FullLinkedList<int>* l2 = new FullLinkedList<int>();
  int i = 1;
  int j = 2;
  l->push(&i);
  l2->push(&j);
  l->appendList(l2);
  FullLinkedList<int>::ReadIterator* it = l->getReadIterator();
  BOOST_REQUIRE(it->length()==2);
}

BOOST_AUTO_TEST_CASE( getIter )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  FullLinkedList<int>::Iterator* it = l->getIterator();
  BOOST_REQUIRE(it!=NULL);
}


BOOST_AUTO_TEST_CASE( hasCurrentT )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  int i = 1;
  l->push(&i);
  FullLinkedList<int>::ReadIterator* it = l->getReadIterator();
  BOOST_REQUIRE(it->hasCurrent());
}

BOOST_AUTO_TEST_CASE( hasCurrentF )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  FullLinkedList<int>::ReadIterator* it = l->getReadIterator();
  BOOST_REQUIRE(it->hasCurrent()==false);
}


BOOST_AUTO_TEST_CASE( hasNextF )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  int i = 1;
  l->push(&i);
  FullLinkedList<int>::ReadIterator* it = l->getReadIterator();
  BOOST_REQUIRE(it->hasNext()==false);
}

BOOST_AUTO_TEST_CASE( hasNextT )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  int i = 1;
  int j = 2;
  l->push(&i);
  l->push(&j);
  FullLinkedList<int>::ReadIterator* it = l->getReadIterator();
  BOOST_REQUIRE(it->hasNext());
}

BOOST_AUTO_TEST_CASE( next )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  int i = 1;
  int j = 2;
  l->push(&i);
  l->push(&j);
  FullLinkedList<int>::ReadIterator* it = l->getReadIterator();
  it->next();
}



BOOST_AUTO_TEST_CASE( hasPrevF )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  int i = 1;
  l->push(&i);
  FullLinkedList<int>::ReadIterator* it = l->getReadIterator();
  BOOST_REQUIRE(it->hasPrevious()==false);
}

BOOST_AUTO_TEST_CASE( hasPrevT )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  int i = 1;
  int j = 2;
  l->push(&i);
  l->push(&j);
  FullLinkedList<int>::ReadIterator* it = l->getReadIterator();
  it->next();
  BOOST_REQUIRE(it->hasPrevious());
}

BOOST_AUTO_TEST_CASE( prev )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  int i = 1;
  int j = 2;
  l->push(&i);
  l->push(&j);
  FullLinkedList<int>::Iterator* it = l->getIterator();
  it->next();
  it->previous();
}

BOOST_AUTO_TEST_CASE( getCurrent )
{
  FullLinkedList<int>* l = new FullLinkedList<int>();
  int i = 1;
  l->push(&i);
  FullLinkedList<int>::Iterator* it = l->getIterator();
  BOOST_REQUIRE(*(it->getCurrent())==1);
}

BOOST_AUTO_TEST_SUITE_END()

// THE END
