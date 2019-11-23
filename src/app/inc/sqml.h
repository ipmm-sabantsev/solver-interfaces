#ifndef SQML_H
#define SQML_H

#include <QQuickView>
#include <QList>
#include <QString>
#include <QScopedPointer>
#include "error.h"
#include "ISolver.h"
#include "IProblem.h"
#include "linker.h"

QList<QObject*> getTextInputs(QObject * root);

class SolveQML : public QObject {
    Q_OBJECT
public:
    SolveQML(ISolver * solver,
             IProblem * problem,
             QQuickView * qml,
             Linker * link) :
        QObject(qml), m_solver(solver), m_problem(problem),
        qmlView(qml), link(link)
    {
    }

    virtual ~SolveQML()
    {        
    }

private:    
    ISolver * m_solver;
    IProblem * m_problem;
    QQuickView * qmlView;
    Linker * link;

public slots:
    void solve(const QString & msg);

};

#endif // SQML_H
