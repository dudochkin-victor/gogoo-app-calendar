/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef __calendarservicemodel_h
#define __calendarservicemodel_h

#include <servicemodel.h>

class CalendarServiceModel: public McaServiceModel
{
    Q_OBJECT

public:
    CalendarServiceModel(QObject *parent = NULL);
    ~CalendarServiceModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
};

#endif  // __calendarservicemodel_h
