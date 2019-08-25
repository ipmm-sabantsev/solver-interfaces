#include "ICompact.h"

#include <windows.h>
#include <logging.h>
#include <IVector.h>

EXTERN_C bool WINAPI DllMain()
{
  int logInit = ILog::init("log/compact.log");
  if (logInit != ERR_OK)
    return false;
  return true;
}

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

    ICompact* clone() const override;

    ~Compact_0() override;

  protected:
    Compact_0(
        const IVector* const begin,
        const IVector* const end,
        const IVector* const step
        );

  private:
    const IVector* const m_begin;
    const IVector* const m_end;
    const IVector* const m_step;
  };
}

int Compact_0::getId() const
{
  return ICompact::InterfaceTypes::INTERFACE_0;
}

ICompact* Compact_0::clone() const
{
  const IVector* const begin = m_begin->clone();
  const IVector* const end   = m_end->clone();
  const IVector* const step  = m_step->clone();

  return createCompact(begin, end, step);
}

Compact_0::~Compact_0()
{
  if (m_begin)
    delete(m_begin);
  if (m_end)
    delete(m_end);
  if (m_step)
    delete(m_step);
}

Compact_0::Compact_0(
    const IVector * const begin,
    const IVector * const end,
    const IVector * const step)
  : m_begin(begin),
    m_end(end),
    m_step(step)
{

}

ICompact* ICompact::createCompact(const IVector *const begin, const IVector *const end, const IVector *const step)
{
  if (!begin || !end || !step)
    return nullptr;

  return new Compact_0(begin, end, step);
}
