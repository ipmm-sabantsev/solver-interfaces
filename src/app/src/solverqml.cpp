#include <QGraphicsObject>
#include <QVariant>
#include <QList>
#include <QQuickView>

#include "solverqml.h"

/*
SolverQML::SolverQML(const QUrl &source, unsigned argsDim, unsigned paramsDim, QWidget *parent)
    : QDialog(parent), ui(new Ui_solverqml)
{
    ui->setupUi(this);

    dimArgs = argsDim;
    dimParams = paramsDim;

    QQuickView * qmlView = new QQuickView();
    //qmlView->rootContext()->setContextProperty("params", params);
    qmlView->setSource(source);
    qmlView->setResizeMode(QQuickView::SizeViewToRootObject);

    qmlView->show();
    //ui->vLayout->addWidget(qmlView);
    //ui->vLayout->addWidget(qmlView);

    //root = qmlView->rootObject();
}

SolverQML::~SolverQML()
{
}

QList<QObject*> getTextInputs(QObject * root)
{
    QList<QObject*> res, children = root->findChildren<QObject*>();
    foreach (QObject * val, children) {
        if (!QString(val->metaObject()->className()).compare("QDeclarativeTextInput"))
            res.append(val);
    }
    return res;
}

QString SolverQML::getParameters() const
{
    QList<QObject*> children = getTextInputs(root);
    QString res("args:");
    res.append(QString::number(dimArgs) + " params:" + QString::number(dimParams) + " ");
    foreach (QObject * val, children) {
        res.append(val->property("objectName").toString() + ":" + val->property("text").toString() + " ");
    }
    res.remove(res.length() - 1, 1);
    return res;
}
*/
