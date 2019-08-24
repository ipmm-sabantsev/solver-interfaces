#include <QVector>
#include <cmath>
#include "ISet.h"
#include "error.h"
#include "logging.h"

const double EPS = 1e-8;

namespace {
class Set_0 : public ISet
{
public:
    int getId() const;
    int put(IVector const* const element);
    int get(unsigned int index, IVector*& p_element) const;
    int remove(unsigned int index);
    int contains(IVector const* const p_element, bool& result) const;
    unsigned int getSize() const;
    int clear();

    IIterator* begin();
    IIterator* end();

    int deleteIterator(IIterator * pIter);
    int getByIterator(IIterator const* pIter, IVector*& p_element) const;

    class Iterator_0 : public ISet::IIterator
    {
    public:
        int next();
        int prev();
        bool isEnd() const;
        bool isBegin() const;

        ISet const* const m_set;
        unsigned int m_pos;

        Iterator_0(ISet const* const set, int pos);
    };

    /*ctor*/
    Set_0(uint dim);
    /*dtor*/
    ~Set_0();


private:
    int findIterator(IIterator const * pIter) const;
    QVector<IVector*> m_ptr_points;
    QVector<Iterator_0*> m_ptr_iterators;
    unsigned int m_dim;
};
} //end anonymous namespace

int Set_0::getId() const
{
    return ISet::INTERFACE_0;
}

unsigned int Set_0::getSize() const {
    return m_ptr_points.size();
}

Set_0::Set_0(uint dim)
{
    m_dim = dim;
}

Set_0::~Set_0()
{
    for (int i = 0; i < m_ptr_points.size(); i++)
    {
        delete m_ptr_points[i];
    }
    for (int i = 0; i < m_ptr_iterators.size(); i++)
    {
        delete m_ptr_iterators[i];
    }
}

ISet* ISet::createSet(unsigned int dim)
{
    if (dim == 0)
    {
        LOG("ERR: Incorrect dimension");
        return nullptr;
    }

    ISet* set = new(std::nothrow) Set_0(dim);
    if (!set)
    {
        LOG("ERR: Not enough memory");
        return nullptr;
    }
    return set;
}


int Set_0::put(const IVector *const p_element)
{
    if (!p_element)
    {
            LOG("ERR: Incorrect argument");
            return ERR_WRONG_ARG;
        }
        if  (m_dim != p_element->getDim())
        {
            LOG("ERR: Dimensions mismatch");
            return ERR_DIMENSIONS_MISMATCH;
        }

        IVector* tmp = p_element->clone();
        if (!tmp)
        {
            LOG("ERR: Not enough memory");
            return ERR_MEMORY_ALLOCATION;
        }

        m_ptr_points.append(tmp);
        return ERR_OK;
}

int Set_0::get(unsigned int index, IVector*& p_element) const
{
    if  (index >= (uint)m_ptr_points.size())
    {
        LOG("ERR: Out of range");
        return ERR_OUT_OF_RANGE;
    }

    p_element = m_ptr_points[index]->clone();
    if (!p_element)
    {
        LOG("ERR: Not enough memory");
        return ERR_MEMORY_ALLOCATION;
    }

    return ERR_OK;
}

int Set_0::remove(unsigned int index)
{
    if  (index >= (uint)m_ptr_points.size())
    {
        LOG("ERR: Out of range");
        return ERR_OUT_OF_RANGE;
    }

    for (int i = 0; i < m_ptr_iterators.size(); i++)
    {
        if (m_ptr_iterators[i]->m_pos > index)
        {
            m_ptr_iterators[i]->m_pos--;
        }
        else if (m_ptr_iterators[i]->m_pos == index)
        {
            deleteIterator(m_ptr_iterators[i]);
        }
    }

    m_ptr_points.remove(index);

    return ERR_OK;
}

int Set_0::contains(IVector const* const p_element, bool& result) const
{
    if (!p_element)
    {
        LOG("ERR: Incorrect argument");
        return ERR_WRONG_ARG;
    }
    if  (m_dim != p_element->getDim())
    {
        LOG("ERR: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }

    int errType;
    result = false;
    for (int i = 0; i < m_ptr_points.size(); i++)
    {
        errType = p_element->eq(m_ptr_points[i], IVector::NORM_INF, result, EPS);
        if (errType != ERR_OK)
        {
            LOG("ERR: eq failed");
            return errType;
        }
        if (result)
            break;
    }
    return ERR_OK;
}

int Set_0::clear()
{
    for (int i = 0; i < m_ptr_points.size(); i++)
    {
        delete m_ptr_points[i];
    }
    m_ptr_points.clear();

    for (int i = 0; i < m_ptr_iterators.size(); i++)
    {
        delete m_ptr_iterators[i];
    }
    m_ptr_iterators.clear();

    return ERR_OK;
}

Set_0::IIterator* Set_0::begin()
{
    if (m_ptr_points.size() == 0)
    {
        LOG("ERR: Iterator of empty set");
        return nullptr;
    }
    Iterator_0* iterator = new(std::nothrow) Iterator_0(this, 0);
    if (!iterator)
    {
        LOG("ERR: Not enough memory");
        return nullptr;
    }
    m_ptr_iterators.append(iterator);
    return iterator;
}

Set_0::IIterator* Set_0::end()
{
    if (m_ptr_points.size() == 0)
    {
        LOG("ERR: Iterator of empty set");
        return nullptr;
    }
    Iterator_0* iterator = new(std::nothrow) Iterator_0(this, m_ptr_points.size() - 1);
    if (!iterator)
    {
        LOG("ERR: Not enough memory");
        return nullptr;
    }
     m_ptr_iterators.append(iterator);
    return iterator;
}

int Set_0::findIterator(ISet::IIterator const * pIter) const
{
    for (int i = 0; i < m_ptr_iterators.size(); i++)
    {
        if (dynamic_cast<ISet::IIterator*>(m_ptr_iterators[i]) == pIter)
        {
            return i;
        }
    }
    return -1;
}

int Set_0::deleteIterator(IIterator * pIter)
{
    if (!pIter)
    {
        LOG("ERR: Incorrect argument");
        return ERR_WRONG_ARG;
    }

    int indIterator = findIterator(pIter);

    if (indIterator == -1)
    {
        LOG("ERR: Failed to find iterator");
        return ERR_WRONG_ARG;
    }
    else
    {
        delete m_ptr_iterators[indIterator];
        m_ptr_iterators.remove(indIterator);
        return ERR_OK;
    }
}

int Set_0::getByIterator(IIterator const* pIter, IVector*& p_element) const
{
    if (!p_element)
    {
        LOG("ERR: Incorrect argument");
        return ERR_WRONG_ARG;
    }

    int indIterator = findIterator(pIter);

    if (indIterator == -1)
    {
        LOG("ERR: Failed to find iterator");
        return ERR_WRONG_ARG;
    }
    else
    {
        return get(m_ptr_iterators[indIterator]->m_pos, p_element);
    }
}

int Set_0::Iterator_0::next()
{
    if (m_pos + 1 >= m_set->getSize())
    {
        LOG("ERR: Iterator was last");
        return ERR_OUT_OF_RANGE;
    }
    m_pos++;
    return ERR_OK;
}

int Set_0::Iterator_0::prev()
{
    if (m_pos == 0)
    {
        LOG("ERR: Iterator was first");
        return ERR_OUT_OF_RANGE;
    }
    m_pos--;
    return ERR_OK;
}

bool Set_0::Iterator_0::isEnd() const
{
    return m_pos == m_set->getSize() - 1;
}

bool Set_0::Iterator_0::isBegin() const
{
    return m_pos == 0;
}

ISet::IIterator::IIterator(const ISet *const set, int pos) {}

Set_0::Iterator_0::Iterator_0(ISet const* const set, int pos): ISet::IIterator(set, pos), m_set(set), m_pos(pos) {}
