/*
    CREATE DOMAIN

    https://www.postgresql.org/docs/current/sql-createdomain.html

    EXAMPLE:

    CREATE DOMAIN <schema>.addr1 AS CHAR(1) NOT NULL DEFAULT 'N' CHECK(VALUE IN('Y', 'N', 'U'));

    --Do not allow null or space
    CREATE DOMAIN <schema>.contact_name AS 
    VARCHAR NOT NULL CHECK (value !~ '\s');
    GO

    CREATE TABLE <schema>.mail_list (
        id serial PRIMARY KEY,
        first_name contact_name,
        last_name contact_name,
        email VARCHAR NOT NULL
    );
    GO

    --generate error, space in first_name
    INSERT INTO <schema>.mail_list (first_name, last_name, email)
    VALUES
        (
            'Jame V',
            'Doe',
            'jame.doe@example.com'
        );
*/
