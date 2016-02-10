#define MOD_NOREPEAT 0x4000
#define MOD_ALT 0x0001
#define MOD_WIN 0x0008

#include <QDebug>
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    qDebug() << "start";
    bool hotkeyLoop(true);
    RegisterHotKey(NULL, 1, MOD_WIN | MOD_NOREPEAT, 0x5A); // WIN + Z

    QApplication a(argc, argv);

    MainWindow w(&hotkeyLoop);
    w.show();

    QApplication::processEvents();

    // Boucle infinie pour les raccourcis globaux
    MSG msg;
    while(GetMessage(&msg,NULL,0,0) != 0 && hotkeyLoop)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_HOTKEY)
        {
            qDebug() << "touche 1";
            if (msg.wParam == 1)
            {
                qDebug() << "touche 2";
                //w.openNoteDialog();
            }
        }
    }

    // return a.exec() ne fonctionne pas à cause de
    // la gestion des raccourcis globaux
    return 0;
}
