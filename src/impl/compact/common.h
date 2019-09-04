#ifndef COMPACT_COMMON_H_
#define COMPACT_COMMON_H_

#include <QScopedPointer>
#include <IVector.h>

int absVector(const IVector* const step_in, IVector*& step_out);
int leftBound(const IVector* const begin, const IVector* const end,
              IVector*& bound);
int rightBound(const IVector* const begin, const IVector* const end,
               IVector*& bound);

#endif // COMPACT_COMMON_H_
