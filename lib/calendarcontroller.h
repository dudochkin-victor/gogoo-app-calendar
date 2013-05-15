/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef CALENDARCONTROLLER_H
#define CALENDARCONTROLLER_H

#include <extendedcalendar.h>
#include <extendedstorage.h>
#include <notebook.h>
#include <sqlitestorage.h>
#include <kdatetime.h>
#include <duration.h>
#include <QDir>
#include <QDebug>
#include <QtDeclarative/qdeclarative.h>
#include <QObject>


#include <incidenceio.h>
#include  <utilmethods.h>


using namespace mKCal;

class CalendarController : public QObject
{
    Q_OBJECT;

public:
    CalendarController(QObject *parent = 0);
    ~CalendarController();

    Q_INVOKABLE bool addModifyEvent(int actionType, QObject* eventIO);
    Q_INVOKABLE bool deleteEvent(QString eventUId);
    Q_INVOKABLE QList<IncidenceIO> getEventsFromDB(int listType,KDateTime startDate=KDateTime::currentLocalDateTime(), KDateTime endDate=KDateTime::currentLocalDateTime(),const QString uid="");
    Q_INVOKABLE QObject* getEventForEdit(const QString uid);

private:
    bool setUpCalendars();
    void handleRepeat(KCalCore::Event::Ptr coreEventPtr,const IncidenceIO& eventIO);
    void handleEventTime(KCalCore::Event::Ptr coreEventPtr,const IncidenceIO& eventIO);
    void handleAlarm(const IncidenceIO& eventIO,KCalCore::Alarm::Ptr eventAlarm);    


private:    
    ExtendedCalendar::Ptr calendar;
    ExtendedStorage::Ptr storage;

    Notebook *notebook;
    QString nUid;
};

#endif // CALENDARCONTROLLER_H
