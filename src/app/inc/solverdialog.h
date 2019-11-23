#ifndef SOLVERDIALOG_H
#define SOLVERDIALOG_H

#include <QDialog>
#include "ui_solverdialog.h"

class SolverDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SolverDialog(QWidget *parent = 0);
    QString solverName() const;
    QString solverDescription() const;
    QString solverPath() const;
    ~SolverDialog();

private slots:


    void on_addButton_clicked()
    {
        accept();
    }
    void on_cancelButton_clicked()
    {
        reject();
    }
    void on_browseSolverButton_clicked();

private:
    Ui::SolverDialog *ui;
};

#endif // SOLVERDIALOG_H
