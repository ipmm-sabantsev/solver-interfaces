/// \brief  Rectangle ICompact implementation
/// \author NE Vladimir Sabantsev
/// \date   05.09.2019

// Suppress 'unused parameter' warnings in interface headers
#pragma warning(push)
#pragma warning(disable: 4100)
  #include "common.h"
  #include <ICompact.h>
  #include <IVector.h>
  //#include "vector.h"
  #include <logging.h>
#pragma warning(pop)

#include <QScopedPointer>
#include <QSet>
#include <QVector>
//#include "compact.h"

// Common methods for ICompact implementations
#include "common.cpp"

namespace /* PIMPL_NAMESPACE */ {
  /// \brief Abstract compact
  ///
  /// This class shares some common ICompact methods implementations
  /// with Compact_R and Compact_C classes
  class ACompact : public ICompact {
  /// \brief ICompact methods impl
  public:
    int getId() const ;

    class AIterator : public IIterator
    {
    /// \brief IIterator methods impl
    public:
      int setStep(IVector const* const step = NULL) ;

    /// \brief Internal methods
    public:
      AIterator(const ACompact* const parent,
                IVector* const vector,
                const IVector* const step);
      ~AIterator() ;

      const IVector* vectorPtr() const;

      template<class T>
      const T* parent() const;

    /// \brief Internal variables
    protected:
      const ACompact* const   m_parentCompact;
      QScopedPointer<IVector> m_curVector;
      QScopedPointer<IVector> m_step;
    };

    int deleteIterator(IIterator* pIter) ;
    int getByIterator(IIterator const* pIter, IVector*& pItem) const ;

    IIterator* end(IVector const* const step = NULL) ;
    IIterator* begin(IVector const* const step = NULL) ;

  /// \brief Internal methods
  public:
    ACompact(const IVector* const step);
    ~ACompact() ;

    virtual unsigned int getDim() const = 0;
    bool isValidIterator(const IIterator* const pIter) const;

  private:
    virtual AIterator* createIterator(const IVector* const step, bool begin = true) = 0;

  /// \brief Internal variables
  protected:
    /// \brief Step to iterate through compact
    ///
    /// m_step[i] >= 0.0
    /// m_step[i] = @defaultIncrement if not defined other
    const QScopedPointer<const IVector> m_step;

    /// \brief Internaly created IIterators
    QSet<const ICompact::IIterator* const> m_iterators;

  };

  /// \brief Rectangle ICompact impelmentation
  class Compact_R : public ACompact {
  /// \brief ICompact methods impl
  public:
    class Iterator_R : public AIterator
    {
    /// \brief IIterator methods impl
    public:
      /// \brief Scaner-like iterating steps
      ///
      ///              R                      R
      ///  ┌──────────┐           ┌──────────┐
      ///  │          │           │          │
      ///  │          │           │          │
      ///  │          │     >     │--------->│
      ///  │--------->│           │--------->│
      ///  └──────────┘           └──────────┘
      /// L                      L
      int doStep() ;

    /// \brief Internal methods
    public:
      Iterator_R(const Compact_R* const parent,
                 IVector* const vector,
                 const IVector* const step);

    };

    int isContains(IVector const* const vec, bool& result) const ;

    /// \returns
    /// ERR_OK:          this is subset of other
    /// DIMENSION_ERROR: this is not subset of other
    int isSubSet(ICompact const* const other) const ;

    int getNearestNeighbor(IVector const* vec, IVector*& nn) const ;

    ICompact* clone() const ;

  /// \brief ACompact methods impl
  public:
    unsigned int getDim() const ;
  private:
    AIterator* createIterator(IVector const* const step, bool begin = true) ;

  /// \brief Internal methods
  public:
    Compact_R(
        IVector* const begin,
        IVector* const end,
        const IVector* const step
        );

  /// \brief Internal variables
  protected:
    /// \brief Left bound of compact
    ///
    /// m_leftBound[i] <= m_rightBound[i]
    QScopedPointer<IVector> m_leftBound;

    /// \brief Right bound of compact
    ///
    /// m_leftBound[i] <= m_rightBound[i]
    QScopedPointer<IVector> m_rightBound;

  };

  /// \brief Container ICompact implementation
  ///
  /// This ICompact implementation handles operations,
  /// but does not have its own shape.
  /// It processes other compact shapes with operations
  /// defined to gain illusion of having own shape.
  class Compact_C : public Compact_R
  {
  /// \brief Internal classes declaration
  private:
    enum Operation {
      OPERATION_Intersection = 0,
      OPERATION_Union,
      OPERATION_Difference,
      OPERATION_SymDifference,

      DIMENSION_OPERATION
    };

  /// \brief ICompact methods impl
  public:
    int Intersection(const ICompact& c) ;
    int Union(const ICompact& c) ;
    int Difference(const ICompact& c) ;
    int SymDifference(const ICompact& c) ;
    int MakeConvex() ;

    class Iterator_C : public Iterator_R
    {
    /// \brief IIterator methods impl
    public:
      /// \brief Scaner-like iterating steps
      ///
      ///              R                      R
      ///  ┌──────────┐           ┌──────────┐
      ///  │          │           │          │
      ///  │ ┌──────┐ │           │ ┌──────┐ │
      ///  │ └──────┘ │     >     │-└──────┘>│
      ///  │--------->│           │--------->│
      ///  └──────────┘           └──────────┘
      /// L                      L
      int doStep() ;

    /// \brief Internal methods
    public:
      Iterator_C(const Compact_C* const parent,
                 IVector* const vector,
                 const IVector* const step);

    };

    int isContains(IVector const* const vec, bool& result) const ;
    int isSubSet(ICompact const* const other) const ;

    int getNearestNeighbor(IVector const* vec, IVector *& nn) const ;

    ICompact* clone() const ;

  /// \brief ACompact methods impl
  public:
    unsigned int getDim() const ;
  private:
    AIterator* createIterator(IVector const* const step, bool begin = true) ;

  /// \brief Internal methods
  public:
    Compact_C(
        ICompact* const compact = NULL,
        const IVector* const step = NULL);
    ~Compact_C() ;

  protected:
    int appendTerm(const Operation& operation, ICompact* const compact);

    int update();
    /// \brief Properly clears m_compacts
    ///
    /// Unable to make QScopedPointer container in current Qt Version
    void clearCompacts();

  /// \brief Internal variables
  private:
    /// \brief Processed compacts
    ///
    /// m_compacts.size() == n
    /// Compacts processing rule:
    /// ((m_compacts[0]) m_operations[0] m_compacts[1])
    ///
    /// Unable to make QScopedPointer container in current Qt Version
    QVector<ICompact*> m_compacts;

    /// \brief Operations that will be applied to compacts
    ///
    /// m_operations.size() == n - 1
    /// /// Compacts processing rule:
    /// ((m_compacts[0]) m_operations[0] m_compacts[1])
    QVector<Operation> m_operations;

  };

  /// \brief Default step increment to iterate through compacts
  static const double defaultIncrement = 1e-3;

} /* PIMPL_NAMESPACE */

/* ---- ICompact factory methods ---- */

ICompact* ICompact::createCompact(
    const IVector* const begin,
    const IVector* const end,
    const IVector* const step)
{
  int result = DIMENSION_ERROR;
  unsigned int compDim = 0;
  IVector* m_leftBound(NULL);
  IVector* m_rightBound(NULL);
  IVector* m_step(NULL);

  /* Process begin and end */ {
    if (begin == NULL)
      LOG_RET("begin was NULL", NULL);

    if (end == NULL)
      LOG_RET("end was NULL", NULL);

    compDim = begin->getDim();
    if (compDim == 0UL)
      LOG_RET("Wrong begin vector dimension", NULL);

    if (end->getDim() != compDim)
      LOG_RET("Wrong end vector dimension", NULL);

    m_leftBound = begin->clone();
    if (m_leftBound == NULL)
      LOG_RET("Failed to clone begin vector", NULL);

    m_rightBound = end->clone();
    if (m_rightBound == NULL)
      LOG_RET("Failed to clone end vector", NULL);

    for (unsigned int i = 0; i < compDim; ++i) {
      double beginCoord, endCoord;

      result = m_leftBound->getCoord(i, beginCoord);
      if (result != ERR_OK)
        LOG_RET("Failed to get begin coord: " + std::to_string(i), NULL);

      result = m_rightBound->getCoord(i, endCoord);
      if (result != ERR_OK)
        LOG_RET("Failed to get end coord: " + std::to_string(i), NULL);

      result = m_leftBound->setCoord(i, beginCoord < endCoord ? beginCoord : endCoord);
      if (result != ERR_OK)
        LOG_RET("Failed to change begin coord: " + std::to_string(i), NULL);

      result = m_rightBound->setCoord(  i, beginCoord > endCoord ? beginCoord : endCoord);
      if (result != ERR_OK)
        LOG_RET("Failed to change end coord: " + std::to_string(i), NULL);
    }

  } /* Process begin and end */

  /* Process step */ {
    if (step == NULL) {
      QVector<double> tmpStep(static_cast<int>(compDim), defaultIncrement);
      m_step = IVector::createVector(compDim, tmpStep.data());
      if (m_step == NULL)
        LOG_RET("Failed to create new step IVector", NULL);
    } else {
      result = absVector(step, m_step);
      if (result != ERR_OK)
        LOG_RET("Failed to make step coordinates absolute", NULL);
    }

    if (m_step->getDim() != compDim)
      LOG_RET("Wrong step dimension", NULL);

  } /* Process step */

  const QScopedPointer<const Compact_R> rectCompact(
        new Compact_R(m_leftBound, m_rightBound, m_step));
  if (!rectCompact)
    LOG_RET("Failed to create rectangular compact", NULL);

  Compact_C* abstrCompact =
        new Compact_C(rectCompact.data()->clone(), m_step->clone());
  if (!abstrCompact)
    LOG_RET("Failed to create abstract compact", NULL);

  return abstrCompact;
}

ICompact::IIterator::IIterator(
  ICompact const* const compact,
  int pos, IVector const* const step)
{
  // Suppress 'unused parameter' warnings
  Q_UNUSED(compact)
  Q_UNUSED(pos);
  Q_UNUSED(step);
}

/* ---- ACompact implementation ---- */

int ACompact::getId() const
{
  return ICompact::InterfaceTypes::INTERFACE_0;
}

int ACompact::AIterator::setStep(const IVector* const step)
{
  if (step == NULL)
    m_step.reset(m_parentCompact->m_step->clone());
  else
    m_step.reset(step->clone());

  if (!m_step)
    LOG_RET("Failed to clone step", ERR_ANY_OTHER);

  return ERR_OK;
}

ACompact::AIterator::AIterator(
    const ACompact* const parent,
    IVector* const vector,
    const IVector* const step)
  : IIterator(parent, 0, step),
    m_parentCompact(parent),
    m_curVector(vector),
    m_step(NULL)
{
  Q_ASSERT(m_parentCompact);
  Q_ASSERT(m_curVector);

  Q_ASSERT(setStep(step) == ERR_OK);
  Q_ASSERT(m_step);
}

ACompact::AIterator::~AIterator()
{
  // Nothing to do, all variables are smart ptrs.
  // Destructor is called only from ACompact.
  // ACompact handles iterators QSet by it self.
}

const IVector* ACompact::AIterator::vectorPtr() const
{
  return m_curVector.data();
}

int ACompact::deleteIterator(ICompact::IIterator* pIter)
{
  if (!isValidIterator(pIter))
    LOG_RET("Invalid iterator", ERR_WRONG_ARG);

  m_iterators.remove(pIter);
  delete static_cast<AIterator*>(pIter);

  return ERR_OK;
}

int ACompact::getByIterator(const ICompact::IIterator* pIter, IVector*& pItem) const
{
  if (!isValidIterator(pIter))
    LOG_RET("Invalid iterator", ERR_WRONG_ARG);

  pItem = static_cast<const AIterator*>(pIter)->vectorPtr()->clone();
  if (pItem == NULL)
    LOG_RET("Failed to clone iterated vector", ERR_ANY_OTHER);

  return ERR_OK;
}

ICompact::IIterator* ACompact::end(const IVector* const step)
{
  IIterator* iter = createIterator(step, false);
  if (iter == NULL)
    LOG_RET("Failed to get end iterator", NULL);

  m_iterators.insert(iter);

  return iter;
}

ICompact::IIterator* ACompact::begin(const IVector* const step)
{
  AIterator* iter = createIterator(step, true);
  if (iter == NULL)
    LOG_RET("Failed to get begin iterator", NULL);

  m_iterators.insert(iter);

  return iter;
}

ACompact::ACompact(const IVector* const step)
  : m_step(step),
    m_iterators()
{
  Q_ASSERT(m_step);
}

ACompact::~ACompact()
{
  for (const IIterator* it : m_iterators)
    delete static_cast<const AIterator*>(it);
}

bool ACompact::isValidIterator(const ICompact::IIterator* const pIter) const
{
  if (pIter == NULL)
    LOG_RET("Passed iterator was NULL", false);

  if (m_iterators.find(pIter) == m_iterators.end())
    LOG_RET("Passed iterator does not iterates this compact", false);

  return true;
}

template<class T>
const T* ACompact::AIterator::parent() const
{
  return static_cast<const T*>(m_parentCompact);
}

/* ---- Compact_R implementation ---- */

int Compact_R::Iterator_R::doStep()
{
  int result;

  unsigned int currentDim = 0UL;
  unsigned int compactDim = m_curVector->getDim();

  double element;
  double increment;
  double leftBound;
  double rightBound;

  do {
   /* Variables initialization */ {
      result = m_curVector->getCoord(currentDim, element);
      if (result != ERR_OK)
        LOG_RET("Failed to get current dimension coordinate: " + std::to_string(currentDim), ERR_ANY_OTHER);

      result = m_step->getCoord(currentDim, increment);
      if (result != ERR_OK)
        LOG_RET("Failed to get current dimension step increment: " + std::to_string(currentDim), ERR_ANY_OTHER);

      result = parent<Compact_R>()->m_leftBound->getCoord(currentDim, leftBound);
      if (result != ERR_OK)
        LOG_RET("Failed to get current dimension left boundary: " + std::to_string(currentDim), ERR_ANY_OTHER);

      result = parent<Compact_R>()->m_rightBound->getCoord(currentDim, rightBound);
      if (result != ERR_OK)
        LOG_RET("Failed to get current dimension right boundary: " + std::to_string(currentDim), ERR_ANY_OTHER);
    }

    element += increment;
    if (element <= rightBound) { /* Move a bit in current dimension */
      result = m_curVector->setCoord(currentDim, element);
      if (result != ERR_OK)
        LOG_RET("Failed to increment current vector", ERR_ANY_OTHER);

      return ERR_OK;
    } else { /* Start from scratch in higher dimension */
      result = m_curVector->setCoord(currentDim, leftBound);
      if (result != ERR_OK)
        LOG_RET("Failed to increment current vector", ERR_ANY_OTHER);

      currentDim += 1;
    }

  } while (currentDim < compactDim);
  return ERR_OUT_OF_RANGE;
}

Compact_R::Iterator_R::Iterator_R(
    const Compact_R* const parent,
    IVector* const vector,
    const IVector* const step)
  : AIterator(parent, vector, step)
{  }

int Compact_R::isContains(const IVector* const vec, bool &result) const
{
  if (vec == NULL)
    LOG_RET("vector was NULL", ERR_WRONG_ARG);

  unsigned int compDim = getDim();
  if (vec->getDim() != compDim)
    LOG_RET("vector was wrong dimension", ERR_DIMENSIONS_MISMATCH);

  for (unsigned int i = 0; i < compDim; ++i) {
    double vec_elem;
    if (vec->getCoord(i, vec_elem) != ERR_OK)
      LOG_RET("Failed to get vector element", ERR_OUT_OF_RANGE);

    double begin_elem;
    if (m_leftBound->getCoord(i, begin_elem) != ERR_OK)
      LOG_RET("Failed to get begin element", ERR_OUT_OF_RANGE);

    double end_elem;
    if (m_rightBound->getCoord(i, end_elem) != ERR_OK)
      LOG_RET("Failed to get begin element", ERR_OUT_OF_RANGE);

    if (vec_elem < begin_elem - 0.001 ||
        vec_elem > end_elem) {
      result = false;
      return ERR_OK;
    }
  }

  result = true;
  return ERR_OK;
}

int Compact_R::isSubSet(const ICompact* const other) const
{
  LOG("Ill formated interface:");
  LOG("  1. begin() and end() methods should have const variation");
  LOG("  2. isSubSet result out param is absent");
  LOG("Method returns:");
  LOG("  ERR_OK:          this is subset of other");
  LOG("  DIMENSION_ERROR: this is not subset of other");

  int result;

  bool other_contains_this_begin;
  result = other->isContains(m_leftBound.data(), other_contains_this_begin);
  if (result != ERR_OK)
    LOG_RET("Failed to check if other contains left boundary of this compact", ERR_ANY_OTHER);

  bool other_contains_this_end;
  result = other->isContains(m_rightBound.data(), other_contains_this_end);
  if (result != ERR_OK)
    LOG_RET("Failed to check if other contains left boundary of this compact", ERR_ANY_OTHER);

  return other_contains_this_begin && other_contains_this_end ?
        ERR_OK : DIMENSION_ERROR;
}

int Compact_R::getNearestNeighbor(const IVector* vec, IVector*& nn) const
{
  int result;

  if (vec == NULL)
    LOG_RET("IVector passed was NULL", ERR_WRONG_ARG);

  unsigned int compDim = getDim();
  if (vec->getDim() != compDim)
    LOG_RET("IVector passed has wrong dimension", ERR_DIMENSIONS_MISMATCH);

  QVector<double> neighborVector(static_cast<int>(compDim));
  for (unsigned int coord = 0; coord < compDim; ++coord) {
    double iterator;
    result = m_leftBound->getCoord(coord, iterator);
    if (result != ERR_OK)
      LOG_RET("Failed to get current dimension left boundary: " + std::to_string(coord), ERR_ANY_OTHER);

    double increment;
    result = m_step->getCoord(coord, increment);
    if (result != ERR_OK)
      LOG_RET("Failed to get current dimension step increment: " + std::to_string(coord), ERR_ANY_OTHER);

    double bound;
    result = m_rightBound->getCoord(coord, bound);
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
  if (nn == NULL)
    LOG_RET("Failed to create IVector nn", ERR_ANY_OTHER);

  return ERR_OK;
}

ICompact* Compact_R::clone() const
{
  return new Compact_R(m_leftBound->clone(), m_rightBound->clone(), m_step->clone());
}

Compact_R::Compact_R(
    IVector* const begin,
    IVector* const end,
    const IVector* const step)
  : ACompact(step),
    m_leftBound(begin),
    m_rightBound(end)
{
  //Q_ASSERT(m_leftBound);
  //Q_ASSERT(m_rightBound);
}

unsigned int Compact_R::getDim() const
{
  return m_leftBound->getDim();
}

ACompact::AIterator* Compact_R::createIterator(const IVector* const step, bool begin)
{
  IVector* iter_vec = begin ? m_leftBound->clone() : m_rightBound->clone();
  if (iter_vec == NULL)
    LOG_RET("Failed to clone vector", NULL);

  AIterator* iter = new Iterator_R(this, iter_vec, step);
  if (iter == NULL)
    LOG_RET("Failed to create new IIterator", NULL);

  return iter;
}

/* ---- Compact_C implementation ---- */

int Compact_C::Iterator_C::doStep()
{
  int result;
  bool contains;
  do {
    result = Iterator_R::doStep();
    if (result != ERR_OK)
      return result;

    result = parent<Compact_C>()->isContains(vectorPtr(), contains);
    if (result != ERR_OK)
      LOG_RET("Failed to check if current vector contains in compact", ERR_ANY_OTHER);
  } while (!contains);

  return ERR_OK;
}

Compact_C::Iterator_C::Iterator_C(
    const Compact_C* const parent,
    IVector* const vector,
    const IVector* const step)
  : Iterator_R(parent, vector, step)
{  }

int Compact_C::Intersection(const ICompact& c)
{
  return appendTerm(OPERATION_Intersection, c.clone());
}

int Compact_C::Union(const ICompact& c)
{
  return appendTerm(OPERATION_Union, c.clone());
}

int Compact_C::Difference(const ICompact& c)
{
  return appendTerm(OPERATION_Difference, c.clone());
}

int Compact_C::SymDifference(const ICompact& c)
{
  return appendTerm(OPERATION_SymDifference, c.clone());
}

int Compact_C::MakeConvex()
{
  IVector* begin = m_leftBound->clone();
  IVector* end   = m_rightBound->clone();
  IVector* step  = m_step->clone();

  if (begin == NULL ||
      end   == NULL ||
      step  == NULL)
    LOG_RET("Failed to clone convex compact data", ERR_MEMORY_ALLOCATION);

  clearCompacts();
  m_operations.clear();

  Compact_R* comp_convex = new Compact_R(begin, end, step);
  if (comp_convex == NULL)
    LOG_RET("Failed to create convex subCompact", ERR_MEMORY_ALLOCATION);

  m_compacts.append(comp_convex);
  update();

  return ERR_OK;
}

int Compact_C::isContains(const IVector* const vec, bool& result) const
{
  int errResult;
  int i = 0;

  errResult = m_compacts[0]->isContains(vec, result);
  if (errResult != ERR_OK)
    LOG_RET("Failed to check if vec contains in compact: " + std::to_string(i), ERR_ANY_OTHER);

  bool contains_in_current;
  for (i = 1; i < m_compacts.size(); ++i) {
    errResult = m_compacts[i]->isContains(vec, contains_in_current);
    if (errResult != ERR_OK)
      LOG_RET("Failed to check if vec contains in compact: " + std::to_string(i), ERR_ANY_OTHER);

    switch (m_operations[i - 1]) {
    case OPERATION_Intersection:
      result = result && contains_in_current;
      break;
    case OPERATION_Union:
      result = result || contains_in_current;
      break;
    case OPERATION_Difference:
      result = result && !contains_in_current;
      break;
    case OPERATION_SymDifference:
      result = (result || contains_in_current) && !(result && contains_in_current);
      break;
    default:
      return ERR_NOT_IMPLEMENTED;
    }
  }

  return ERR_OK;
}

int Compact_C::isSubSet(const ICompact* const other) const
{
  Q_UNUSED(other);
  return ERR_NOT_IMPLEMENTED;
}

// Not quite right implementation
// nn is gauranted to be nearest
// nn is not gauranted to be found
//
// Implementation should be more like Compact_R::getNearestNeighbor
// but with modifications tracking non convex nature of Compact_C
int Compact_C::getNearestNeighbor(const IVector* vec, IVector*& nn) const
{
  int result;

  QScopedPointer<IVector> meth_nearest(NULL);
  double min_distance = std::nan("");
  for (auto compact : m_compacts) {
    QScopedPointer<IVector> for_nearest; {
      IVector* l_nearest_ptr;
      compact->getNearestNeighbor(vec, l_nearest_ptr);
      if (l_nearest_ptr == NULL)
        LOG_RET("Failed to get nearest to subCompact", ERR_ANY_OTHER);
      for_nearest.reset(l_nearest_ptr);
    }

    bool contains; {
      result = isContains(for_nearest.data(), contains);
      if (result != ERR_OK)
        LOG_RET("Failed to check if near vector contains in compact", ERR_ANY_OTHER);
    }

    // Not quite right implementation
    // We should continue to search for containing nearest
    // instead of wiping non containing
    if (contains) {
      QScopedPointer<IVector> diff(IVector::subtract(vec, for_nearest.data()));

      double distance;
      result = diff->norm(IVector::NORM_1, distance);
      if (result != ERR_OK)
        LOG_RET("Failed to get distance btw vecotors", ERR_ANY_OTHER);

      // Compare with nan always false
      if (!(min_distance > distance))
        meth_nearest.reset(for_nearest->clone());
    }
  }

  if (meth_nearest)
    nn = meth_nearest->clone();
  else
    nn = NULL;

  return ERR_OK;
}

ICompact* Compact_C::clone() const
{
  IVector* step_clone = m_step->clone();
  if (step_clone == NULL)
    LOG_RET("Failed to clone step", NULL);

  Compact_C* this_clone = new Compact_C(NULL, step_clone);
  if (this_clone == NULL)
    LOG_RET("Failed to create empty abstract compact", NULL);

  for (auto it = m_compacts.begin(); it != m_compacts.end(); ++it) {
    ICompact* comp_clone = (*it)->clone();
    if (comp_clone == NULL) {
      delete this_clone;
      LOG_RET("Failed to clone inner compact", NULL);
    }

    this_clone->m_compacts.append(comp_clone);
  }

  for (auto it = m_operations.begin(); it != m_operations.end(); ++it)
    this_clone->m_operations.append(*it);

  this_clone->update();

  return this_clone;
}

Compact_C::Compact_C(
    ICompact* const compact,
    const IVector* const step)
  : Compact_R(NULL, NULL, step),
    m_compacts(),
    m_operations()
{
  // compact could be NULL only while clone()
  if (compact != NULL) {
    m_compacts.append(compact);
    update();
  }
}

Compact_C::~Compact_C()
{
  clearCompacts();
}

unsigned int Compact_C::getDim() const
{
  int result;

  ICompact* inner_compact = m_compacts[0];
  if (inner_compact == NULL)
    LOG_RET("Failed to get inner compact", 0UL);

  ICompact::IIterator* iter = inner_compact->begin();
  if (iter == NULL)
    LOG_RET("Failed to get inner compact begin iterator", 0UL);

  IVector* inner_begin;
  result = inner_compact->getByIterator(iter, inner_begin);
  if (result != ERR_OK)
    LOG_RET("Failed to get inner compact begin vector", 0UL);

  return inner_begin->getDim();
}

ACompact::AIterator* Compact_C::createIterator(const IVector* const step, bool begin)
{
  if (m_leftBound == NULL || m_rightBound == NULL)
    LOG_RET("Bounds are not valid", NULL);

  IVector* iter_vec = begin ? m_leftBound->clone() : m_rightBound->clone();
  if (iter_vec == NULL)
    LOG_RET("Failed to clone vector", NULL);

  AIterator* iter = new Iterator_C(this, iter_vec, step);
  if (iter == NULL)
    LOG_RET("Failed to create new IIterator", NULL);

  return iter;
}

int Compact_C::appendTerm(const Compact_C::Operation& operation, ICompact* const compact)
{
  m_compacts.append(compact);
  m_operations.append(operation);
  update();

  return ERR_OK;
}

int Compact_C::update()
{
  int result;

  unsigned int compDim = getDim();

  /* Reset bounds */ {
    // Compare with NAN are always false
    QVector<double> empty(static_cast<int>(compDim), std::nan(""));

    m_leftBound.reset(IVector::createVector(compDim, empty.data()));
    if (!m_leftBound)
      LOG_RET("Failed to create empty vector", ERR_ANY_OTHER);

    m_rightBound.reset(IVector::createVector(compDim, empty.data()));
    if (!m_rightBound)
      LOG_RET("Failed to create empty vector", ERR_ANY_OTHER);
  } /* Reset bounds */

  for (auto comp_it = m_compacts.begin(); comp_it != m_compacts.end(); ++comp_it) {
    IVector* leftBound(NULL);
    IVector* rightBound(NULL);
    /* Get bounds of current compact */ {
      ICompact::IIterator* begin_it = (*comp_it)->begin();
      if (begin_it == NULL)
        LOG_RET("Failed to get begin iterator", ERR_ANY_OTHER);

      IVector* begin_vec(NULL);
      result = (*comp_it)->getByIterator(begin_it, begin_vec);
      if (result != ERR_OK || begin_vec == NULL)
        LOG_RET("Failed to get vector from begin iterator", ERR_ANY_OTHER);

      ICompact::IIterator* end_it = (*comp_it)->end();
      if (end_it == NULL)
        LOG_RET("Failed to get end iterator", ERR_ANY_OTHER);

      IVector* end_vec(NULL);
      result = (*comp_it)->getByIterator(end_it, end_vec);
      if (result != ERR_OK || end_vec == NULL)
        LOG_RET("Failed to get vector from end iterator", ERR_ANY_OTHER);

      result = getBound(begin_vec, end_vec, leftBound, true);
      if (result != ERR_OK || leftBound == NULL)
        LOG_RET("Failed to detect leftBounds of inner compact", ERR_ANY_OTHER);

      result = getBound(begin_vec, end_vec, rightBound, false);
      if (result != ERR_OK || rightBound == NULL)
        LOG_RET("Failed to detect rightBounds of inner compact", ERR_ANY_OTHER);
    } /* Get bounds of current compact */

    for (unsigned int i = 0; i < compDim; ++i) {
      double m_leftBound_value, leftBound_value;
      double m_rightBound_value, rightBound_value;
      /* Get current dim values */ {
        result = m_leftBound->getCoord(i, m_leftBound_value);
        if (result != ERR_OK)
          LOG_RET("Failed to get vector coord: " + std::to_string(i), ERR_ANY_OTHER);

        result = leftBound->getCoord(i, leftBound_value);
        if (result != ERR_OK)
          LOG_RET("Failed to get vector coord: " + std::to_string(i), ERR_ANY_OTHER);

        result = m_rightBound->getCoord(i, m_rightBound_value);
        if (result != ERR_OK)
          LOG_RET("Failed to get vector coord: " + std::to_string(i), ERR_ANY_OTHER);

        result = rightBound->getCoord(i, rightBound_value);
        if (result != ERR_OK)
          LOG_RET("Failed to get vector coord: " + std::to_string(i), ERR_ANY_OTHER);
      } /* Get current dim values */

      result = m_leftBound->setCoord(i, m_leftBound_value < leftBound_value ?
                                        m_leftBound_value : leftBound_value);
      if (result != ERR_OK)
        LOG_RET("Failed to set vector coord: " + std::to_string(i), ERR_ANY_OTHER);

      result = m_rightBound->setCoord(i, m_rightBound_value > rightBound_value ?
                                         m_rightBound_value : rightBound_value);
      if (result != ERR_OK)
        LOG_RET("Failed to set vector coord: " + std::to_string(i), ERR_ANY_OTHER);
    }
  }

  return ERR_OK;
}

void Compact_C::clearCompacts()
{
  for (auto it = m_compacts.begin(); it != m_compacts.end(); ++it)
    delete (*it);
  m_compacts.clear();
}
