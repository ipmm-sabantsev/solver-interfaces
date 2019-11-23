/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef BROWSER_H
#define BROWSER_H

#include <QWidget>
#include "ui_linkerwidget.h"
#include "IBrocker.h"
#include "ISolver.h"
#include "IProblem.h"
class ConnectionWidget;
QT_FORWARD_DECLARE_CLASS(QTableView)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QTextEdit)
QT_FORWARD_DECLARE_CLASS(QSqlError)

class Linker: public QWidget, private Ui::Linker
{
    Q_OBJECT
public:
    Linker(QWidget *parent = 0);
    virtual ~Linker();

    QSqlError addConnection(const QString &driver, const QString &dbName, const QString &host,
                  const QString &user, const QString &passwd, int port = -1);

    void insertRow();
    void deleteRow();
    void updateActions();
    bool create();
private:
    QString query;
    IVector * solution = nullptr;
    typedef IBrocker *(*get_brocker_func)();
    IProblem * m_problem;
    ISolver * _solver;
    IBrocker * m_problem_brocker;
    IBrocker * m_solver_brocker;

    bool getBrockerFunc(QString text, get_brocker_func & func, QString type);

public slots:
    void exec();
    void showTable(const QString &table);
    void showMetaData(const QString &table);
    void addConnection();
    void currentChanged() { updateActions(); }
    void about();

    void on_insertRowAction_triggered()
    { insertRow(); }
    void on_deleteRowAction_triggered()
    { deleteRow(); }
    void on_connectionWidget_tableActivated(const QString &table)
    { showTable(table); }
    void on_connectionWidget_metaDataRequested(const QString &table)
    { showMetaData(table); }

    void on_clearButton_clicked()
    {
        filePathProblem->clear();
        filePathProblem->setFocus();
    }
    void setSolution(IVector * solution)
    {
        QString res("");
        for (int i = 0; i < solution->getDim(); i++) {
            double elem;
            if (solution->getCoord(i, elem) != ERR_OK)
                return;

            res.append(QString::number(elem) + " ");
        }

        lineResult->insert(res);

        this->solution = solution;
    }

signals:
    void statusMessage(const QString &message);
private slots:
    void on_browseButton_clicked();
    void on_addSolverButton_clicked();

    void on_solveButton_clicked();
    void on_table_clicked(const QModelIndex &index);

    void on_Draw_clicked();
};

#endif
