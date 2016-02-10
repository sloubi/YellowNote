#include <QUrlQuery>
#include <QNetworkInterface>
#include <QHostInfo>
#include "oauth2.h"
#include "o2/o2.h"
#include "o2/o2replyserver.h"
#include "o2/o2globals.h"
#include "o2/o2settingsstore.h"
#define trace() if (1) qDebug()

void Oauth2::link()
{
    trace() << "O2::link";

    if (linked()) {
        trace() << " Linked already";
        emit linkingSucceeded();
        return;
    }

    if(grantFlow_ == GrantFlowAuthorizationCode){

        // Start listening to authentication replies
        replyServer_->listen(QHostAddress::Any, localPort_);

        // Save redirect URI, as we have to reuse it when requesting the access token
        redirectUri_ = localhostPolicy_.arg(replyServer_->serverPort());

        // Assemble intial authentication URL
        QList<QPair<QString, QString> > parameters;
        parameters.append(qMakePair(QString(O2_OAUTH2_RESPONSE_TYPE), (grantFlow_ == GrantFlowAuthorizationCode) ? QString(O2_OAUTH2_CODE) : QString(O2_OAUTH2_TOKEN)));
        parameters.append(qMakePair(QString(O2_OAUTH2_CLIENT_ID), clientId_));
        parameters.append(qMakePair(QString(O2_OAUTH2_REDIRECT_URI), redirectUri_));
        // parameters.append(qMakePair(QString(OAUTH2_REDIRECT_URI), QString(QUrl::toPercentEncoding(redirectUri_))));
        parameters.append(qMakePair(QString(O2_OAUTH2_SCOPE), scope_));
        parameters.append(qMakePair(QString(O2_OAUTH2_API_KEY), apiKey_));

        // Show authentication URL with a web browser
        QUrl url(requestUrl_);
    #if QT_VERSION < 0x050000
        url.setQueryItems(parameters);
    #else
        QUrlQuery query(url);
        query.setQueryItems(parameters);
        url.setQuery(query);
    #endif

        trace() << "Emit openBrowser" << url.toString();
        emit openBrowser(url);

    }

    else if(grantFlow_ == GrantFlowResourceOwnerPasswordCredentials)
    {
        QUrl url(tokenUrl_);

        QUrlQuery params;
        params.addQueryItem(O2_OAUTH2_CLIENT_ID,clientId_);
        params.addQueryItem(O2_OAUTH2_CLIENT_SECRET,clientSecret_);
        params.addQueryItem(O2_OAUTH2_USERNAME,username_);
        params.addQueryItem(O2_OAUTH2_PASSWORD,password_);
        params.addQueryItem(O2_OAUTH2_GRANT_TYPE,"password");
        params.addQueryItem("device_uid", getMacAddress());
        params.addQueryItem("device_name", "PC (" + QHostInfo::localHostName() + ")");

        QNetworkRequest tokenRequest(url);
        tokenRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        QNetworkReply *tokenReply = manager_->post(tokenRequest, params.toString(QUrl::FullyEncoded).toUtf8());

        connect(tokenReply, SIGNAL(finished()), this, SLOT(onTokenReplyFinished()), Qt::QueuedConnection);
        connect(tokenReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onTokenReplyError(QNetworkReply::NetworkError)), Qt::QueuedConnection);
    }
}

QString Oauth2::getMacAddress()
{
    foreach(QNetworkInterface netInterface, QNetworkInterface::allInterfaces())
    {
        // Return only the first non-loopback MAC Address
        if (!(netInterface.flags() & QNetworkInterface::IsLoopBack))
            return netInterface.hardwareAddress();
    }
    return QString();
}

O2AbstractStore* Oauth2::store()
{
    return store_;
}
