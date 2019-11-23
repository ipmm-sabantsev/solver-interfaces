#ifndef SOLVERQML_H
#define SOLVERQML_H

#include <QDialog>
#include "ui_solverqml.h"

class SolverQML : public QDialog
{
    Q_OBJECT

public:
    explicit SolverQML(const QUrl &source, unsigned argsDim, unsigned paramsDim, QWidget *parent = 0);
    ~SolverQML();

    QString getParameters() const;

private:
    Ui_solverqml * ui;
    QObject * root;
    unsigned dimArgs;
    unsigned dimParams;
};

#endif // SOLVERQML_H
