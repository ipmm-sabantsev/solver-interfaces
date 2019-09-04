#include "common.h"
#include <logging.h>

namespace {
  int getBound(const IVector* const begin, const IVector* const end,
               IVector*& bound, bool left = true)
  {
    int result = DIMENSION_ERROR;
    unsigned int dim = begin->getDim();
    if (end->getDim() != dim)
      LOG_RET("begin and end dimensions are not equal", ERR_DIMENSIONS_MISMATCH);

    bound = begin->clone();

    for (unsigned int coord = 0; coord < dim; ++coord) {
      double begin_element;
      result = begin->getCoord(coord, begin_element);
      if (result != ERR_OK)
        LOG_RET("Failed to get begin IVector coordinate: " + std::to_string(coord), ERR_ANY_OTHER);

      double end_element;
      result = end->getCoord(coord, end_element);
      if (result != ERR_OK)
        LOG_RET("Failed to get end IVector coordinate: " + std::to_string(coord), ERR_ANY_OTHER);

      bound->setCoord(coord, (begin_element < end_element) == left ?
                              begin_element : end_element);
    }

    return ERR_OK;
  }
}

int absVector(const IVector* const step_in, IVector*&  step_out)
{
  unsigned int stepDim;
  const double* stepCoords;

  int result = step_in->getCoordsPtr(stepDim, stepCoords);
  if (result != ERR_OK)
    LOG_RET("Failed to get step coordinates", ERR_ANY_OTHER);

  step_out = step_in->clone();
  if (step_out == nullptr)
    LOG_RET("Failed to clone step_in", ERR_ANY_OTHER);

  for (unsigned int i = 0; i < stepDim; ++i)
    step_out->setCoord(i, std::abs(stepCoords[i]));

  return ERR_OK;
}

int leftBound(const IVector* const begin, const IVector* const end, IVector*& bound)
{
  return getBound(begin, end, bound, true);
}

int rightBound(const IVector* const begin, const IVector* const end, IVector*& bound)
{
  return getBound(begin, end, bound, false);
}
