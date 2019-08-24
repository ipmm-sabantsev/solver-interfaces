#include <cmath>
#include "IVector.h"
#include "ILog.h"
#include "error.h"

namespace {
class IVector_Impl: IVector {
public:

    int getId() const;

    /*operations*/
    int add(IVector const* const right);
    int subtract(IVector const* const right);
    int multiplyByScalar(double scalar);
    int dotProduct(IVector const* const right, double& res) const;

    /*utils*/
    unsigned int getDim() const;
    int norm(NormType type, double& res) const;
    int setCoord(unsigned int index, double elem);
    int getCoord(unsigned int index, double & elem) const;
    int setAllCoords(unsigned int dim, double* coords);
    int getCoordsPtr(unsigned int & dim, double const*& elem) const;
    IVector* clone() const;

    /*comparators*/
    int gt(IVector const* const right, NormType type, bool& result) const;
    int lt(IVector const* const right, NormType type, bool& result) const;
    int eq(IVector const* const right, NormType type, bool& result, double precision) const;

    /*ctor*/
    IVector_Impl(unsigned int size, double* vals);

    /*dtor*/
    ~IVector_Impl();

private:
    double* vals;
    size_t size;

    /*non default copyable*/
    IVector_Impl(const IVector& other) = delete;
    void operator=(const IVector_Impl& other) = delete;
};
}

int IVector_Impl::getId() const
{
    return IVector::INTERFACE_0;
}

IVector_Impl::IVector_Impl(unsigned int size, double *vals): size(size), vals(vals)
{}

IVector_Impl::~IVector_Impl()
{
    delete[] vals;
}

IVector* IVector::createVector(unsigned int size, double const* vals)
{
    double *valsNew = new(std::nothrow) double[size];
    if (!valsNew)
    {
        ILog::report("ERR: Not enough memory");
        return NULL;
    }

    for(size_t i = 0; i < size; i++)
    {
        valsNew[i] = vals[i];
    }

    IVector *vect = new(std::nothrow) IVector_Impl(size, valsNew);
    if (!vect)
    {
        ILog::report("ERR: Not enough memory");
        delete[] valsNew;
        return NULL;
    }

    return vect;
}

int IVector_Impl::add(IVector const* const right)
{
    if (!right)
    {
        ILog::report("ERR: Null pointer");
        return ERR_WRONG_ARG;
    }
    if (size != right->getDim())
    {
        ILog::report("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }

    int errType;
    double coord;
    double *valsTmp = new(std::nothrow) double[size];
    if (!valsTmp)
    {
        ILog::report("ERR: Not enough memory");
        return ERR_MEMORY_ALLOCATION;
    }

    for(size_t i = 0; i < size; i++)
    {
        errType = right->getCoord(i, coord);
        if (errType != ERR_OK)
        {
            ILog::report("ERR: Failed to get coordinate");
            delete[] valsTmp;
            return errType;
        }
        valsTmp[i] += coord;
    }

    delete[] vals;
    vals = valsTmp;

    return ERR_OK;
}

int IVector_Impl::subtract(IVector const* const right)
{
    if (!right)
    {
        ILog::report("ERR: Null pointer");
        return ERR_WRONG_ARG;
    }
    if (size != right->getDim())
    {
        ILog::report("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }
    int errType;
    double coord;
    double *valsTmp = new(std::nothrow) double[size];
    if (!valsTmp)
    {
        ILog::report("ERR: Not enough memory");
        return ERR_MEMORY_ALLOCATION;
    }

    for(size_t i = 0; i < size; i++)
    {
        errType = right->getCoord(i, coord);
        if (errType != ERR_OK)
        {
            ILog::report("ERR: Failed to get coordinate");
            delete[] valsTmp;
            return errType;
        }
        valsTmp[i] -= coord;
    }

    delete[] vals;
    vals = valsTmp;

    return ERR_OK;
}

int IVector_Impl::multiplyByScalar(double scalar)
{
    for(size_t i = 0; i < size; i++)
    {
        valsTmp[i] *= scalar;
    }
    return ERR_OK;
}

int IVector_Impl::dotProduct(IVector const* const right, double& res) const
{
    if (!right)
    {
        ILog::report("ERR: Null pointer");
        return ERR_WRONG_ARG;
    }
    if (size != right->getDim())
    {
        ILog::report("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }
    int errType;
    double coord;
    double resTmp = 0;
    for(size_t i = 0; i < size; i++)
    {
        errType = right->getCoord(i, coord);
        if (errType != ERR_OK)
        {
            ILog::report("ERR: Failed to get coordinate");
            return errType;
        }
        resTmp += vals[i] * coord;
    }

    res = resTmp;

    return ERR_OK;
}

unsigned int IVector_Impl::getDim() const
{
    return size;
}

int IVector_Impl::norm(NormType type, double& res) const
{
    res = 0;
    switch (type)
    {
    case NORM_1:
        for (size_t i = 0; i < size; i++)
        {
            res += fabs(vals[i]);
        }
        break;
    case NORM_2:
        for (size_t i = 0; i < size; i++)
        {
            res += vals[i] * vals[i];
        }
        res = sqrt(res);
        break;
    case NORM_INF:
        for (size_t i = 0; i < size; i++)
        {
            if (res < fabs(vals[i]))
            {
                res = fabs(vals[i]);
            }
        }
        break;
    default:
        ILog::report("ERR: Unknown norm type");
        return ERR_NORM_NOT_DEFINED;
    }
    return ERR_OK;
}

int IVector_Impl::setCoord(unsigned int index, double elem)
{
    if (index > size - 1)
    {
        ILog::report("ERR: Out of range");
        return ERR_OUT_OF_RANGE;
    }
    vals[index] = elem;
    return ERR_OK;
}

int IVector_Impl::getCoord(unsigned int index, double & elem) const
{
    if (index > size - 1)
    {
        ILog::report("ERR: Out of range");
        return ERR_OUT_OF_RANGE;
    }
    elem = vals[index];
    return ERR_OK;
}

int IVector_Impl::setAllCoords(unsigned int dim, double* coords)
{
    if (dim != size)
    {
        ILog::report("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }
    if (!coords)
    {
        ILog::report("ERR: Null pointer");
        return ERR_WRONG_ARG;
    }
    for(size_t i = 0; i < size; i++)
    {
        vals[i] = coords[i];
    }
    return ERR_OK;
}

int IVector_Impl::getCoordsPtr(unsigned int & dim, double const*& elem) const
{
    dim = size;
    elem = vals;
    return ERR_OK;
}

IVector* IVector_Impl::clone() const
{
    return createVector(size, vals);
}

static IVector* IVector::add(IVector const* const left, IVector const* const right)
{
    if (!left || !right)
    {
        ILog::report("ERR: Null pointer");
        return NULL;
    }
    if (left->getDim() != right->getDim())
    {
        ILog::report("ERR: Dimensions mismatch");
        return NULL;
    }
    IVector res = left->clone();
    if (!res)
    {
        ILog::report("ERR: Cloning of vector failed");
        return NULL;
    }
    if (res.add(right) != ERR_OK)
    {
        delete res;
        return NULL;
    }
    return res;
}

static IVector* IVector::subtract(IVector const* const left, IVector const* const right)
{
    if (!left || !right)
    {
        ILog::report("ERR: Null pointer");
        return NULL;
    }
    if (left->getDim() != right->getDim())
    {
        ILog::report("ERR: Dimensions mismatch");
        return NULL;
    }
    IVector res = left->clone();
    if (!res)
    {
        ILog::report("ERR: Cloning of vector failed");
        return NULL;
    }
    if (res.subtract(right) != ERR_OK)
    {
        delete res;
        return NULL;
    }
    return res;
}

static IVector* IVector::multiplyByScalar(IVector const* const left, double scalar)
{
    if (!left)
    {
        ILog::report("ERR: Null pointer");
        return NULL;
    }
    IVector res = left->clone();
    if (!res)
    {
        ILog::report("ERR: Cloning of vector failed");
        return NULL;
    }
    if (res.multiplyByScalar(scalar) != ERR_OK)
    {
        delete res;
        return NULL;
    }
    return res;
}

int IVector_Impl::gt(IVector const* const right, NormType type, bool& result) const
{
    if (!right)
    {
        ILog::report("ERR: Null pointer");
        return ERR_WRONG_ARG;
    }
    if (size != right->getDim())
    {
        ILog::report("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }

    double normResL, normResR;
    int errType;

    errType = norm(type, normResL);
    if (errType != ERR_OK)
    {
        ILog::report("ERR: Norm calculating failed");
        return errType;
    }
    errType = right->norm(type, normResR);
    if (errType != ERR_OK)
    {
        ILog::report("ERR: Norm calculating failed");
        return errType;
    }
    result = normResL > normResR;
    return ERR_OK;
}
int IVector_Impl::lt(IVector const* const right, NormType type, bool& result) const
{
    if (!right)
    {
        ILog::report("ERR: Null pointer");
        return ERR_WRONG_ARG;
    }
    if (size != right->getDim())
    {
        ILog::report("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }

    double normResL, normResR;
    int errType;

    errType = norm(type, normResL);
    if (errType != ERR_OK)
    {
        ILog::report("ERR: Norm calculating failed");
        return errType;
    }
    errType = right->norm(type, normResR);
    if (errType != ERR_OK)
    {
        ILog::report("ERR: Norm calculating failed");
        return errType;
    }
    result = normResL < normResR;
    return ERR_OK;
}
int IVector_Impl::eq(IVector const* const right, NormType type, bool& result, double precision) const
{
    if (!right)
    {
        ILog::report("ERR: Null pointer");
        return ERR_WRONG_ARG;
    }
    if (size != right->getDim())
    {
        ILog::report("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }

    IVector *tmp = IVector::subtract(this, right);
    if (!tmp)
    {
        ILog::report("ERR: Subtraction failed");
        return ERR_ANY_OTHER;
    }
    int errType;
    double normRes;
    errType = tmp->norm(type, normRes);
    if (errType != ERR_OK)
    {
        ILog::report("ERR: Norm calculating failed");
        delete tmp;
        return errType;
    }
    result = normRes < precision;
    return ERR_OK;
}
