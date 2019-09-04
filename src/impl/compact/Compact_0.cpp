#include "ICompact.h"
#include "common.h"

#include <logging.h>
#include <IVector.h>
#include <QScopedPointer>

namespace  {
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
                 const IVector* const vector,
                 const IVector* const step);
    protected:
      ~Iterator_0() override;

    private:
      Iterator_0(const Iterator_0& other) = delete;
      void operator=(const Iterator_0& other) = delete;

    /// \brief Internal variables
    private:
      const Compact_0* const m_parentCompact;
      const IVector*         m_curVector;
      IVector* const         m_step;
    };

    int deleteIterator(IIterator* pIter) override;
    int getByIterator(IIterator const* pIter, IVector*& pItem) const override;

    IIterator* end(IVector const* const step = nullptr) override;
    IIterator* begin(IVector const* const step = nullptr) override;

    int isContains(IVector const* const vec, bool& result) const override;
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
    unsigned int getDim() const;
  private:
    Iterator_0* getIterator(IVector const* const step, bool begin = true);

  /// \brief Internal variables
  public:
    static const double defaultStep;
  private:
    // m_begin[i] <= m_end[i]
    QScopedPointer<const IVector> m_begin;
    // m_begin[i] <= m_end[i]
    QScopedPointer<const IVector> m_end;
    // m_step[i] >= 0.0
    QScopedPointer<const IVector> m_step;
  };

}

const double Compact_0::defaultStep = 1e-3;

//double clamp(const double& v,
//             const double& min,
//             const double& max)
//{
//  if (v < min)
//    return min;
//  if (v > max)
//    return max;
//  return v;
//}
//
//bool intersect(const double &begin_1, const double &end_1,
//               const double &begin_2, const double &end_2,
//               double &begin_in, double &end_in)
//{
//  begin_in = std::nan("");
//  end_in   = std::nan("");
//
//  if (begin_1 > end_1)
//    return false;
//  if (begin_2 > end_2)
//    return false;
//
//  if (end_1 < begin_2)
//    return false;
//  if (end_2 < begin_1)
//    return false;
//
//  begin_in = clamp(begin_1, begin_2, end_2);
//  end_in   = clamp(end_1,   begin_2, end_2);
//
//  return true;
//}

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
      const double* tmpStep = new double[compDim];
      if (tmpStep == nullptr)
        LOG_RET("Failed to create temporary steps array", nullptr);

      std::fill(tmpStep, tmpStep + sizeof(tmpStep),
                Compact_0::defaultStep);

      m_step = IVector::createVector(compDim, tmpStep);
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

ICompact::IIterator* Compact_0::end(const IVector* const step)
{
  return getIterator(step, false);
}

ICompact::IIterator*Compact_0::begin(const IVector* const step)
{
  return getIterator(step, true);
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
  //std::unique_ptr<ICompact> other_clone(other->clone());
  //if (other_clone == nullptr) {
  //  LOG_RET("Failed to clone other compact");
  //  return ERR_MEMORY_ALLOCATION;
  //}
  //
  //std::unique_ptr<IVector> other_begin; {
  //  std::unique_ptr<IIterator> other_beginIterator(other_clone->begin());
  //  IVector* other_beginPtr;
  //  const int getOtherBegin =
  //      other_clone->getByIterator(other_beginIterator.get(), other_beginPtr);
  //  if (getOtherBegin != ERR_OK) {
  //    LOG_RET("Failed to get other begin vector");
  //    return ERR_ANY_OTHER;
  //  }
  //  other_begin = std::make_unique<IVector>(other_beginPtr);
  //}
  //
  //std::unique_ptr<IVector> other_end; {
  //  std::unique_ptr<IIterator> other_endIterator(other_clone->end());
  //  IVector* other_endPtr;
  //  const int getOtherend =
  //      other_clone->getByIterator(other_endIterator.get(), other_endPtr);
  //  if (getOtherend != ERR_OK) {
  //    LOG_RET("Failed to get other end vector");
  //    return ERR_ANY_OTHER;
  //  }
  //  other_end = std::make_unique<IVector>(other_endPtr);
  //}
  //
  //if (other_begin->getDim() != getDim() ||
  //    other_end->getDim()   != getDim()) {
  //  LOG_RET("other compcat was wrong dimesion");
  //  return ERR_DIMENSIONS_MISMATCH;
  //}
  //
  //bool other_begiContains;
  //bool other_endContains;

  LOG("Ill formated interface:");
  LOG("  1. begin() and end() methods should have const variation");
  LOG("  2. isSubSet result out param is absent");
  return ERR_NOT_IMPLEMENTED;
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

unsigned int Compact_0::getDim() const
{
  return m_begin->getDim();
}

Compact_0::Iterator_0*Compact_0::getIterator(const IVector* const step, bool begin)
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
