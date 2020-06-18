#ifndef _UNIT_CONNECTTCP_H
#define _UNIT_CONNECTTCP_H

#ifdef _CPPUNIT_TEST

#include "Unit_Common.h"
#include "PostServerMessage.h"

const short COMMAND_AUTOTEST_HEAD = 0x1000;

class CUnit_ConnectTcp : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CUnit_ConnectTcp);
    CPPUNIT_TEST(Test_Connect_Tcp_Server);
    CPPUNIT_TEST(Test_Connect_Tcp_Server_With_Local);
    CPPUNIT_TEST_SUITE_END();

public:
    CUnit_ConnectTcp();

    virtual void setUp(void);

    virtual void tearDown(void);

    void Test_Connect_Tcp_Server(void) const;

    void Test_Connect_Tcp_Server_With_Local(void) const;

private:
    int m_nServerID = 0;
};

#endif

#endif
