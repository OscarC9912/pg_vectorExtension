DROP TABLE vectors;
DROP EXTENSION vector_extension;
-- DROP FUNCTION IF EXISTS vector_in(cstring) CASCADE;
-- DROP FUNCTION IF EXISTS vector_out(vector) CASCADE;
-- DROP FUNCTION IF EXISTS vector_typmod_in(cstring[]) CASCADE;
-- DROP FUNCTION IF EXISTS vector_eq(vector, vector) CASCADE;
-- DROP FUNCTION IF EXISTS vector_ne(vector, vector) CASCADE;
-- DROP FUNCTION IF EXISTS vector_add_vector(vector, vector) CASCADE;
-- DROP FUNCTION IF EXISTS vector_add_number(vector, float4) CASCADE;
-- DROP FUNCTION IF EXISTS vector_sub_vector(vector, vector) CASCADE;
-- DROP FUNCTION IF EXISTS vector_sub_number(vector, float4) CASCADE;
-- DROP FUNCTION IF EXISTS vector_dotproduct_vector(vector, vector) CASCADE;
-- DROP FUNCTION IF EXISTS vector_dot_number(vector, float4) CASCADE;
-- DROP FUNCTION IF EXISTS vector_div_number(vector, float4) CASCADE;
-- DROP FUNCTION IF EXISTS dim(vector) CASCADE;
-- DROP FUNCTION IF EXISTS norm(vector) CASCADE;
-- DROP FUNCTION IF EXISTS l2_distance(vector, vector) CASCADE;
-- DROP FUNCTION IF EXISTS cos_distance(vector, vector) CASCADE;
-- DROP TYPE IF EXISTS vector CASCADE;
-- DROP TYPE IF EXISTS neighbor_result CASCADE;
-- DROP ACCESS METHOD IF EXISTS vector_k_nearest_neighbor;

CREATE EXTENSION vector_extension;

CREATE TABLE vectors (id serial PRIMARY KEY, vec vector(5));

INSERT INTO vectors (vec) VALUES
    ('[1.0, 2.0, 3.0, 4.0, 5.0]'),
    ('[6.0, 7.0, 8.0, 9.0, 10.0]'),
    ('[11.0, 12.0, 13.0, 14.0, 15.0]');

INSERT INTO vectors (vec) VALUES
    ('[1.0, 2.0, 3.0, 4.0, 5.0]'),
    ('[6.0, 7.0, 8.0, 9.0, 10.0]'),
    ('[11.0, 12.0, 13.0, 14.0, 15.0]');

INSERT INTO vectors (vec) VALUES
    ('[1.0, 2.0, 3.0, 4.0, 5.0]'),
    ('[6.0, 7.0, 8.0, 9.0, 10.0]'),
    ('[11.0, 12.0, 13.0, 14.0, 15.0]');

INSERT INTO vectors (vec) VALUES
    ('[1.0, 2.0, 3.0, 4.0, 5.0]'),
    ('[6.0, 7.0, 8.0, 9.0, 10.0]'),
    ('[11.0, 12.0, 13.0, 14.0, 15.0]');

INSERT INTO vectors (vec) VALUES
    ('[1.0, 2.0, 3.0, 4.0, 5.0]'),
    ('[6.0, 7.0, 8.0, 9.0, 10.0]'),
    ('[11.0, 12.0, 13.0, 14.0, 15.0]');

INSERT INTO vectors (vec) VALUES
    ('[1.0, 2.0, 3.0, 4.0, 5.0]'),
    ('[6.0, 7.0, 8.0, 9.0, 10.0]'),
    ('[11.0, 12.0, 13.0, 14.0, 15.0]');

EXPLAIN ANALYZE SELECT * FROM vector_k_nearest_neighbor('[6.0, 7.0, 7.0, 9.0, 10.0]', 'vectors', 'vec', 2);