#include "solverdialog.h"
#include "ui_solverdialog.h"
#include "linker.h"

#include <QFileDialog>

#include "qsqlconnectiondialog.h"

#include <QtGui>
#include <QtSql>
#include <QMessageBox>
#include "ui_linkerwidget.h"

SolverDialog::SolverDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SolverDialog)
{
    ui->setupUi(this);


}

SolverDialog::~SolverDialog()
{}


void SolverDialog::on_browseSolverButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                "C://",
                "DLL Files (*.dll)"
                );

    ui->filePathSolver->append(filename);
}

QString SolverDialog::solverName() const
{
    return ui->solverName->text();
}

QString SolverDialog::solverDescription() const
{
    return ui->solverDesc->text();
}

QString SolverDialog::solverPath() const
{
    return ui->filePathSolver->toPlainText();
}


