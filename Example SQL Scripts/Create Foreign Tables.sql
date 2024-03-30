--The function script is shown here and should be installed on the database that you want to script the tables as foreign tables for future use on a target server.

CREATE OR REPLACE FUNCTION script_foreign_tables(param_server text
 , param_schema_search text
 , param_table_search text, param_ft_prefix text) RETURNS SETOF text
AS 
$$
-- params: param_server: name of foreign data server
--        param_schema_search: wildcard search on schema use % for non-exact
--        param_ft_prefix: prefix to give new table in target database 
--                        include schema name if not default schema
-- example usage: SELECT script_foreign_tables('prod_server', 'ch01', '%', 'ch01.ft_');
  WITH cols AS 
   ( SELECT cl.relname As table_name, na.nspname As table_schema, att.attname As column_name
    , format_type(ty.oid,att.atttypmod) AS column_type
    , attnum As ordinal_position
      FROM pg_attribute att
      JOIN pg_type ty ON ty.oid=atttypid
      JOIN pg_namespace tn ON tn.oid=ty.typnamespace
      JOIN pg_class cl ON cl.oid=att.attrelid
      JOIN pg_namespace na ON na.oid=cl.relnamespace
      LEFT OUTER JOIN pg_type et ON et.oid=ty.typelem
      LEFT OUTER JOIN pg_attrdef def ON adrelid=att.attrelid AND adnum=att.attnum
     WHERE 
     -- only consider non-materialized views and concrete tables (relations)
     cl.relkind IN('v','r') 
      AND na.nspname LIKE $2 AND cl.relname LIKE $3 
       AND cl.relname NOT IN('spatial_ref_sys', 'geometry_columns'
          , 'geography_columns', 'raster_columns')
       AND att.attnum > 0
       AND NOT att.attisdropped 
     ORDER BY att.attnum )
        SELECT 'CREATE FOREIGN TABLE ' || $4  || table_name || ' ('
         || string_agg(quote_ident(column_name) || ' ' || column_type 
           , ', ' ORDER BY ordinal_position)
         || ')  
   SERVER ' || quote_ident($1) || '  OPTIONS (schema_name ''' || quote_ident(table_schema) 
     || ''', table_name ''' || quote_ident(table_name) || '''); ' As result        
FROM cols
  GROUP BY table_schema, table_name
$$ language 'sql';

/*
Setting up the Target database with postgres_fdw
The first step is to have a database with a postgres_fdw server and user setup. Basic steps are:
*/

--script to run on target server --
CREATE EXTENSION postgres_fdw;

CREATE SERVER book_server
 FOREIGN DATA WRAPPER postgres_fdw 
  OPTIONS (host 'localhost', port '5432', dbname 'postgis_in_action');
  
CREATE USER MAPPING FOR public SERVER 
  book_server
  OPTIONS (user 'book_guest', password 'whatever');

/*
Script tables and views as foreign tables
The next step is to run the function we described on the source server. For this exercise, we just want to script one schema and we want to give the tables a different name in case we want to create materialized views against these that have the original name.
*/

SELECT script_foreign_tables('book_server', 'ch01', '%', 'ch01.ft_');
