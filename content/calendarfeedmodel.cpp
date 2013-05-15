/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include "calendarfeedmodel.h"

#include <QDebug>

#include <QDateTime>
#include <QStringList>

#include <actions.h>

#include <calendarlistmodel.h>

CalendarFeedModel::CalendarFeedModel(const QString &searchText, QObject *parent):
        McaFeedModel(parent)
{
    m_source = new CalendarListModel(this);
    m_source->setModelType(UtilMethods::EAllEvents);
    m_actions = new McaActions;

    connect(m_source, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
            this, SLOT(sourceRowsAboutToBeInserted(QModelIndex,int,int)));
    connect(m_source, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(sourceRowsInserted(QModelIndex,int,int)));
    connect(m_source, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(sourceRowsAboutToBeRemoved(QModelIndex,int,int)));
    connect(m_source, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(sourceRowsRemoved(QModelIndex,int,int)));
    connect(m_source, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)),
            this, SLOT(sourceRowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)));
    connect(m_source, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
            this, SLOT(sourceRowsMoved(QModelIndex,int,int,QModelIndex,int)));
    connect(m_source, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(sourceDataChanged(QModelIndex,QModelIndex)));
    connect(m_source, SIGNAL(modelAboutToBeReset()),
            this, SLOT(sourceModelAboutToBeReset()));
    connect(m_source, SIGNAL(modelReset()),
            this, SLOT(sourceModelReset()));

    connect(m_actions, SIGNAL(standardAction(QString,QString)),
            this, SLOT(performAction(QString,QString)));

    setSearchText(searchText);
}

CalendarFeedModel::~CalendarFeedModel()
{
    delete m_actions;
}

//
// public member functions
//

void CalendarFeedModel::setSearchText(const QString &text)
{
    if (m_searchText == text)
        return;

    if (text.isEmpty()) {
        int count = rowCount();
        if (count > 0) {
            beginRemoveRows(QModelIndex(), 0, count - 1);
            m_searchText = text;
            endRemoveRows();
        }
        return;
    }

    m_source->filterOut(text, static_cast<FilterTerminate*>(this));

    if (m_searchText.isEmpty()) {
        int count = m_source->rowCount();
        if (count > 0) {
            beginInsertRows(QModelIndex(), 0, count - 1);
            m_searchText = text;
            endInsertRows();
        }
        return;
    }

    m_searchText = text;
}

int CalendarFeedModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (m_searchText.isEmpty())
        return 0;
    return m_source->rowCount();
}

QVariant CalendarFeedModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row >= m_source->rowCount())
        return QVariant();

    QModelIndex sourceIndex = m_source->index(row);

    switch (role) {
    case RequiredTypeRole:
        return "content";

    case RequiredUniqueIdRole:
        return m_source->data(sourceIndex, CalendarDataItem::Uid);

    case RequiredTimestampRole:
        {
            QDateTime dt;
            dt.setDate(m_source->data(sourceIndex, CalendarDataItem::StartDate).toDate());
            dt.setTime(m_source->data(sourceIndex, CalendarDataItem::StartTime).toTime());
            return dt;
        }

    case GenericTitleRole:
        return m_source->data(sourceIndex, CalendarDataItem::Summary);

    case GenericContentRole:
        return m_source->data(sourceIndex, CalendarDataItem::Location);

    case CommonActionsRole:
        return QVariant::fromValue<McaActions*>(m_actions);

    default:
        return QVariant();
    }

    return QVariant();
}

//
// protected slots
//

void CalendarFeedModel::sourceRowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)

    if (m_searchText.isEmpty())
        return;
    beginInsertRows(QModelIndex(), start, end);
}

void CalendarFeedModel::sourceRowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    if (m_searchText.isEmpty())
        return;
    endInsertRows();
}

void CalendarFeedModel::sourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)

    if (m_searchText.isEmpty())
        return;
    beginRemoveRows(QModelIndex(), start, end);
}

void CalendarFeedModel::sourceRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    if (m_searchText.isEmpty())
        return;
    endRemoveRows();
}

void CalendarFeedModel::sourceRowsAboutToBeMoved(const QModelIndex &source, int start, int end,
                                                 const QModelIndex &dest, int destStart)
{
    Q_UNUSED(source)
    Q_UNUSED(dest)

    if (m_searchText.isEmpty())
        return;
    beginMoveRows(QModelIndex(), start, end, QModelIndex(), destStart);
}

void CalendarFeedModel::sourceRowsMoved(const QModelIndex &source, int start, int end,
                                        const QModelIndex &dest, int destStart)
{
    Q_UNUSED(source)
    Q_UNUSED(start)
    Q_UNUSED(end)
    Q_UNUSED(dest)
    Q_UNUSED(destStart)

    if (m_searchText.isEmpty())
        return;
    endMoveRows();
}

void CalendarFeedModel::sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if (m_searchText.isEmpty())
        return;
    QModelIndex top = index(topLeft.row(), 0);
    QModelIndex bottom = index(bottomRight.row(), 0);
    emit dataChanged(top, bottom);
}

void CalendarFeedModel::sourceModelAboutToBeReset()
{
    if (m_searchText.isEmpty())
        return;
    beginResetModel();
}

void CalendarFeedModel::sourceModelReset()
{
    if (m_searchText.isEmpty())
        return;
    endResetModel();
}

void CalendarFeedModel::performAction(QString action, QString uniqueid)
{
    qDebug() << "Action" << action << "called for calendar item" << uniqueid;

    if (action == "default") {
        QString executable("meego-qml-launcher");
        QStringList parameters;
        parameters << "--app" << "meego-app-calendar";
        parameters << "--opengl" << "--fullscreen";
        parameters << "--cmd" << "openCalendar";
        parameters << "--cdata" << uniqueid;
        QProcess::startDetached(executable, parameters);
    }
}

bool CalendarFeedModel::filteringStopped()
{
    return isSearchHalted();
}
