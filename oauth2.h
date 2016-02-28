#ifndef OAUTH2_H
#define OAUTH2_H

#include "o2/o2.h"

class Oauth2 : public O2
{
    Q_OBJECT
public:
    O2AbstractStore* store();
    static QString getMacAddress();

public slots:
    /// Authenticate.
    Q_INVOKABLE virtual void link();
};

#endif // OAUTH2_H
