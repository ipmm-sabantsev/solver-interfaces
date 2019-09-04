/// \brief  Rectangle ICompact implementation
/// \author Vladimir Sabantsev
/// \date   05.09.2019

// Suppress 'unused parameter' warnings in interface headers
#pragma warning(push)
#pragma warning(disable: 4100)
  #include "common.h"
  #include <ICompact.h>
  #include <IVector.h>
  #include <logging.h>
#pragma warning(pop)

#include <QScopedPointer>
#include <QSet>
#include <QVector>

// Common methods for ICompact implementations
#include "common.cpp"

namespace /* PIMPL_NAMESPACE */ {
  /// \brief Rectangle ICompact impelmentation
  class Compact_0 : public ICompact {
  /// \brief ICompact methods impl
  public:
    int getId() const override;

    class Iterator_0 : public IIterator
    {
    /// \brief IIterator methods impl
    public:
      int doStep() override;
      int setStep(IVector const* const step = nullptr) override;

    /// \brief Internal methods
    public:
      Iterator_0(const Compact_0* const parent,
                 IVector* const vector,
                 IVector* const step);
      ~Iterator_0() override;

      const IVector* getVectorPtr() const;

    private:
      Iterator_0(const Iterator_0& other) = delete;
      void operator=(const Iterator_0& other) = delete;

    /// \brief Internal variables
    private:
      QScopedPointer<const Compact_0> m_parentCompact;
      QScopedPointer<IVector>         m_curVector;
      QScopedPointer<IVector>         m_step;

      unsigned int m_lastModifiedDim;
    };

    int deleteIterator(IIterator* pIter) override;
    int getByIterator(IIterator const* pIter, IVector*& pItem) const override;

    IIterator* end(IVector const* const step = nullptr) override;
    IIterator* begin(IVector const* const step = nullptr) override;

    int isContains(IVector const* const vec, bool& result) const override;

    /// \returns
    /// ERR_OK:          this is subset of other
    /// DIMENSION_ERROR: this is not subset of other
    int isSubSet(ICompact const* const other) const override;

    int getNearestNeighbor(IVector const* vec, IVector*& nn) const override;

    ICompact* clone() const override;

  /// \brief Internal methods
  public:
    Compact_0(
        const IVector* const begin,
        const IVector* const end,
        const IVector* const step
        );
    ~Compact_0() override;

    unsigned int getDim() const;
    bool isValidIterator(const IIterator* const pIter) const;

  private:
    Iterator_0* createIterator(IVector const* const step, bool begin = true);

  /// \brief Internal variables
  public:
    /// \brief Default step increment to iterate through compact
    ///
    /// m_step[i] = defaultIncrement if other not defined
    static const double defaultIncrement;
  private:
    /// \brief Left bound of compact
    ///
    /// m_begin[i] <= m_end[i]
    QScopedPointer<const IVector> m_begin;

    /// \brief Right bound of compact
    ///
    /// m_begin[i] <= m_end[i]
    QScopedPointer<const IVector> m_end;

    /// \brief Step to iterate compact through
    ///
    /// m_step[i] >= 0.0
    QScopedPointer<const IVector> m_step;

    /// \brief Internaly created IIterators
    QSet<const ICompact::IIterator* const> m_iterators;

  };

} /* PIMPL_NAMESPACE */

const double Compact_0::defaultIncrement = 1e-3;

ICompact* ICompact::createCompact(
    const IVector* const begin,
    const IVector* const end,
    const IVector* const step)
{
  int result = DIMENSION_ERROR;
  unsigned int compDim = 0;
  IVector* m_begin(nullptr);
  IVector* m_end(nullptr);
  IVector* m_step(nullptr);

  /* Process begin and end */ {
    if (begin == nullptr)
      LOG_RET("begin was nullptr", nullptr);

    if (end == nullptr)
      LOG_RET("end was nullptr", nullptr);

    compDim = begin->getDim();
    if (compDim == 0UL)
      LOG_RET("Wrong begin vector dimension", nullptr);

    if (end->getDim() != compDim)
      LOG_RET("Wrong end vector dimension", nullptr);

    m_begin = begin->clone();
    if (m_begin == nullptr)
      LOG_RET("Failed to clone begin vector", nullptr);

    m_end = end->clone();
    if (m_end == nullptr)
      LOG_RET("Failed to clone end vector", nullptr);

    for (unsigned int i = 0; i < compDim; ++i) {
      double beginCoord, endCoord;

      result = m_begin->getCoord(i, beginCoord);
      if (result != ERR_OK)
        LOG_RET("Failed to get begin coord: " + std::to_string(i), nullptr);

      result = m_end->getCoord(i, endCoord);
      if (result != ERR_OK)
        LOG_RET("Failed to get end coord: " + std::to_string(i), nullptr);

      result = m_begin->setCoord(i, beginCoord < endCoord ? beginCoord : endCoord);
      if (result != ERR_OK)
        LOG_RET("Failed to change begin coord: " + std::to_string(i), nullptr);

      result = m_end->setCoord(  i, beginCoord > endCoord ? beginCoord : endCoord);
      if (result != ERR_OK)
        LOG_RET("Failed to change end coord: " + std::to_string(i), nullptr);
    }

  } /* Process begin and end */

  /* Process step */ {
    if (!step) {
      QVector<double> tmpStep(static_cast<int>(compDim), Compact_0::defaultIncrement);
      m_step = IVector::createVector(compDim, tmpStep.data());
      if (m_step == nullptr)
        LOG_RET("Failed to create new step IVector", nullptr);
    } else {
      result = absVector(step, m_step);
      if (result != ERR_OK)
        LOG_RET("Failed to make step coordinates absolute", nullptr);
    }

    if (m_step->getDim() != compDim)
      LOG_RET("Wrong step dimension", nullptr);

  } /* Process step */

  return new Compact_0(m_begin, m_end, m_step);
}

int Compact_0::getId() const
{
  return ICompact::InterfaceTypes::INTERFACE_0;
}

int Compact_0::deleteIterator(ICompact::IIterator* pIter)
{
  if (!isValidIterator(pIter))
    LOG_RET("Invalid iterator", ERR_WRONG_ARG);

  m_iterators.remove(pIter);
  delete static_cast<Iterator_0*>(pIter);

  return ERR_OK;
}

int Compact_0::getByIterator(const ICompact::IIterator* pIter, IVector*& pItem) const
{
  if (!isValidIterator(pIter))
    LOG_RET("Invalid iterator", ERR_WRONG_ARG);

  pItem = static_cast<const Iterator_0*>(pIter)->getVectorPtr()->clone();
  if (pItem == nullptr)
    LOG_RET("Failed to clone iterated vector", ERR_ANY_OTHER);

  return ERR_OK;
}

ICompact::IIterator* Compact_0::end(const IVector* const step)
{
  IIterator* iter = createIterator(step, false);
  if (iter == nullptr)
    LOG_RET("Failed to get end iterator", nullptr);

  m_iterators.insert(iter);

  return iter;
}

ICompact::IIterator* Compact_0::begin(const IVector* const step)
{
  IIterator* iter = createIterator(step, true);
  if (iter == nullptr)
    LOG_RET("Failed to get begin iterator", nullptr);

  m_iterators.insert(iter);

  return iter;
}

int Compact_0::isContains(const IVector* const vec, bool &result) const
{
  if (vec == nullptr)
    LOG_RET("vector was nullptr", ERR_WRONG_ARG);

  unsigned int compDim = getDim();
  if (vec->getDim() != compDim)
    LOG_RET("vector was wrong dimension", ERR_DIMENSIONS_MISMATCH);

  for (unsigned int i = 0; i < compDim; ++i) {
    double vec_elem;
    if (vec->getCoord(i, vec_elem) != ERR_OK)
      LOG_RET("Failed to get vector element", ERR_OUT_OF_RANGE);

    double begin_elem;
    if (m_begin->getCoord(i, begin_elem) != ERR_OK)
      LOG_RET("Failed to get begin element", ERR_OUT_OF_RANGE);

    double end_elem;
    if (m_begin->getCoord(i, end_elem) != ERR_OK)
      LOG_RET("Failed to get begin element", ERR_OUT_OF_RANGE);

    if (vec_elem < begin_elem ||
        vec_elem > end_elem) {
      result = false;
      return ERR_OK;
    }
  }

  result = true;
  return ERR_OK;
}

int Compact_0::isSubSet(const ICompact* const other) const
{
  LOG("Ill formated interface:");
  LOG("  1. begin() and end() methods should have const variation");
  LOG("  2. isSubSet result out param is absent");
  LOG("Method returns:");
  LOG("  ERR_OK:          this is subset of other");
  LOG("  DIMENSION_ERROR: this is not subset of other");

  int result;

  bool other_contains_this_begin;
  result = other->isContains(m_begin.data(), other_contains_this_begin);
  if (result != ERR_OK)
    LOG_RET("Failed to check if other contains left boundary of this compact", ERR_ANY_OTHER);

  bool other_contains_this_end;
  result = other->isContains(m_end.data(), other_contains_this_end);
  if (result != ERR_OK)
    LOG_RET("Failed to check if other contains left boundary of this compact", ERR_ANY_OTHER);

  return other_contains_this_begin && other_contains_this_end ?
        ERR_OK : DIMENSION_ERROR;
}

int Compact_0::getNearestNeighbor(const IVector* vec, IVector*& nn) const
{
  int result;

  if (vec == nullptr)
    LOG_RET("IVector passed was nullptr", ERR_WRONG_ARG);

  unsigned int compDim = getDim();
  if (vec->getDim() != compDim)
    LOG_RET("IVector passed has wrong dimension", ERR_DIMENSIONS_MISMATCH);

  QVector<double> neighborVector(static_cast<int>(compDim));
  for (unsigned int coord = 0; coord < compDim; ++coord) {
    double iterator;
    result = m_begin->getCoord(coord, iterator);
    if (result != ERR_OK)
      LOG_RET("Failed to get current dimension left boundary: " + std::to_string(coord), ERR_ANY_OTHER);

    double increment;
    result = m_step->getCoord(coord, increment);
    if (result != ERR_OK)
      LOG_RET("Failed to get current dimension step increment: " + std::to_string(coord), ERR_ANY_OTHER);

    double bound;
    result = m_end->getCoord(coord, bound);
    if (result != ERR_OK)
      LOG_RET("Failed to get current dimension right boundary: " + std::to_string(coord), ERR_ANY_OTHER);

    double target;
    result = vec->getCoord(coord, target);
    if (result != ERR_OK)
      LOG_RET("Failed to get current dimension target: " + std::to_string(coord), ERR_ANY_OTHER);

    while (true) {
      iterator += increment;

      // Edge cases
      if (iterator > bound) {
        neighborVector[static_cast<int>(coord)] =
            std::abs(target - bound) < std::abs(target - (iterator - increment)) ?
                              bound  :                   (iterator - increment);
        break;
      }

      // Inner cases
      if (iterator > target) {
        neighborVector[static_cast<int>(coord)] =
            std::abs(target - iterator) < std::abs(target - (iterator - increment)) ?
                              iterator  :                    iterator - increment;
        break;
      }
    }
  }

  nn = IVector::createVector(compDim, neighborVector.data());
  if (nn == nullptr)
    LOG_RET("Failed to create IVector nn", ERR_ANY_OTHER);

  return ERR_OK;
}

ICompact* Compact_0::clone() const
{
  return new Compact_0(m_begin->clone(), m_end->clone(), m_step->clone());
}

Compact_0::Compact_0(
    const IVector* const begin,
    const IVector* const end,
    const IVector* const step)
  : m_begin(begin),
    m_end(end),
    m_step(step)
{
  Q_ASSERT(m_begin);
  Q_ASSERT(m_end);
  Q_ASSERT(m_step);
}

Compact_0::~Compact_0()
{
  for (const IIterator* it : m_iterators)
    delete static_cast<const Iterator_0*>(it);
}

unsigned int Compact_0::getDim() const
{
  return m_begin->getDim();
}

bool Compact_0::isValidIterator(const ICompact::IIterator* const pIter) const
{
  if (pIter == nullptr)
    LOG_RET("Passed iterator was nullptr", false);

  if (m_iterators.find(pIter) == m_iterators.end())
    LOG_RET("Passed iterator does not iterates this compact", false);

  return true;
}

Compact_0::Iterator_0* Compact_0::createIterator(const IVector* const step, bool begin)
{
  IVector* _step(nullptr);
  if (step != nullptr) {
    if (absVector(step, _step) != ERR_OK)
      LOG_RET("Failed to make step coordinates absolute", nullptr);
  } else {
    _step = m_step->clone();
  }

  IVector* vector = begin ?
        m_begin.data()->clone() : m_end.data()->clone();
  if (vector == nullptr)
    LOG_RET("Failed to clone end vector", nullptr);

  return new Iterator_0(this, vector, _step);
}

int Compact_0::Iterator_0::doStep()
{
  int result;
  double element;
  double increment;
  double bound;

  do {
    result = m_curVector->getCoord(m_lastModifiedDim, element);
    if (result != ERR_OK)
      LOG_RET("Failed to get current dimension coordinate: " + std::to_string(m_lastModifiedDim), ERR_ANY_OTHER);

    result = m_step->getCoord(m_lastModifiedDim, increment);
    if (result != ERR_OK)
      LOG_RET("Failed to get current dimension step increment: " + std::to_string(m_lastModifiedDim), ERR_ANY_OTHER);

    result = m_parentCompact->m_end->getCoord(m_lastModifiedDim, bound);
    if (result != ERR_OK)
      LOG_RET("Failed to get current dimension right boundary: " + std::to_string(m_lastModifiedDim), ERR_ANY_OTHER);

    element += increment;

    if (element + increment > bound) {
      m_lastModifiedDim += 1;
      if (m_lastModifiedDim == m_curVector->getDim())
        return ERR_OUT_OF_RANGE;
    } else {
      result = m_curVector->setCoord(m_lastModifiedDim, element);
      if (result != ERR_OK)
        LOG_RET("Failed to increment current vector", ERR_ANY_OTHER);

      return ERR_OK;
    }
  } while(true);
}

int Compact_0::Iterator_0::setStep(const IVector* const step)
{
  if (step == nullptr)
    m_step.reset(m_parentCompact->m_step->clone());
  else
    m_step.reset(step->clone());

  if (!m_step)
    LOG_RET("Failed to clone step", ERR_ANY_OTHER);

  return ERR_OK;
}

ICompact::IIterator::IIterator(
  ICompact const* const compact, int pos, IVector const* const step)
{
  // Suppress 'unused parameter' warnings
  Q_UNUSED(compact)
  Q_UNUSED(pos);
  Q_UNUSED(step);
}

Compact_0::Iterator_0::Iterator_0(
    const Compact_0* const parent,
    IVector* const vector,
    IVector* const step)
  : IIterator(parent, 0, step),
    m_parentCompact(parent),
    m_curVector(vector),
    m_step(step),
    m_lastModifiedDim(0UL)
{
  Q_ASSERT(m_parentCompact);
  Q_ASSERT(m_curVector);
  Q_ASSERT(m_step);
}

Compact_0::Iterator_0::~Iterator_0()
{
  // nothing to do, all variables are smart ptrs
  // destructor is called only from Compact_0
  // Compact_0 handles iterators set by it self
}

const IVector* Compact_0::Iterator_0::getVectorPtr() const
{
  return m_curVector.data();
}
