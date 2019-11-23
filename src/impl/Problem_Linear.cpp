#include "logging.h"
#include "IBrocker.h"
#include "IProblem.h"
#include "IVector.h"
//#include "problem.h"

namespace {

class Problem_0 : public IProblem {

public:

    int getId() const;

    int goalFunction(IVector const* args, IVector const* params, double& res) const;
    int goalFunctionByArgs(IVector const*  args, double& res) const;
    int goalFunctionByParams(IVector const*  params, double& res) const;
    int getArgsDim(size_t& dim) const;
    int getParamsDim(size_t& dim) const;

    int setParams(IVector const* params);
    int setArgs(IVector const* args);

    int derivativeGoalFunction(size_t order, size_t idx, DerivedType dr, double& value, IVector const* args, IVector const* params) const;
    int derivativeGoalFunctionByArgs(size_t order, size_t idx, DerivedType dr, double& value, IVector const* args) const;
    int derivativeGoalFunctionByParams(size_t order, size_t idx, DerivedType dr, double& value, IVector const* params) const;

    /*ctor*/
    Problem_0();

    /*dtor*/
    ~Problem_0();

private:

    size_t m_dimArgs, m_dimParams;
    IVector *m_args, *m_params;

    int GoalFunctionCalculate_0(unsigned int dimArgs, unsigned int dimParams, double const* args, double const* params, double &res) const;
    int derivativeGoalFunctionCalculate_Order1_0(size_t idx, DerivedType dr, double &value, const IVector *args, const IVector *params) const;
    int derivativeGoalFunctionCalculate_Order2_0(size_t idx, DerivedType dr, double &value, const IVector *args, const IVector *params) const;

};

class Brocker_0 : public IBrocker {

public:

    int getId() const;

    bool canCastTo(Type type) const;
    void* getInterfaceImpl(Type type) const;

    int release();

    /*ctor*/
    Brocker_0(Problem_0 *problem);

    /*dtor*/
    ~Brocker_0();

private:

    Problem_0 *m_problem;

};

}

int Problem_0::getId() const {
    return IProblem::INTERFACE_0;
}

int Problem_0::GoalFunctionCalculate_0(unsigned int dimArgs, unsigned int dimParams, double const*args, double const*params, double &res ) const
{
    if (dimArgs != dimParams)
    {
        LOG("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }
    res = 0;
    for(size_t i = 0; i < dimArgs; i++)
    {
        res += args[i] * params[i];
    }
    return ERR_OK;
}

int Problem_0::getArgsDim(size_t& dim) const
{
    dim = m_dimArgs;
    return ERR_OK;
}
int Problem_0::getParamsDim(size_t& dim) const
{
    dim = m_dimParams;
    return ERR_OK;
}

int Problem_0::goalFunction(const IVector *args, const IVector *params, double &res) const
{
    if (!args)
    {
        LOG("ERR: Invalid args");
        return ERR_WRONG_ARG;
    }
    if (!params)
    {
        LOG("ERR: Invalid params");
        return ERR_WRONG_ARG;
    }
    /*if (args->getDim() != params->getDim())
    {
        LOG("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }*/
    size_t dimArgs, dimParams;
    double const *argsClone = 0, *paramsClone = 0;
    int errType;
    if ((errType = args->getCoordsPtr(dimArgs, argsClone)) != ERR_OK)
    {
        LOG("ERR: Failed to get args coords");
        return errType;
    }
    if ((errType = params->getCoordsPtr(dimParams, paramsClone)) != ERR_OK)
    {
        LOG("ERR: Failed to get params coords");
        return errType;
    }
    errType = GoalFunctionCalculate_0(dimArgs, dimParams, argsClone, paramsClone, res);
    if (errType != ERR_OK)
    {
        LOG("ERR: Goal function failed to calculate");
        return errType;
    }
    return ERR_OK;
}

int Problem_0::goalFunctionByArgs(const IVector *args, double &res) const
{
    int errType = goalFunction(args, m_params, res);
    if (errType != ERR_OK)
    {
        LOG("ERR: Failed to calculate goal function");
        return errType;
    }
    return ERR_OK;
}

int Problem_0::goalFunctionByParams(const IVector *params, double &res) const
{
    int errType = goalFunction(m_args, params, res);
    if (errType != ERR_OK)
    {
        LOG("ERR: Failed to calculate goal function");
        return errType;
    }
    return ERR_OK;
}

int Problem_0::setParams(const IVector *params)
{
    if (!params)
    {
        LOG("ERR: Invalid argument");
        return ERR_WRONG_ARG;
    }
    delete m_params;
    m_params = params->clone();
    if (!m_params)
    {
        LOG("ERR: Failed to clone params");
        return ERR_MEMORY_ALLOCATION;
    }
    m_dimParams = params->getDim();
    return ERR_OK;
}

int Problem_0::setArgs(const IVector *args)
{
    if (!args)
    {
        LOG("ERR: Invalid argument");
        return ERR_WRONG_ARG;
    }
    delete m_args;
    m_args = args->clone();
    if (!m_args)
    {
        LOG("ERR: Failed to clone args");
        return ERR_MEMORY_ALLOCATION;
    }
    m_dimArgs = args->getDim();
    return ERR_OK;
}

int Problem_0::derivativeGoalFunctionCalculate_Order1_0(size_t idx, IProblem::DerivedType dr, double &value, const IVector *args, const IVector *params) const
{
    if (!args)
    {
        LOG("ERR: Invalid argument args");
        return ERR_WRONG_ARG;
    }
    if (!params)
    {
        LOG("ERR: Invalid argument params");
        return ERR_WRONG_ARG;
    }
    if (args->getDim() != params->getDim())
    {
        LOG("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }
    switch (dr)
    {
    case BY_ARGS:
        if (idx > args->getDim() - 1)
        {
            LOG("ERR: Index out of range");
            return ERR_OUT_OF_RANGE;
        }
        int errType;
        if ((errType = params->getCoord(idx, value)) != ERR_OK)
        {
            LOG("ERR: Failed to get coord");
            return errType;
        }
        break;
    case BY_PARAMS:
        if (idx > params->getDim() - 1)
        {
            LOG("ERR: Index out of range");
            return ERR_OUT_OF_RANGE;
        }

        if ((errType = args->getCoord(idx, value)) != ERR_OK)
        {
            LOG("ERR: Failed to get coord");
            return errType;
        }
        break;
    default:
        LOG("ERR: Invalid derivated type");
        return ERR_WRONG_ARG;
    }

    return ERR_OK;
}

int Problem_0::derivativeGoalFunctionCalculate_Order2_0(size_t idx, IProblem::DerivedType dr, double &value, const IVector *args, const IVector *params) const
{
    value = 0;
    return ERR_OK;
}

int Problem_0::derivativeGoalFunction(size_t order, size_t idx, IProblem::DerivedType dr, double &value, const IVector *args, const IVector *params) const
{
    if (!args)
    {
        LOG("ERR: Invalid argument args");
        return ERR_WRONG_ARG;
    }
    if (!params)
    {
        LOG("ERR: Invalid argument params");
        return ERR_WRONG_ARG;
    }
    int errType;
    if (order == 0)
    {
        if ((errType = goalFunction(args, params, value)) != ERR_OK)
        {
            LOG("ERR: Failed to calculate goal function");
            return errType;
        }
    }
    if (order == 1)
    {
        errType = derivativeGoalFunctionCalculate_Order1_0(idx, dr, value, args, params);
        if (errType != ERR_OK)
        {
            LOG("ERR: Failed to calculate first order derivative");
            return errType;
        }
    }
    if (order == 2)
    {
        errType = derivativeGoalFunctionCalculate_Order2_0(idx, dr, value, args, params);
        if (errType != ERR_OK)
        {
            LOG("ERR: Failed to calculate first order derivative");
            return errType;
        }
    }
    if (order >= 3)
    {
        value = 0;
    }
    return ERR_OK;
}

int Problem_0::derivativeGoalFunctionByArgs(size_t order, size_t idx, IProblem::DerivedType dr, double &value, const IVector *args) const
{
    int errType = derivativeGoalFunction(order, idx, dr, value, args, m_params);
    if (errType != ERR_OK)
    {
        LOG("ERR: Failed to calculate derivative");
        return errType;
    }
    return ERR_OK;
}

int Problem_0::derivativeGoalFunctionByParams(size_t order, size_t idx, IProblem::DerivedType dr, double &value, const IVector *params) const
{
    int errType = derivativeGoalFunction(order, idx, dr, value, m_args, params);
    if (errType != ERR_OK)
    {
        LOG("ERR: Failed to calculate derivative");
        return errType;
    }
    return ERR_OK;
}

Problem_0::Problem_0(): m_dimArgs(2), m_dimParams(2), m_args(nullptr), m_params(nullptr)
{}

Problem_0::~Problem_0() {
    delete m_args;
    delete m_params;
}

int Brocker_0::getId() const {
    return IBrocker::INTERFACE_0;
}

bool Brocker_0::canCastTo(IBrocker::Type type) const
{
    if (type == IBrocker::PROBLEM)
    {
        return true;
    }
    return false;
}

void * Brocker_0::getInterfaceImpl(IBrocker::Type type) const
{
    if (type == IBrocker::PROBLEM)
    {
        return m_problem;
    }
    return nullptr;
}

int Brocker_0::release()
{
    delete m_problem;
    return ERR_OK;
}

Brocker_0::Brocker_0(Problem_0 *problem): m_problem(problem)
{}

Brocker_0::~Brocker_0()
{
    delete m_problem;
}

extern "C" {
    SHARED_EXPORT void* getBrocker() {
        Problem_0 *problem = new (std::nothrow) Problem_0();

        if (!problem) {
            LOG("ERR: Not enough memory");
            return nullptr;
        }

        Brocker_0 *brocker = new (std::nothrow) Brocker_0(problem);

        if (!brocker) {
            LOG("ERR: Not enough memory");
            delete problem;
            return nullptr;
        }

        return brocker;
    }
}
