/*
 * timeserver.cpp - Entity time server
 * Copyright (C) 2001, 2002, 2007  Justin Karneges, Maciej Niedzielski
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include "timeserver.h"
#include "systeminfo.h"
#include "xmpp_xmlcommon.h"
#include <QDateTime>

using namespace XMPP;


/**
 * \class TimeServer
 * \brief Server current time
 *
 * This serving task answers XEP-0202 and XEP-0090 queries
 */

TimeServer::TimeServer(Task *parent)
:Task(parent)
{
}

TimeServer::~TimeServer()
{
}

bool TimeServer::take(const QDomElement &e)
{
	if (e.tagName() != "iq" || e.attribute("type") != "get")
		return false;

	QString ns = e.firstChildElement("time").attribute("xmlns");
	if (ns == "urn:xmpp:time") {
		QDomElement iq = createIQ(doc(), "result", e.attribute("from"), e.attribute("id"));
		QDomElement time = doc()->createElement("time");
		time.setAttribute("xmlns", ns);
		iq.appendChild(time);

		QDateTime local = QDateTime::currentDateTime();
		int off = SystemInfo::instance()->timezoneOffset();
		QTime t = QTime(0, 0).addSecs(qAbs(off)*60);
		QString tzo = (off < 0 ? "-" : "+") + t.toString("HH:mm");
		time.appendChild(textTag(doc(), "tzo", tzo));
		QString localTimeStr = local.toUTC().toString(Qt::ISODate);
		if (!localTimeStr.endsWith("Z"))
			localTimeStr.append("Z");
		time.appendChild(textTag(doc(), "utc", localTimeStr));

		send(iq);
		return true;
	}
	return false;
}
