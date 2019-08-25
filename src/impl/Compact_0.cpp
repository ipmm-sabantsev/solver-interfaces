#include "ICompact.h"

#include <logging.h>
#include <IVector.h>
//#include <memory>

namespace  {
  class Compact_0 : public ICompact {
  public:
    int getId() const override;

    class Iterator_0 : public IIterator
    {
    public:
      int doStep() override;
      int setStep(IVector const* const step = nullptr) override;

    protected:
      ~Iterator_0() override;

    private:
      Iterator_0(const Iterator_0& other) = delete;
      void operator=(const Iterator_0& other) = delete;

      const ICompact* const m_parentCompact;
      const IVector* m_curVector;
      IVector* const m_step;
    };

    int deleteIterator(IIterator * pIter) override;
    int getByIterator(IIterator const* pIter, IVector*& pItem) const override;

    IIterator* end(IVector const* const step = nullptr) override;
    IIterator* begin(IVector const* const step = nullptr) override;

    int isContains(IVector const* const vec, bool& result) const override;
    int isSubSet(ICompact const* const other) const override;

    int getNearestNeighbor(IVector const* vec, IVector *& nn) const override;

    ICompact* clone() const override;

    Compact_0(
        const IVector* const begin,
        const IVector* const end,
        const IVector* const step
        );
    ~Compact_0() override;

    static const double defaultStep;

  protected:
    unsigned int getDim() const;

  private:
    const IVector* const m_begin;
    const IVector* const m_end;
    const IVector* const m_step;
  };
}

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

int Compact_0::getId() const
{
  return ICompact::InterfaceTypes::INTERFACE_0;
}

int Compact_0::isContains(const IVector * const vec, bool &result) const
{
  if (!vec) {
    LOG("vector was nullptr");
    return ERR_WRONG_ARG;
  }

  unsigned int compDim = getDim();
  if (vec->getDim() != compDim) {
    LOG("vector was wrong dimension");
    return ERR_DIMENSIONS_MISMATCH;
  }

  for(unsigned int i = 0; i < compDim; ++i) {
    double vec_elem;
    if (vec->getCoord(i, vec_elem) != ERR_OK) {
      LOG("Failed to get vector element");
      return ERR_OUT_OF_RANGE;
    }

    double begin_elem;
    if (m_begin->getCoord(i, begin_elem) != ERR_OK) {
      LOG("Failed to get begin element");
      return ERR_OUT_OF_RANGE;
    }

    double end_elem;
    if (m_begin->getCoord(i, end_elem) != ERR_OK) {
      LOG("Failed to get begin element");
      return ERR_OUT_OF_RANGE;
    }

    if (vec_elem < begin_elem ||
        vec_elem > end_elem) {
      result = false;
      return ERR_OK;
    }
  }

  result = true;
  return ERR_OK;
}

int Compact_0::isSubSet(const ICompact * const other) const
{
  //std::unique_ptr<ICompact> other_clone(other->clone());
  //if (other_clone == nullptr) {
  //  LOG("Failed to clone other compact");
  //  return ERR_MEMORY_ALLOCATION;
  //}
  //
  //std::unique_ptr<IVector> other_begin; {
  //  std::unique_ptr<IIterator> other_beginIterator(other_clone->begin());
  //  IVector* other_beginPtr;
  //  const int getOtherBegin =
  //      other_clone->getByIterator(other_beginIterator.get(), other_beginPtr);
  //  if (getOtherBegin != ERR_OK) {
  //    LOG("Failed to get other begin vector");
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
  //    LOG("Failed to get other end vector");
  //    return ERR_ANY_OTHER;
  //  }
  //  other_end = std::make_unique<IVector>(other_endPtr);
  //}
  //
  //if (other_begin->getDim() != getDim() ||
  //    other_end->getDim()   != getDim()) {
  //  LOG("other compcat was wrong dimesion");
  //  return ERR_DIMENSIONS_MISMATCH;
  //}
  //
  //bool other_begiContains;
  //bool other_endContains;

  LOG("Ill formated interface [method should be const, result output is absent]");
  return ERR_NOT_IMPLEMENTED;
}

ICompact* Compact_0::clone() const
{
  return createCompact(m_begin, m_end, m_step);
}

Compact_0::Compact_0(
    const IVector * const begin,
    const IVector * const end,
    const IVector * const step)
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
  if (m_begin != nullptr)
    delete(m_begin);
  if (m_end != nullptr)
    delete(m_end);
  if (m_step != nullptr)
    delete(m_step);
}

unsigned int Compact_0::getDim() const
{
  return m_begin->getDim();
}

const double Compact_0::defaultStep = 1.0;

ICompact* ICompact::createCompact(const IVector *const begin, const IVector *const end, const IVector *const step)
{
  if (begin == nullptr) {
    LOG("begin was nullptr");
    return nullptr;
  }
  if(end == nullptr) {
    LOG("end was nullptr");
    return nullptr;
  }

  const unsigned int compDim = begin->getDim();
  if (end->getDim() != compDim) {
    LOG("Wrong dimesions");
    return nullptr;
  }

  const IVector* _step = step;

  if (_step == nullptr) {
    const double* tmpStep = new double[compDim];
    if (tmpStep == nullptr) {
      LOG("Failed to create temporary steps array");
      return nullptr;
    }
    std::fill(tmpStep, tmpStep + sizeof(tmpStep),
              Compact_0::defaultStep);

    IVector::createVector(compDim, tmpStep);
  }

  const IVector* _begin = begin->clone();
  if (_begin == nullptr) {
    LOG("Failed to clone begin");
    return nullptr;
  }
  const IVector* _end = end->clone();
  if (_end == nullptr) {
    LOG("Failed to clone end");
    return nullptr;
  }

  return new Compact_0(_begin, _end, _step);
}
