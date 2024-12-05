EXTENSION = vector_extension
MODULE_big = vector_extension
OBJS = vector.o
HEADERS = vector.h
DATA = vector_extension--1.0.sql

PG_CONFIG = pg_config
PG_CONFIG = /usr/lib/postgresql/12/bin/pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)