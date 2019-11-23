#include <QMessageBox>
#include "sqml.h"

QList<QObject*> getTextInputs(QObject * root)
{
    QList<QObject*> res, children = root->findChildren<QObject*>();
    foreach (QObject * val, children) {
        if (!QString(val->metaObject()->className()).compare("QQuickTextInput"))
            res.append(val);
    }
    return res;
}

void SolveQML::solve(const QString & msg) {
    unsigned int dimArgs, dimParams;
    if (m_problem->getArgsDim(dimArgs) != ERR_OK) {
        return;
    }
    if (m_problem->getParamsDim(dimParams) != ERR_OK) {
        return;
    }

    QList<QObject*> children = getTextInputs(qmlView);
    QString res;
    res.append(QString::number(dimArgs) + " " + QString::number(dimParams) + " ");
    foreach (QObject * val, children) {
        res.append(val->property("text").toString() + " ");
    }
    res.remove(res.length() - 1, 1);
    if (m_solver->setParams(res) != ERR_OK) {
          return;
    }
    if (m_solver->solve() != ERR_OK) {
      return;
    }

    IVector * solution;
    if (m_solver->getSolution(solution) != ERR_OK) {
        return;
    }

    link->setSolution(solution);
    qmlView->close();
}
