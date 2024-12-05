#ifndef VECTOR_H
#define VECTOR_H

static inline size_t vector_size(int dim) {
    return VARHDRSZ + sizeof(int) + sizeof(float) * dim;
}

typedef struct Vector{
	int32		vl_len_;
	int	dim;
	float x[FLEXIBLE_ARRAY_MEMBER];
} Vector;

#if PG_VERSION_NUM >= 160000
#define FUNCTION_PREFIX
#else
#define FUNCTION_PREFIX PGDLLEXPORT
#endif

#endif