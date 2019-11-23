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

#include "linker.h"
#include "qsqlconnectiondialog.h"
#include "solverdialog.h"
#include <ISolver.h>
#include <IProblem.h>
#include <IBrocker.h>
#include "logging.h"
#include "sqml.h"
#include <QtGui>
#include <QtSql>
#include <QFileDialog>
#include <QMessageBox>
#include <QQuickView>

Linker::Linker(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    table->addAction(insertRowAction);
    table->addAction(deleteRowAction);

    if (QSqlDatabase::drivers().isEmpty())
        QMessageBox::information(this, tr("No database drivers found"),
                                 tr("This program requires at least one Qt database driver. "
                                    "Please check the documentation how to build the "
                                    "Qt SQL plugins."));

    emit statusMessage(tr("Ready."));
}

Linker::~Linker()
{
    if (m_problem_brocker)
        m_problem_brocker->release();
    if (m_solver_brocker)
        m_solver_brocker->release();
}

void Linker::exec()
{
    QStringList connectionNames = QSqlDatabase::connectionNames();

    QSqlQueryModel *model = new QSqlQueryModel(table);
    model->setQuery(QSqlQuery(query, connectionWidget->currentDatabase()));
    table->setModel(model);

    if (model->lastError().type() != QSqlError::NoError)
        emit statusMessage(model->lastError().text());
    else if (model->query().isSelect())
        emit statusMessage(tr("Query OK."));
    else
        emit statusMessage(tr("Query OK, number of affected rows: %1").arg(
                           model->query().numRowsAffected()));

    updateActions();
}

QSqlError Linker::addConnection(const QString &driver, const QString &dbName, const QString &host,
                            const QString &user, const QString &passwd, int port)
{
    static int cCount = 0;

    QSqlError err;
    QSqlDatabase db = QSqlDatabase::addDatabase(driver, QString("SolverDB%1").arg(++cCount));
    db.setDatabaseName(dbName);
    db.setHostName(host);
    db.setPort(port);
    if (!db.open(user, passwd)) {
        err = db.lastError();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(QString("SolverDB%1").arg(cCount));
    } else
        create();

    connectionWidget->refresh();

    return err;
}


void Linker::addConnection()
{
    QSqlConnectionDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

//    if (dialog.useInMemoryDatabase()) {
//        QSqlDatabase::database("in_mem_db", false).close();
//        QSqlDatabase::removeDatabase("in_mem_db");
//        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "in_mem_db");
//        db.setDatabaseName(":memory_solver:");
//        if (!db.open())
//            QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
//                                                                                    "opening the connection: ") + db.lastError().text());
//        QSqlQuery q("", db);
//        q.exec("drop table solvers");
//        q.exec("create table solvers (Name varchar, Description varchar, Path varchar primary key)");
//        q.exec("insert into solvers values ('test', 'solver', 'solverproblem.dll')");

//        connectionWidget->refresh();
//    }
        QSqlError err = addConnection(dialog.driverName(), dialog.databaseName(), dialog.hostName(),
                           dialog.userName(), dialog.password(), dialog.port());

        if (err.type() != QSqlError::NoError)
            QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
                                       "opening the connection: ") + err.text());
}

void Linker::showTable(const QString &t)
{
    QSqlTableModel *model = new QSqlTableModel(table, connectionWidget->currentDatabase());
    model->setEditStrategy(QSqlTableModel::OnRowChange);
    model->setTable(connectionWidget->currentDatabase().driver()->escapeIdentifier(t, QSqlDriver::TableName));
    model->select();
    if (model->lastError().type() != QSqlError::NoError)
        emit statusMessage(model->lastError().text());
    table->setModel(model);
    table->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);

    connect(table->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(currentChanged()));
    updateActions();
}

void Linker::showMetaData(const QString &t)
{
    QSqlRecord rec = connectionWidget->currentDatabase().record(t);
    QStandardItemModel *model = new QStandardItemModel(table);

    model->insertRows(0, rec.count());
    model->insertColumns(0, 7);

    model->setHeaderData(0, Qt::Horizontal, "Fieldname");
    model->setHeaderData(1, Qt::Horizontal, "Type");
    model->setHeaderData(2, Qt::Horizontal, "Length");
    model->setHeaderData(3, Qt::Horizontal, "Precision");
    model->setHeaderData(4, Qt::Horizontal, "Required");
    model->setHeaderData(5, Qt::Horizontal, "AutoValue");
    model->setHeaderData(6, Qt::Horizontal, "DefaultValue");


    for (int i = 0; i < rec.count(); ++i) {
        QSqlField fld = rec.field(i);
        model->setData(model->index(i, 0), fld.name());
        model->setData(model->index(i, 1), fld.typeID() == -1
                ? QString(QVariant::typeToName(fld.type()))
                : QString("%1 (%2)").arg(QVariant::typeToName(fld.type())).arg(fld.typeID()));
        model->setData(model->index(i, 2), fld.length());
        model->setData(model->index(i, 3), fld.precision());
        model->setData(model->index(i, 4), fld.requiredStatus() == -1 ? QVariant("?")
                : QVariant(bool(fld.requiredStatus())));
        model->setData(model->index(i, 5), fld.isAutoValue());
        model->setData(model->index(i, 6), fld.defaultValue());
    }

    table->setModel(model);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    updateActions();
}

void Linker::insertRow()
{
    QSqlTableModel *model = qobject_cast<QSqlTableModel *>(table->model());
    if (!model)
        return;

    QModelIndex insertIndex = table->currentIndex();
    int row = insertIndex.row() == -1 ? 0 : insertIndex.row();
    model->insertRow(row);
    insertIndex = model->index(row, 0);
    table->setCurrentIndex(insertIndex);
    table->edit(insertIndex);
}

void Linker::deleteRow()
{
    QSqlTableModel *model = qobject_cast<QSqlTableModel *>(table->model());
    if (!model)
        return;

    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    QModelIndexList currentSelection = table->selectionModel()->selectedIndexes();
    for (int i = 0; i < currentSelection.count(); ++i) {
        if (currentSelection.at(i).column() != 0)
            continue;
        model->removeRow(currentSelection.at(i).row());
    }

    model->submitAll();
    model->setEditStrategy(QSqlTableModel::OnRowChange);

    updateActions();
}

void Linker::updateActions()
{
    bool enableIns = qobject_cast<QSqlTableModel *>(table->model());
    bool enableDel = enableIns && table->currentIndex().isValid();

    insertRowAction->setEnabled(enableIns);
    deleteRowAction->setEnabled(enableDel);
}

void Linker::about()
{
    QMessageBox::about(this, tr("About"), tr("The Solver Program helps "
        "to find a solution for your optimization problem"
        "by using gradient method."));
}

void Linker::on_browseButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                "C://",
                "DLL Files (*.dll)"
                );
    Linker::filePathProblem->append( filename );
}

void release(IBrocker * problem_brocker, IBrocker * solver_brocker)
{
    problem_brocker->release();
    solver_brocker->release();
}

void Linker::on_addSolverButton_clicked()
{
    SolverDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;


    QSqlDatabase curDB = connectionWidget->currentDatabase();
    QStringList tables = curDB.tables();

    if(tables.count() == 0)
    {
        create();
        tables = curDB.tables();
    }

    QSqlQueryModel *model = new QSqlQueryModel(table);
    QString query = QString("insert into %1 (Name, Description, Path) values ('%2', '%3', '%4');").arg(tables.at(0)).arg(dialog.solverName()).arg(dialog.solverDescription()).arg(dialog.solverPath());
    model->setQuery(QSqlQuery(query, curDB));
    if (model->lastError().type() != QSqlError::NoError) {
        emit statusMessage(model->lastError().text());
    }

    delete model;
}


bool Linker::create()
{
    QSqlDatabase curDB = connectionWidget->currentDatabase();
    QStringList tables = curDB.tables();
    if (tables.count() == 0) {
        QSqlQueryModel *m = new QSqlQueryModel(table);
        QString createTable = "create table solvers"
                "(Name text, Description text, Path text);";
        m->setQuery(QSqlQuery(createTable, curDB));
        if (m->lastError().type() != QSqlError::NoError) {
            emit statusMessage(m->lastError().text());
            delete m;
            return false;
        }
        delete m;
    }
    tables = curDB.tables();
    QSqlTableModel *model = new QSqlTableModel(table, curDB);
    model->setEditStrategy(QSqlTableModel::OnRowChange);
    model->setTable(curDB.driver()->escapeIdentifier(tables.at(0), QSqlDriver::TableName));
    model->select();

    if (model->lastError().type() != QSqlError::NoError) {
        emit statusMessage(model->lastError().text());
        delete model;
        return false;
    }

    table->setModel(model);
    table->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);

    connect(table->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
            this, SLOT(currentChanged()));

    if (tables.count() >= 1)
        emit statusMessage(tr("Query OK."));

    updateActions();
    return true;
}

void Linker::on_table_clicked(const QModelIndex &index)
{
    const QAbstractItemModel * model = index.model();

    Linker::lineSolver->setText( model->data(model->index(index.row(), 2), Qt::DisplayRole).toString());
}

bool Linker::getBrockerFunc(QString text, get_brocker_func & func, QString type)
{

    QFileInfo fileInfo(text);
    if (!fileInfo.exists()) {
        QMessageBox::critical(this, tr("File doesn't exist"), tr("File with ") + type + tr(" doesn't exist."));
        return false;
    }

    QLibrary lib(fileInfo.absoluteFilePath(), this);
    if (!lib.load()) {
        QMessageBox::critical(this, tr("Library with ") + type + tr(" failed to load:"), lib.errorString());
        return false;
    }

    func = reinterpret_cast<get_brocker_func>(lib.resolve("getBrocker"));
    if (!func) {
        QMessageBox::critical(this, tr("getBrocker wasn't resolved"),
                              tr("Failed to get function getBrocker from library with ") + type + ".");
        return false;
    }

    return true;
}

void Linker::on_solveButton_clicked()
{
    QItemSelectionModel *select = table->selectionModel();
    if (!select || select->selectedRows().count() != 1) {
        QMessageBox::critical(this, tr("No solver"), tr("Solver not selected."));
        return;
    }

    IBrocker * problem_brocker = NULL, * solver_brocker = NULL;
    IProblem * problem = NULL;
    ISolver * solver = NULL;

    get_brocker_func problem_brocker_func = NULL, solver_brocker_func = NULL;

    if (!getBrockerFunc(filePathProblem->toPlainText(), problem_brocker_func, "problem"))
         return;
    if (!getBrockerFunc(lineSolver->text(), solver_brocker_func, "solver"))
         return;


    problem_brocker = reinterpret_cast<IBrocker*>(problem_brocker_func());

    if (!problem_brocker) {
           QMessageBox::critical(this, tr("Failed to get brocker"),
                                 tr("Failed to get brocker from library with problem."));
           return;
       } else {
           if (problem_brocker->canCastTo(IBrocker::PROBLEM)) {
               problem = reinterpret_cast<IProblem*>(problem_brocker->getInterfaceImpl(IBrocker::PROBLEM));
           } else {
               problem_brocker->release();
               QMessageBox::critical(this, tr("Failed to cast brocker"),
                                     tr("Failed to cast brocker to problem."));
               return;
           }
       }

    solver_brocker = reinterpret_cast<IBrocker*>(solver_brocker_func());

    if (!solver_brocker) {
          problem_brocker->release();
          QMessageBox::critical(this, tr("Failed to get brocker"),
                                tr("Failed to get brocker from library with solver."));
          return;
      } else {
          if (solver_brocker->canCastTo(IBrocker::SOLVER)) {
              solver = reinterpret_cast<ISolver*>(solver_brocker->getInterfaceImpl(IBrocker::SOLVER));
          } else {
              problem_brocker->release();
              solver_brocker->release();
              QMessageBox::critical(this, tr("Failed to cast brocker"),
                                    tr("Failed to cast brocker to solver."));
              return;
          }
      }

    if (solver->setProblem(problem) != ERR_OK) {
        release(problem_brocker, solver_brocker);
        QMessageBox::critical(this, tr("Failed to set problem"),
                              tr("Failed to set problem to solver."));
        return;
    }

    QUrl url;
    url.setPath(select->selectedRows(2).at(0).data().toString());
    if (solver->getQml(url) != ERR_OK) {
        release(problem_brocker, solver_brocker);
        QMessageBox::critical(this, tr("Failed to get dialog"),
                              tr("Failed to get dialog from solver for setting parameters."));
        return;
    }

    QQuickView * qmlView = new QQuickView();
    qmlView->setSource(url);
    qmlView->resize(640, 480);

    m_problem = problem;
    SolveQML * sqml = new SolveQML(solver, problem, qmlView, this);
    QObject * item = (QObject *)qmlView->rootObject();
    QObject::connect(item, SIGNAL(qmlSignal(QString)), sqml, SLOT(solve(QString)));

    qmlView->show();
    m_problem_brocker = problem_brocker;
    m_solver_brocker = solver_brocker;
}


void Linker::on_Draw_clicked()
{
    uint coord = Number_C->text().toInt();
    double a = Left_B->text().toDouble();
    double b = Right_B->text().toDouble();
    IVector * args = solution->clone();
    if (coord > solution->getDim())
        LOG("ERR: Dimension mismatch in draw section");
    int steps = (b - a) / 0.1 + 2;
    QVector<double> coords(steps), func(steps);
    Plot->clearGraphs();
    Plot->addGraph();
    int i = 0;

    for (double step = a; step <= b; step += 0.1, i++)
    {
        coords[i] = step;
        args->setCoord(coord, step);
        if (m_problem->goalFunctionByArgs(args, func[i]) != ERR_OK)
             LOG("ERR: Failed to calculate goal function in draw section");
    }

    Plot->graph(0)->setData(coords, func);
    Plot->xAxis->setLabel("coords");
    Plot->yAxis->setLabel("func");
    Plot->xAxis->setRange(a, b);

    double minval = func[0], maxval = func[0];

    for (int i = 1; i < steps; ++i)
       {
           if (func[i] < minval)
               minval = func[i];
           else if (func[i] > maxval)
               maxval = func[i];
       }
    QVector<double> solution_c(1), solution_f(1);
    solution->getCoord(coord, solution_c[0]);
    if (m_problem->goalFunctionByArgs(solution, solution_f[0]) != ERR_OK)
         LOG("ERR: Failed to calculate goal function in draw section");
    Plot->yAxis->setRange(minval, maxval);
    Plot->addGraph(Plot->xAxis, Plot->yAxis);
    Plot->graph(1)->setPen(QColor(50, 50, 50, 255));
    Plot->graph(1)->setLineStyle(QCPGraph::lsNone);
    Plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
    Plot->graph(1)->setData(solution_c, solution_f);

    Plot->replot();

}

