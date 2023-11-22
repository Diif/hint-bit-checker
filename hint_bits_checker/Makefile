# contrib/hint_bits_checker/Makefile

MODULE_big = hint_bits_checker

EXTENSION = hint_bits_checker
DATA = hint_bits_checker--1.0.sql
OBJS = hint_bits_checker.o

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/hint_bits_checker
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif