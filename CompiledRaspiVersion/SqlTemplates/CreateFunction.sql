/*
    CREATE FUNCTION

    https://www.postgresql.org/docs/current/sql-createfunction.html

    EXAMPLE:

    CREATE OR REPLACE FUNCTION <schema>.inc(val integer) 
    RETURNS integer AS $$
    BEGIN
    RETURN val + 1;
    END; $$
    LANGUAGE PLPGSQL;

    SELECT <schema>.inc(20);
*/
