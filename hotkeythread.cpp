#include "hotkeythread.h"
#include <process.h>

#define WM_END_THREAD (WM_USER+2)

HotKeyThread::HotKeyThread(QObject *parent)
    : QThread(parent)
{
    this->m_threadId = 0;
    m_atomCounter = 0;
}

HotKeyThread::~HotKeyThread()
{
}

void HotKeyThread::stop()
{
    if(this->m_threadId != 0)
        ::PostThreadMessage(this->m_threadId, WM_END_THREAD, 0, 0);
}

/**
 * @brief HotKeyThread::addHotKey
 * @param modifier
 * @param key
 * @see http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
 */
bool HotKeyThread::registerHotKey(int modifier, int key)
{
    // register an atom, and a hotkey
    bool retVal;
    int magicNum = 1128;
    ATOM id = ::GlobalAddAtom(MAKEINTATOM(magicNum + m_atomCounter++));
    m_hotKeyIdList.append(id);

    if(QSysInfo::windowsVersion() > QSysInfo::WV_VISTA)
    {
        retVal = ::RegisterHotKey(NULL, id, modifier | MOD_NOREPEAT, key);
    }
    else
    {
        // No repeat is only supported in Win7 and later
        retVal = ::RegisterHotKey(NULL, id, modifier, key);
    }

    return retVal;
}

void HotKeyThread::run()
{
    // store a thread id so we can exit later
    m_threadId = ::GetCurrentThreadId();

    if (!registerHotKey(MOD_WIN, KEY_Z))
        qDebug() << "Impossible d'assigner la raccourci clavier !";

    // wait on hotkeys
    MSG msg;
    while (0 < ::GetMessage(&msg, NULL, 0, 0))
    {
        if(msg.message == WM_HOTKEY)
        {
            int key = HIWORD(msg.lParam);
            int modifier = LOWORD(msg.lParam);
            // Notify MainWindow of the event
            emit hotKeyEvent(modifier, key);
        }
        else if(msg.message == WM_END_THREAD)
        {
            // exit
            break;
        }
    }

    // Clean up Hotkeys
    for (int i = 0; i < m_hotKeyIdList.size(); ++i)
    {
        ATOM id = m_hotKeyIdList.at(i);
        ::UnregisterHotKey(NULL, id);
        ::GlobalDeleteAtom(id);
    }
}
