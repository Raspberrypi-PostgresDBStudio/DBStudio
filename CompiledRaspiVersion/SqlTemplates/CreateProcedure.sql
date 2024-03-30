/*
    CREATE PROCEDURE

    https://www.postgresql.org/docs/current/sql-createprocedure.html

    EXAMPLE:

    CREATE OR REPLACE PROCEDURE <schema>.control_transaction()
    AS $$
    DECLARE
    BEGIN
       CREATE TABLE test1 (id int);
       INSERT INTO test1 VALUES (1);
       COMMIT;
       CREATE TABLE test2 (id int);
       INSERT INTO test2 VALUES (1);
       ROLLBACK;
    END $$
    LANGUAGE PLPGSQL;
    
    call <schema>.control_transaction();

    select relname from pg_class where relname like '%test%';
*/
