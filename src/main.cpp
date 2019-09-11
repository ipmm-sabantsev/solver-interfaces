#include <QString>
#include <QVector>
#include <QScopedPointer>

#pragma warning(push)
#pragma warning(disable: 4100)
#include <logging.h>
#include <IVector.h>
#include <ICompact.h>
#pragma warning(pop)

#define array_size(array) (sizeof(array)/sizeof(*array))

QString toString(const IVector* const vector)
{
  QString string = "( ";
  for (unsigned int i = 0; i < vector->getDim(); ++i) {
    double element;
    vector->getCoord(i, element);
    string += QString::number(element) + ", ";
  }
  string.remove(string.size() - 2, 2);
  string += " )";
  return string;
}

int main(int argc, char *argv[])
{
  ScopedILog logger("logFile");
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  //double vals[5] = { 0.1, 0.2, 0.3, 0.4, 0.5 };
  //IVector* vector = IVector::createVector(array_size(vals), vals);
  //for (unsigned int i = 0; i < vector->getDim(); ++i) {
  //  double element;
  //  vector->getCoord(i, element);
  //  LOG(QString::number(element).toStdString().c_str());
  //}

  unsigned int dim = 2;

  QVector<double> O(static_cast<int>(dim), 0.0);
  QVector<double> A(static_cast<int>(dim), 1.0);
  QVector<double> S(static_cast<int>(dim), 0.2);
  QScopedPointer<ICompact> compact(
    ICompact::createCompact(
      IVector::createVector(dim, O.data()),
      IVector::createVector(dim, A.data()),
      IVector::createVector(dim, S.data())
    )
  );

  QVector<double> B(static_cast<int>(dim), 0.2);
  QVector<double> C(static_cast<int>(dim), 0.8);
  QScopedPointer<ICompact> compactMini(
    ICompact::createCompact(
      IVector::createVector(dim, B.data()),
      IVector::createVector(dim, C.data())
    )
  );

  compact->Difference(*compactMini.data());

  ICompact::IIterator* compactIterator = compact->begin();
  do {
    QScopedPointer<IVector> vector; {
      IVector* _vector;
      compact->getByIterator(compactIterator, _vector);
      vector.reset(_vector);
    }
    LOG(toString(vector.data()).toStdString());
  } while(compactIterator->doStep() == ERR_OK);

  return 0;
}
