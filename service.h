/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -p service connman-service.xml
 *
 * qdbusxml2cpp is Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef SERVICE_H_1294049950
#define SERVICE_H_1294049950

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

template <class T>
inline T argVarConvert(const QVariant &v)
{
	if (qVariantCanConvert<T>(v))
		return qvariant_cast<T>(v);
		
	QDBusArgument a = qvariant_cast<QDBusArgument>(v);
	return qdbus_cast<T>(a);
}

/*
 * Proxy class for interface net.connman.Service
 */
class NetConnmanServiceInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "net.connman.Service"; }

public:
    NetConnmanServiceInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~NetConnmanServiceInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> ClearProperty(const QString &name)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(name);
        return asyncCallWithArgumentList(QLatin1String("ClearProperty"), argumentList);
    }

    inline QDBusPendingReply<> Connect()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("Connect"), argumentList);
    }

    inline QDBusPendingReply<> Disconnect()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("Disconnect"), argumentList);
    }

    inline QDBusPendingReply<QVariantMap> GetProperties()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("GetProperties"), argumentList);
    }

    inline QDBusPendingReply<> MoveAfter(const QDBusObjectPath &service)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(service);
        return asyncCallWithArgumentList(QLatin1String("MoveAfter"), argumentList);
    }

    inline QDBusPendingReply<> MoveBefore(const QDBusObjectPath &service)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(service);
        return asyncCallWithArgumentList(QLatin1String("MoveBefore"), argumentList);
    }

    inline QDBusPendingReply<> Remove()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("Remove"), argumentList);
    }

    inline QDBusPendingReply<> SetProperty(const QString &name, const QDBusVariant &value)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(name) << qVariantFromValue(value);
        return asyncCallWithArgumentList(QLatin1String("SetProperty"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void PropertyChanged(const QString &name, const QDBusVariant &value);
};

namespace net {
  namespace connman {
    typedef ::NetConnmanServiceInterface Service;
  }
}
#endif
