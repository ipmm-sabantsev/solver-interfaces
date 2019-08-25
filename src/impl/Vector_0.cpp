#include "IVector.h"

#include <logging.h>
#include <error.h>
#include <cmath>

namespace {
class Vector_0: public IVector {
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
    Vector_0(unsigned int m_size, double* m_vals);

    /*dtor*/
    ~Vector_0();

private:
    double* m_vals;
    size_t m_size;

    /*non default copyable*/
    Vector_0(const IVector& other) = delete;
    void operator=(const Vector_0& other) = delete;
};
}

int Vector_0::getId() const
{
    return IVector::INTERFACE_0;
}

Vector_0::Vector_0(unsigned int size, double *vals): m_size(size), m_vals(vals)
{}

Vector_0::~Vector_0()
{
    delete[] m_vals;
}

IVector* IVector::createVector(unsigned int size, double const* vals)
{
    double *valsNew = new(std::nothrow) double[size];
    if (!valsNew)
    {
        LOG("ERR: Not enough memory");
        return nullptr;
    }

    for(size_t i = 0; i < size; i++)
    {
        valsNew[i] = vals[i];
    }

    IVector *vect = new(std::nothrow) Vector_0(size, valsNew);
    if (!vect)
    {
        LOG("ERR: Not enough memory");
        delete[] valsNew;
        return nullptr;
    }

    return vect;
}

int Vector_0::add(IVector const* const right)
{
    if (!right)
    {
        LOG("ERR: nullptr pointer");
        return ERR_WRONG_ARG;
    }
    if (m_size != right->getDim())
    {
        LOG("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }

    int errType;
    double coord;
    double *valsTmp = new(std::nothrow) double[m_size];
    if (!valsTmp)
    {
        LOG("ERR: Not enough memory");
        return ERR_MEMORY_ALLOCATION;
    }

    for(size_t i = 0; i < m_size; i++)
    {
        errType = right->getCoord(i, coord);
        if (errType != ERR_OK)
        {
            LOG("ERR: Failed to get coordinate");
            delete[] valsTmp;
            return errType;
        }
        valsTmp[i] += coord;
    }

    delete[] m_vals;
    m_vals = valsTmp;

    return ERR_OK;
}

int Vector_0::subtract(IVector const* const right)
{
    if (!right)
    {
        LOG("ERR: nullptr pointer");
        return ERR_WRONG_ARG;
    }
    if (m_size != right->getDim())
    {
        LOG("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }
    int errType;
    double coord;
    double *valsTmp = new(std::nothrow) double[m_size];
    if (!valsTmp)
    {
        LOG("ERR: Not enough memory");
        return ERR_MEMORY_ALLOCATION;
    }

    for(size_t i = 0; i < m_size; i++)
    {
        errType = right->getCoord(i, coord);
        if (errType != ERR_OK)
        {
            LOG("ERR: Failed to get coordinate");
            delete[] valsTmp;
            return errType;
        }
        valsTmp[i] -= coord;
    }

    delete[] m_vals;
    m_vals = valsTmp;

    return ERR_OK;
}

int Vector_0::multiplyByScalar(double scalar)
{
    for(size_t i = 0; i < m_size; i++)
    {
        m_vals[i] *= scalar;
    }
    return ERR_OK;
}

int Vector_0::dotProduct(IVector const* const right, double& res) const
{
    if (!right)
    {
        LOG("ERR: nullptr pointer");
        return ERR_WRONG_ARG;
    }
    if (m_size != right->getDim())
    {
        LOG("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }
    int errType;
    double coord;
    double resTmp = 0;
    for(size_t i = 0; i < m_size; i++)
    {
        errType = right->getCoord(i, coord);
        if (errType != ERR_OK)
        {
            LOG("ERR: Failed to get coordinate");
            return errType;
        }
        resTmp += m_vals[i] * coord;
    }

    res = resTmp;

    return ERR_OK;
}

unsigned int Vector_0::getDim() const
{
    return m_size;
}

int Vector_0::norm(NormType type, double& res) const
{
    res = 0;
    switch (type)
    {
    case NORM_1:
        for (size_t i = 0; i < m_size; i++)
        {
            res += fabs(m_vals[i]);
        }
        break;
    case NORM_2:
        for (size_t i = 0; i < m_size; i++)
        {
            res += m_vals[i] * m_vals[i];
        }
        res = sqrt(res);
        break;
    case NORM_INF:
        for (size_t i = 0; i < m_size; i++)
        {
            if (res < fabs(m_vals[i]))
            {
                res = fabs(m_vals[i]);
            }
        }
        break;
    default:
        LOG("ERR: Unknown norm type");
        return ERR_NORM_NOT_DEFINED;
    }
    return ERR_OK;
}

int Vector_0::setCoord(unsigned int index, double elem)
{
    if (index > m_size - 1)
    {
        LOG("ERR: Out of range");
        return ERR_OUT_OF_RANGE;
    }
    m_vals[index] = elem;
    return ERR_OK;
}

int Vector_0::getCoord(unsigned int index, double & elem) const
{
    if (index > m_size - 1)
    {
        LOG("ERR: Out of range");
        return ERR_OUT_OF_RANGE;
    }
    elem = m_vals[index];
    return ERR_OK;
}

int Vector_0::setAllCoords(unsigned int dim, double* coords)
{
    if (dim != m_size)
    {
        LOG("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }
    if (!coords)
    {
        LOG("ERR: nullptr pointer");
        return ERR_WRONG_ARG;
    }
    for(size_t i = 0; i < m_size; i++)
    {
        m_vals[i] = coords[i];
    }
    return ERR_OK;
}

int Vector_0::getCoordsPtr(unsigned int & dim, double const*& elem) const
{
    dim = m_size;
    elem = m_vals;
    return ERR_OK;
}

IVector* Vector_0::clone() const
{
    return createVector(m_size, m_vals);
}

IVector* IVector::add(IVector const* const left, IVector const* const right)
{
    if (!left || !right)
    {
        LOG("ERR: nullptr pointer");
        return nullptr;
    }
    if (left->getDim() != right->getDim())
    {
        LOG("ERR: Dimensions mismatch");
        return nullptr;
    }
    IVector* res = left->clone();
    if (!res)
    {
        LOG("ERR: Cloning of vector failed");
        return nullptr;
    }
    if (res->add(right) != ERR_OK)
    {
        delete res;
        return nullptr;
    }
    return res;
}

IVector* IVector::subtract(IVector const* const left, IVector const* const right)
{
    if (!left || !right)
    {
        LOG("ERR: nullptr pointer");
        return nullptr;
    }
    if (left->getDim() != right->getDim())
    {
        LOG("ERR: Dimensions mismatch");
        return nullptr;
    }
    IVector* res = left->clone();
    if (!res)
    {
        LOG("ERR: Cloning of vector failed");
        return nullptr;
    }
    if (res->subtract(right) != ERR_OK)
    {
        delete res;
        return nullptr;
    }
    return res;
}

IVector* IVector::multiplyByScalar(IVector const* const left, double scalar)
{
    if (!left)
    {
        LOG("ERR: nullptr pointer");
        return nullptr;
    }
    IVector* res = left->clone();
    if (!res)
    {
        LOG("ERR: Cloning of vector failed");
        return nullptr;
    }
    if (res->multiplyByScalar(scalar) != ERR_OK)
    {
        delete res;
        return nullptr;
    }
    return res;
}

int Vector_0::gt(IVector const* const right, NormType type, bool& result) const
{
    if (!right)
    {
        LOG("ERR: nullptr pointer");
        return ERR_WRONG_ARG;
    }
    if (m_size != right->getDim())
    {
        LOG("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }

    double normResL, normResR;
    int errType;

    errType = norm(type, normResL);
    if (errType != ERR_OK)
    {
        LOG("ERR: Norm calculating failed");
        return errType;
    }
    errType = right->norm(type, normResR);
    if (errType != ERR_OK)
    {
        LOG("ERR: Norm calculating failed");
        return errType;
    }
    result = normResL > normResR;
    return ERR_OK;
}
int Vector_0::lt(IVector const* const right, NormType type, bool& result) const
{
    if (!right)
    {
        LOG("ERR: nullptr pointer");
        return ERR_WRONG_ARG;
    }
    if (m_size != right->getDim())
    {
        LOG("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }

    double normResL, normResR;
    int errType;

    errType = norm(type, normResL);
    if (errType != ERR_OK)
    {
        LOG("ERR: Norm calculating failed");
        return errType;
    }
    errType = right->norm(type, normResR);
    if (errType != ERR_OK)
    {
        LOG("ERR: Norm calculating failed");
        return errType;
    }
    result = normResL < normResR;
    return ERR_OK;
}
int Vector_0::eq(IVector const* const right, NormType type, bool& result, double precision) const
{
    if (!right)
    {
        LOG("ERR: nullptr pointer");
        return ERR_WRONG_ARG;
    }
    if (m_size != right->getDim())
    {
        LOG("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }

    IVector *tmp = IVector::subtract(this, right);
    if (!tmp)
    {
        LOG("ERR: Subtraction failed");
        return ERR_ANY_OTHER;
    }
    int errType;
    double normRes;
    errType = tmp->norm(type, normRes);
    if (errType != ERR_OK)
    {
        LOG("ERR: Norm calculating failed");
        delete tmp;
        return errType;
    }
    result = normRes < precision;
    return ERR_OK;
}
