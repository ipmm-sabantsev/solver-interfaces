##--------------------------
## Defines
##--------------------------

PRJ_ROOT = ..

SRC_ROOT = $$PRJ_ROOT/src
IMP_DIR  = $$SRC_ROOT/impl

INC_ROOT = $$PRJ_ROOT/inc

OUT_ROOT  = ../out

CONFIG(debug, debug|release) {
  DBG_RLS_SWITCH = debug
} else {
  DBG_RLS_SWITCH = release
}
