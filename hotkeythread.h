/**
 * http://stackoverflow.com/questions/18504995/clipboard-shortcut-hotkey-binding-with-qt-outside-of-application/18521207#18521207
 */

#ifndef HOTKEYTHREAD_H
#define HOTKEYTHREAD_H

#include <QThread>
#include <QDebug>

#define KEY_B 0x42
#define KEY_N 0x4E
#define KEY_Z 0x5A
#define MOD_NOREPEAT 0x4000
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class HotKeyThread : public QThread
{
    Q_OBJECT

public:
    HotKeyThread(QObject *parent);
    ~HotKeyThread();

signals:
    void hotKeyEvent(int, int);

public slots:
    void run();
    void stop();

protected:
    bool registerHotKey(int modifier, int key);

private:
    volatile bool m_stopped;
    DWORD m_threadId;
    int m_atomCounter;
    QList<ATOM> m_hotKeyIdList;
};

#endif // HOTKEYTHREAD_H
