#ifndef SOLVERDIALOGQML_H
#define SOLVERDIALOGQML_H

#include <QDialog>

namespace Ui {
class SolverDialogQML;
}

class SolverDialogQML : public QDialog
{
    Q_OBJECT

public:
    explicit SolverDialogQML(QWidget *parent = 0);
    ~SolverDialogQML();

private:
    Ui::SolverDialogQML *ui;
};

#endif // SOLVERDIALOGQML_H
