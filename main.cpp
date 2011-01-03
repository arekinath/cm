#include <QtDBus>
#include <QCoreApplication>

#include "manager.h"
#include "service.h"

#include <iostream>
#include <QMetaType>

void printService(const QString &path)
{
	NetConnmanServiceInterface svc("net.connman", path, QDBusConnection::systemBus());

	QDBusPendingReply<QVariantMap> repl = svc.GetProperties();
	repl.waitForFinished();
	QVariantMap props = repl.value();
	
	QString name = argVarConvert<QString>(props.value("Name"));
	
	std::cout << qPrintable(name) << "\t" << qPrintable(path) << "\n";
	QString type = argVarConvert<QString>(props.value("Type"));
	QString state = argVarConvert<QString>(props.value("State"));
	bool autoConnect = argVarConvert<bool>(props.value("AutoConnect"));
	bool fav = argVarConvert<bool>(props.value("Favorite"));
	std::cout << "\ttype:" << qPrintable(type) << " state:" << qPrintable(state);
	if (autoConnect)
		std::cout << " auto";
	if (fav)
		std::cout << " fav";
	std::cout << "\n";
	
	if (props.keys().contains("IPv4")) {
		QVariantMap ipv4 = argVarConvert<QVariantMap>(props.value("IPv4"));
		QString method = argVarConvert<QString>(ipv4.value("Method"));
		QString address = argVarConvert<QString>(ipv4.value("Address"));
		QString netmask = argVarConvert<QString>(ipv4.value("Netmask"));
		QString gateway = argVarConvert<QString>(ipv4.value("Gateway"));
		
		std::cout << "\tipv4 method:" << qPrintable(method) << " addr:" << qPrintable(address)
			<< " mask:" << qPrintable(netmask) << " gw:" << qPrintable(gateway) << "\n";
	}
	
	if (props.keys().contains("IPv6")) {
		QVariantMap ipv6 = argVarConvert<QVariantMap>(props.value("IPv6"));
	}
	
	if (props.keys().contains("Nameservers")) {
		QStringList ns = argVarConvert<QStringList>(props.value("Nameservers"));
		std::cout << "\tns: " << qPrintable(ns.join(" ")) << "\n";
	}
	
	if (props.keys().contains("Domains")) {
		QStringList ns = argVarConvert<QStringList>(props.value("Domains"));
		std::cout << "\tdomains: " << qPrintable(ns.join(" ")) << "\n";
	}
	
	if (type == "wifi") {
		QString security = argVarConvert<QString>(props.value("Security"));
		QString passph = argVarConvert<QString>(props.value("Passphrase"));
		
		std::cout << "\twifi sec:" << qPrintable(security) << " pass:" << qPrintable(passph) << "\n";
	}
	
	if (type == "ethernet") {
		QVariantMap eth = argVarConvert<QVariantMap>(props.value("Ethernet"));
		QString intfn = argVarConvert<QString>(eth.value("Interface"));
		QString mac = argVarConvert<QString>(eth.value("Address"));
		quint16 mtu = argVarConvert<quint16>(eth.value("MTU"));
		std::cout << "\ton:" << qPrintable(intfn) << " mac:" << qPrintable(mac);
		std::cout << " mtu:" << mtu << "\n";
	}
	
	std::cout << "\n";
}

QString cidrMask(const QString &sbits)
{
	bool ok;
	int bits = sbits.toInt(&ok, 10);
	if (!ok || bits < 1 || bits > 31) {
		std::cerr << "cm: could not process CIDR-style mask\n";
		exit(1);
	}
	
	quint32 mask = 0xffffffff;
	mask = mask << (32 - bits);
	
	QStringList octets;
	octets.append(QString::number((mask & 0xff000000) >> 24));
	octets.append(QString::number((mask & 0x00ff0000) >> 16));
	octets.append(QString::number((mask & 0x0000ff00) >> 8));
	octets.append(QString::number( mask & 0x000000ff));
	
	return octets.join(".");
}

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
	QString intf(argv[1]);
	
	NetConnmanManagerInterface cm("net.connman", "/", QDBusConnection::systemBus());
	if (!cm.isValid()) {
		std::cout << "Could not connect to connman on D-bus. Is connmand running?\n";
		return 1;
	}
	
	if (intf == "") {
		QList<QDBusObjectPath> paths = cm.ServicePaths();

		for (int n = 0; n < paths.size(); ++n) {
			QDBusObjectPath path = paths.at(n);
			printService(path.path());
		}
	} else {
		QDBusPendingReply<QDBusObjectPath> repl = cm.LookupService(intf);
		repl.waitForFinished();
		if (!repl.isValid()) {
			std::cerr << "cm: error looking for service: LookupService reply invalid\n";
			return 1;
		}
		
		QString path = repl.value().path();
		if (argc == 2) {
			printService(path);
		} else {
			NetConnmanServiceInterface svc("net.connman", path, QDBusConnection::systemBus());
			
			QString op(argv[2]);
			op = op.toLower();
			
			QRegExp ipAddr("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}");
			QRegExp cidr("([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})/([0-9]+)");
			
			if (op == "up" || op == "connect") {
				QDBusPendingReply<> crep = svc.Connect();
				crep.waitForFinished();
				
				if (crep.isError()) {
					std::cerr << "cm: error connecting service: " 
										<< qPrintable(crep.error().message()) << "\n";
					return 1;
				}
			} else if (op == "down" || op == "disconnect") {
				QDBusPendingReply<> drep = svc.Disconnect();
				drep.waitForFinished();
				
				if (drep.isError()) {
					std::cerr << "cm: error disconnecting service: " 
										<< qPrintable(drep.error().message()) << "\n";
					return 1;
				}
			} else if (op == "pass" || op == "pwd" || op == "passphrase") {
				QString ps(argv[3]);
				QDBusPendingReply<> rep = svc.SetProperty("Passphrase", QDBusVariant(ps));
				rep.waitForFinished();
				
				if (rep.isError()) {
					std::cerr << "cm: error setting passphrase: " 
					          << qPrintable(rep.error().message()) << "\n";
					return 1;
				}
			} else if (op == "ipv4" || ipAddr.exactMatch(op) || cidr.exactMatch(op)) {
				QString ips[3]; // addr mask gw
				QString method = "manual";
				int at = 0;
				
				for (int i = 2; i < argc; ++i) {
					QString arg(argv[i]);
					if (ipAddr.exactMatch(arg)) {
						ips[at++] = arg;
					} else if (cidr.exactMatch(arg)) {
						ips[0] = cidr.cap(1);
						ips[1] = cidrMask(cidr.cap(2));
					} else if (arg == "addr" || arg == "address") {
						QString addr = argv[++i];
						if (ipAddr.exactMatch(addr)) {
							ips[0] = addr;
						} else if (cidr.exactMatch(addr)) {
							ips[0] = cidr.cap(1);
							ips[1] = cidrMask(cidr.cap(2));
						}
					} else if (arg == "mask" || arg == "netmask") {
						ips[1] = QString(argv[++i]);
					} else if (arg == "gw" || arg == "gateway") {
						ips[2] = QString(argv[++i]);
					} else if (arg == "dhcp" || arg == "auto") {
						method = "dhcp";
					}
				}
				
				/*QDBusPendingReply<QVariantMap> repl = svc.GetProperties();
				repl.waitForFinished();
				QVariantMap props = repl.value();*/
				
				QVariantMap ipv4; //  = argVarConvert<QVariantMap>(props.value("IPv4.Configuration"));
				ipv4.insert("Method", QVariant(method));
				
				if (!ips[0].isEmpty())
					ipv4.insert("Address", QVariant(ips[0]));
				if (!ips[1].isEmpty())
					ipv4.insert("Netmask", QVariant(ips[1]));
				if (!ips[2].isEmpty())
					ipv4.insert("Gateway", QVariant(ips[2]));

				QDBusPendingReply<> rep = svc.SetProperty("IPv4.Configuration", QDBusVariant(ipv4));
				rep.waitForFinished();
				if (rep.isError()) {
					std::cerr << "cm: error while setting new configuration: "
										<< qPrintable(rep.error().message()) << "\n";
					return 1;
				}
				
			} else {
				std::cerr << "cm: I don't know that operation?\n";
				return 1;
			}
		}
	}

  return 0;
}
