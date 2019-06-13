#include "Unit_FrameCommand.h"

#ifdef _CPPUNIT_TEST

CUnit_FrameCommand::CUnit_FrameCommand()
{
    m_pFrameCommand = NULL;
}

void CUnit_FrameCommand::setUp(void)
{
    m_pFrameCommand = new CFrameCommand();
}

void CUnit_FrameCommand::tearDown(void)
{
    SAFE_DELETE(m_pFrameCommand);
}

void CUnit_FrameCommand::Test_Frame_Command(void)
{
    bool blRet = false;
    IBuffPacket* pBuffPacket = App_BuffPacketManager::instance()->Create();

    if (CONSOLE_MESSAGE_SUCCESS == m_pFrameCommand->DoFrameCommand("ShowModule -a", pBuffPacket))
    {
        blRet = true;
    }

    CPPUNIT_ASSERT_MESSAGE("[Test_Frame_Command]DoFrameCommand is fail.", true == blRet);
}

#endif


