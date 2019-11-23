#include "solverdialogqml.h"
#include "ui_solverdialogqml.h"

SolverDialogQML::SolverDialogQML(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SolverDialogQML)
{
    ui->setupUi(this);
}

SolverDialogQML::~SolverDialogQML()
{
    delete ui;
}
