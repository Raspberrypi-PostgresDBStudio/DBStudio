/*
    CREATE TYPE

    https://www.postgresql.org/docs/current/sql-createtype.html

    EXAMPLE:

    --Used to return multiple values from functions    
    CREATE TYPE <schema>.email_summary AS (
        id INT,
        first_name contact_name,
        email varchar
    ); 
    GO

    CREATE OR REPLACE FUNCTION <schema>.get_email_summary (f_id INT) 
        RETURNS email_summary AS 
    $$ 
    SELECT
        id,
        first_name,
        email
    FROM
        mail_list
    WHERE
        id = f_id ; 
    $$ 
    LANGUAGE SQL;
    GO

    SELECT * FROM <schema>.get_email_summary (2);
*/
