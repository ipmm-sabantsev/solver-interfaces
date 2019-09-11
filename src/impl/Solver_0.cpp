/// \brief  ISolver implementation
/// \author Nikita Nazarov
/// \date   10.09.2019

#include <new>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QVector>
#include <cmath>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QScopedPointer>

#pragma warning(push)
#pragma warning(disable: 4100)
  #include "IBrocker.h"
  #include "ISolver.h"
  #include "IProblem.h"
  #include "ICompact.h"
  #include <logging.h>
#pragma warning(pop)

namespace {
  class Solver_0 : public ISolver {
  public:
    int getId() const override;

    int setParams(IVector const* params) override;
    int setParams(QString& str) override;
    int setProblem(IProblem* problem) override;
    int solve() override;
    int getSolution(IVector*& vec) const override;
    int getQml(QUrl& qml) const override;

    Solver_0();
    ~Solver_0() override;

  private:
    int doSolveByArgs();
    int doSolveByParams();
    int doStep(double alpha, double lam, IVector* grad, bool byArgs);
    int getGrad(IVector*& res, bool byArgs);

    QScopedPointer<IVector>  m_args;
    QScopedPointer<IVector>  m_params;
    QScopedPointer<IVector>  m_prev;
    QScopedPointer<IVector>  m_curr;
    QScopedPointer<ICompact> m_compact;
    IProblem* m_problem;

    bool solveByArgs;
    double eps;
  };

  class Brocker_1 : public IBrocker {
  public:
    int getId() const override;

    bool canCastTo(Type type) const override;
    void* getInterfaceImpl(Type type) const override;

    int release() override;

    Brocker_1(Solver_0* solver);
    ~Brocker_1() override;

  private:
    Solver_0* m_solver;
  };
}

Solver_0::Solver_0() :
  m_args(nullptr),    m_params(nullptr),
  m_prev(nullptr),    m_curr(nullptr),
  m_compact(nullptr), m_problem(nullptr)
{

}

Solver_0::~Solver_0()
{
}

int Solver_0::getId() const
{
  return ISolver::INTERFACE_0;
}

int Solver_0::setProblem(IProblem* problem)
{
  if (!problem)
    LOG_RET("Invalid problem", ERR_WRONG_PROBLEM);

  size_t dim;
  if (m_params) {
    if (problem->getParamsDim(dim) != ERR_OK)
      LOG_RET("Can't get parameters dimension", ERR_ANY_OTHER);

    if (m_params->getDim() != dim)
      LOG_RET("Mismatch of parameters dimensions", ERR_WRONG_PROBLEM);
  }

  if (m_args) {
    if (problem->getArgsDim(dim) != ERR_OK)
      LOG_RET("Can't get arguments dimension", ERR_ANY_OTHER);

    if (m_args->getDim() != dim)
      LOG_RET("Mismatch of arguments dimensions", ERR_WRONG_PROBLEM);
  }

  m_problem = problem;
  return ERR_OK;
}

int Solver_0::setParams(IVector const* solverParams)
{
  if (!solverParams)
    LOG_RET("Invalid parameters", ERR_WRONG_ARG);

  size_t dim;
  const double* coords;
  if (solverParams->getCoordsPtr(dim, coords) != ERR_OK)
    LOG_RET("Can't get coordinates", ERR_WRONG_ARG);

  if (dim < 4)
    LOG_RET("Parameters dimension is less than 4", ERR_WRONG_ARG);

  size_t paramsDim;
  size_t argsDim;
  if (m_problem) {
    if (m_problem->getParamsDim(paramsDim) != ERR_OK)
      LOG_RET("Can't get dimension of parameters", ERR_ANY_OTHER);

    if (m_problem->getArgsDim(argsDim) != ERR_OK)
      LOG_RET("Can't get dimension of arguments", ERR_ANY_OTHER);

    if (round(coords[0]) != argsDim)
      LOG_RET("Mismatch of arguments dimensions", ERR_WRONG_ARG);

    if (round(coords[1]) != paramsDim)
      LOG_RET("Mismatch of parameters dimensions", ERR_WRONG_ARG);

  } else {
    argsDim = static_cast<size_t>(round(coords[0]));
    paramsDim = static_cast<size_t>(round(coords[1]));
  }

  double eps = coords[2];
  if (eps <= 0)
    LOG_RET("Negative epsilon", ERR_WRONG_ARG);

  size_t solveByArgsFlag = static_cast<size_t>(round(coords[3]));
  if (solveByArgsFlag != 0 && solveByArgsFlag != 1)
    LOG_RET("Invalid solve by arguments flag", ERR_WRONG_ARG);

  solveByArgs = static_cast<bool>(solveByArgsFlag);

  size_t tmp = argsDim + paramsDim + 4;
  if ((solveByArgs && dim != tmp + 2 * argsDim) ||
      (!solveByArgs && dim != tmp + 2 * paramsDim))
    LOG_RET("Dimension mismatch", ERR_WRONG_PROBLEM);
  dim = solveByArgs ? argsDim : paramsDim;

  IVector* args = IVector::createVector(argsDim, (coords + 4));
  if (!args)
    LOG_RET("Not enough memory", ERR_MEMORY_ALLOCATION);

  IVector* params = IVector::createVector(paramsDim, (coords + 4 + argsDim));
  if (!params)
    LOG_RET("Not enough memory", ERR_MEMORY_ALLOCATION);

  QScopedPointer<IVector> begin(
        IVector::createVector(dim, (coords + tmp)));
  if (!begin)
    LOG_RET("Not enough memory", ERR_MEMORY_ALLOCATION);

  QScopedPointer<IVector> end(
        IVector::createVector(dim, (coords + tmp + dim)));
  if (!end)
    LOG_RET("Not enough memory", ERR_MEMORY_ALLOCATION);

  ICompact* compact = ICompact::createCompact(begin.data(), end.data());
  if (!compact)
    LOG_RET("Not enough memory", ERR_MEMORY_ALLOCATION);

  m_args.reset(args);
  m_params.reset(params);
  m_compact.reset(compact);
  //eps = epsilon;

  return ERR_OK;
}

int Solver_0::setParams(QString& str)
{
  QStringList strings = str.split(" ");
  QStringList params;
  for (QString it : strings) {
    QStringList splitStr = it.split(":");
    if (splitStr.count() != 2)
      LOG_RET("Wrong number of params in string", ERR_WRONG_ARG);

    params.append(it.split(":").at(1));
  }

  QVector<double> paramsAsArray(params.size());
  for (int i = 0; i < params.size(); i++) {
    bool result;
    paramsAsArray[i] = params.at(i).toDouble(&result);
    if (!result)
      LOG_RET("Can't parse params", ERR_ANY_OTHER);
  }

  QScopedPointer<IVector> solverParams(
        IVector::createVector(static_cast<unsigned int>(params.size()), paramsAsArray.data()));
  if (!solverParams)
    LOG_RET("Not enough memory", ERR_MEMORY_ALLOCATION);

  return setParams(solverParams.data());
}

int Solver_0::doStep(double alpha, double lam, IVector* grad, bool byArgs)
{
  while (true) {
    QScopedPointer<IVector> tmpMS(
          IVector::multiplyByScalar(grad, alpha));
    if (!tmpMS)
      LOG_RET("Can't multiply by scalar", ERR_ANY_OTHER);

    QScopedPointer<IVector> tmpS(
          IVector::subtract(m_curr.data(), tmpMS.data()));
    if (!tmpS)
      LOG_RET("Can't subtract", ERR_ANY_OTHER);

    QScopedPointer<IVector> prS; {
      IVector* prS_data;
      if (m_compact->getNearestNeighbor(tmpS.data(), prS_data) != ERR_OK)
        LOG_RET("Can't get nearest neighbor", ERR_ANY_OTHER);
    }

    double resS, resC;
    if (byArgs) {
      if (m_problem->goalFunctionByArgs(prS.data(), resS) != ERR_OK)
        LOG_RET("Can't get goal function value by arguments", ERR_ANY_OTHER);

      if (m_problem->goalFunctionByArgs(m_curr.data(), resC) != ERR_OK)
        LOG_RET("Can't get goal function value by arguments", ERR_ANY_OTHER);

    } else {
      if (m_problem->goalFunctionByParams(prS.data(), resS) != ERR_OK)
        LOG_RET("Can't get goal function value by parameters", ERR_ANY_OTHER);

      if (m_problem->goalFunctionByParams(m_curr.data(), resC) != ERR_OK)
        LOG_RET("Can't get goal function value by parameters", ERR_ANY_OTHER);
    }

    if (resS <= resC) {
      m_prev.swap(m_curr);
      m_curr.reset(prS.data());
      break;
    } else {
      alpha *= lam;
    }
  }

  return ERR_OK;
}

int Solver_0::getGrad(IVector*& res, bool byArgs)
{
  size_t argsDim;
  if (m_problem->getArgsDim(argsDim) != ERR_OK)
    LOG_RET("Can't get argsDim", ERR_ANY_OTHER);

  QVector<double> gradArray(static_cast<int>(argsDim));
  for (size_t i = 0; i < argsDim; i++) {
    if (byArgs) {
      if (m_problem->derivativeGoalFunctionByArgs(1, i, IProblem::BY_ARGS,
          gradArray[static_cast<int>(i)], m_curr.data()) != ERR_OK)
        LOG_RET("Can't get derivative of goal function by arguments", ERR_ANY_OTHER);

    } else {
      if (m_problem->derivativeGoalFunctionByParams(1, i, IProblem::BY_PARAMS,
          gradArray[static_cast<int>(i)], m_curr.data()) != ERR_OK)
        LOG_RET("Can't get goal function derivative by parameters", ERR_ANY_OTHER);

    }
  }

  IVector* gradVec = IVector::createVector(argsDim, gradArray.data());
  if (!gradVec)
    LOG_RET("Not enough memory", ERR_MEMORY_ALLOCATION);

  res = gradVec;
  return ERR_OK;
}

int Solver_0::doSolveByArgs()
{
  if (m_problem->setParams(m_params.data()) != ERR_OK)
    LOG_RET("Can't set params to problem", ERR_ANY_OTHER);

  m_curr.reset(m_args->clone());
  if (!m_curr)
    LOG_RET("Not enough memory", ERR_MEMORY_ALLOCATION);

  while (true) {
    IVector* gradVec;
    int res;
    if ((res = getGrad(gradVec, true)) != ERR_OK) {
      return res;
    }

    if ((res = doStep(1, 0.8, gradVec, true)) != ERR_OK) {
      return res;
    }
    delete gradVec;

    bool stop = false;
    if (m_curr->eq(m_prev.data(), IVector::NORM_INF, stop, eps) != ERR_OK)
      LOG_RET("Can't compare vectors", ERR_ANY_OTHER);

    if (stop) {
      break;
    }
  }

  return ERR_OK;
}

int Solver_0::doSolveByParams()
{
  if (m_problem->setArgs(m_args.data()) != ERR_OK)
    LOG_RET("Can't set arguments to problem", ERR_ANY_OTHER);

  m_curr.reset(m_params->clone());

  if (!m_curr)
    LOG_RET("Not enough memory", ERR_MEMORY_ALLOCATION);

  while (true) {
    IVector* gradVec;
    int res;
    if ((res = getGrad(gradVec, false)) != ERR_OK) {
      return res;
    }

    if ((res = doStep(1, 0.8, gradVec, false)) != ERR_OK) {
      return res;
    }
    delete gradVec;

    bool stop = false;
    if (m_curr->eq(m_prev.data(), IVector::NORM_INF, stop, eps) != ERR_OK)
      LOG_RET("Can't compare vectors", ERR_ANY_OTHER)

    if (stop) {
      break;
    }
  }

  return ERR_OK;
}

int Solver_0::solve()
{
  if (!m_args || !m_params)
    LOG_RET("Invalid initial approximation", ERR_WRONG_ARG);

  if (!m_compact)
    LOG_RET("Invalid compact", ERR_WRONG_ARG);

  if (!m_problem)
    LOG_RET("Invalid problem", ERR_WRONG_ARG);

  return solveByArgs ? doSolveByArgs() : doSolveByParams();
}

int Solver_0::getSolution(IVector*& vec) const
{
  if (!m_curr)
    LOG_RET("Solution is invalid", ERR_ANY_OTHER);

  vec = m_curr->clone();
  return ERR_OK;
}

int Brocker_1::getId() const
{
  return IBrocker::INTERFACE_0;
}

int Solver_0::getQml(QUrl& qml) const
{
  QFileInfo dll(qml.path());
  QString file = dll.absolutePath() + "/" + "Solver_0.qml";

  if (!QFile::exists(file)) {
    return ERR_ANY_OTHER;
  }

  qml = QUrl::fromLocalFile(file);
  return ERR_OK;
}

bool Brocker_1::canCastTo(Type type) const
{
  return type == IBrocker::SOLVER;
}

void* Brocker_1::getInterfaceImpl(Type type) const
{
  return type == IBrocker::SOLVER ? m_solver : nullptr;
}

int Brocker_1::release()
{
  delete this;

  return ERR_OK;
}

Brocker_1::Brocker_1(Solver_0* solver) :
  m_solver(solver)
{

}

Brocker_1::~Brocker_1()
{
  delete m_solver;
}

extern "C" {
  SHARED_EXPORT void* getBrocker() {
    QScopedPointer<Solver_0> solver(new (std::nothrow) Solver_0());

    if (!solver)
      LOG_RET("Not enough memory", nullptr);

    Brocker_1* brocker = new (std::nothrow) Brocker_1(solver.data());

    if (!brocker)
      LOG_RET("Not enough memory", nullptr);

    return brocker;
  }
}
