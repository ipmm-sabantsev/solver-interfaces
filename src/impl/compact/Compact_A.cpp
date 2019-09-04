#include "ICompact.h"

#include <logging.h>
#include <IVector.h>
#include <QVector>

namespace  {
  /// \brief Abstract ICompact implementation
  ///
  /// This ICompact implementation handles operations,
  /// but does not have its own shape.
  /// It processes other compact shapes with operations
  /// defined to gain illusion of having own shape.
  class Compact_A : public ICompact {
  /// \brief Internal classes declaration
  private:
    enum Operation {
      OPERATION_Intersection = 0,
      OPERATION_Union,
      OPERATION_Difference,

      DIMENSION_OPERATION
    };

    struct Term;

  /// \brief ICompact methods impl
  public:
    int getId() const override;

    int Intersection(ICompact const& c) override;
    int Union(ICompact const& c) override;
    int Difference(ICompact const& c) override;
    int SymDifference(ICompact const& c) override;

    class Iterator_0 : public IIterator
    {
    /// \brief IIterator methods impl
    public:
      int doStep() override;
      int setStep(IVector const* const step = nullptr) override;

    /// \brief Internal methods
    public:
      Iterator_0(const Compact_A* const parent,
                 const IVector* const vector,
                 const IVector* const step);
    protected:
      ~Iterator_0() override;

    private:
      Iterator_0(const Iterator_0& other) = delete;
      void operator=(const Iterator_0& other) = delete;

    /// \brief Internal variables
    private:
      const Compact_A* const m_parentCompact;
      const IVector*         m_curVector;
      IVector* const         m_step;
    };

    int deleteIterator(IIterator * pIter) override;
    int getByIterator(IIterator const* pIter, IVector*& pItem) const override;

    IIterator* end(IVector const* const step = nullptr) override;
    IIterator* begin(IVector const* const step = nullptr) override;

    int isContains(IVector const* const vec, bool& result) const override;
    int isSubSet(ICompact const* const other) const override;

    int getNearestNeighbor(IVector const* vec, IVector *& nn) const override;

    ICompact* clone() const override;

  /// \brief Internal methods
  public:
    Compact_A(const IVector* const step);
    ~Compact_A() override;

    unsigned int getDim() const;

  protected:
    int appendTerm(Operation const& op,ICompact const& c);
    IIterator* getIterator(bool lesser, IVector const* const step = nullptr);

  /// \brief Internal variables
  private:
    // ((<op_dim> m_terms[0]) <op> m_terms[1]) ...
    QVector<Term> m_terms;
    const IVector* const m_step;

  /// \brief Internal classes definition
  private:
    struct Term {
    public:
      const Operation m_operation;
      ICompact* const m_member;

      Term(const Operation& operation,
           const ICompact* const member);
      ~Term();

    private:
        Term(const Term& other) = delete;
        void operator=(const Term& other) = delete;

    };
  };
}

ICompact* ICompact::createCompact(
    const IVector *const begin,
    const IVector *const end,
    const IVector *const step)
{

  return new Compact_A(step);
}

Compact_A::Term::Term(const Compact_A::Operation &operation,
                      const ICompact * const member) :
  m_operation(operation),
  m_member(member->clone())
{
  Q_ASSERT(m_member != nullptr);
}

Compact_A::Term::~Term()
{
  delete m_member;
}

Compact_A::Iterator_0::Iterator_0(
    const Compact_A * const parent,
    const IVector * const vector,
    const IVector * const step) :
  IIterator(parent, 0, step),
  m_parentCompact(parent),
  m_curVector(vector->clone()),
  m_step(step->clone())
{
  Q_ASSERT(m_parentCompact);
  Q_ASSERT(m_curVector);
  Q_ASSERT(m_step);
}

int Compact_A::getId() const
{
  return ICompact::InterfaceTypes::INTERFACE_0;
}

int Compact_A::appendTerm(const Compact_A::Operation &op, const ICompact &c)
{
  ICompact* c_clone = c.clone();
  if (c_clone == nullptr)
    return ERR_MEMORY_ALLOCATION;

  m_terms.push_back(Term(op, c_clone));
  return ERR_OK;
}

ICompact::IIterator* Compact_A::getIterator(
  bool lesser, IVector const* const step)
{
  int result = DIMENSION_ERROR;
  unsigned int compDim = getDim();
  double* bound_d = new double[compDim];
  if (bound_d == nullptr)
    return nullptr;

  for(int i = 0; i < m_terms.size(); ++i) {
    ICompact* const cur = m_terms[i].m_member;
    const IIterator* cur_begin_it = cur->begin();
    const IIterator* cur_end_it   = cur->end();

    if (cur_begin_it == nullptr || cur_end_it == nullptr)
      LOG_RET("Failed to get term iterator", nullptr);

    IVector* cur_begin;
    result = cur->getByIterator(cur_begin_it, cur_begin);
    if (result != ERR_OK || cur_begin == nullptr)
      LOG_RET("Failed to get IVector by IIterator", nullptr);

    IVector* cur_end;
    result = cur->getByIterator(cur_end_it, cur_end);
    if (result != ERR_OK || cur_end == nullptr)
      LOG_RET("Failed to get IVector by IIterator", nullptr);

    for (unsigned int coord = 0; coord < compDim; ++coord) {
      double begin_element;
      result = cur_begin->getCoord(coord, begin_element);
      if (result != ERR_OK)
        LOG_RET("Failed to get begin IVector coordinate: " + std::to_string(coord), nullptr);

      double end_element;
      result = cur_begin->getCoord(coord, end_element);
      if (result != ERR_OK)
        LOG_RET("Failed to get end IVector coordinate: " + std::to_string(coord), nullptr);

      bound_d[i] = (begin_element < end_element) == lesser ?
                    begin_element : end_element;
    }
  }

  IVector* bound = IVector::createVector(compDim, bound_d);
  IVector* _step =
      step == nullptr ? m_step->clone() : step->clone();

  return new Iterator_0(this, bound, _step);
}

int Compact_A::Intersection(const ICompact &c)
{
  return appendTerm(OPERATION_Intersection, c);
}

int Compact_A::Union(const ICompact &c)
{
  return appendTerm(OPERATION_Union, c);
}

int Compact_A::Difference(const ICompact &c)
{
  return appendTerm(OPERATION_Difference, c);
}

// (this + that) - (this & that)
int Compact_A::SymDifference(const ICompact &c)
{
  ICompact* this_clone = clone();
  if (this_clone == nullptr)
    return ERR_MEMORY_ALLOCATION;

  int unionReslut = this->Union(c);
  if (unionReslut != ERR_OK)
    LOG_RET("Failed to make Union", ERR_ANY_OTHER);

  int intersectionResult = this_clone->Intersection(c);
  if (intersectionResult != ERR_OK)
    LOG_RET("Failed to make Intersection", ERR_ANY_OTHER);

  int differenceResult = this->Difference(*this_clone);
  if (differenceResult != ERR_OK)
    LOG_RET("Failed to make Difference", ERR_ANY_OTHER);

  delete this_clone;
  return ERR_OK;
}

ICompact::IIterator* Compact_A::end(const IVector * const step)
{
  IIterator* end_it = getIterator(false, step);
  if (end_it == nullptr)
    LOG_RET("Failed to get greater bound iterator", nullptr);

  return end_it;
}

ICompact::IIterator* Compact_A::begin(const IVector * const step)
{
  IIterator* begin_it = getIterator(false, step);
  if (begin_it == nullptr)
    LOG_RET("Failed to get lesser bound iterator", nullptr);

  return begin_it;
}

Compact_A::Compact_A(const IVector * const step)
  : m_step(step)
{
  Q_ASSERT(m_step != nullptr);
}
