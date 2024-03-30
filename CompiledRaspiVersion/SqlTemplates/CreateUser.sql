/*
    CREATE ROLE
    
    https://www.postgresql.org/docs/current/sql-createrole.html

    EXAMPLE:
    
    --Create Normal User
    CREATE USER new_user WITH LOGIN NOSUPERUSER NOCREATEDB NOCREATEROLE INHERIT NOREPLICATION CONNECTION LIMIT -1 ENCRYPTED PASSWORD 'xxxxxxx';

    --Create Admin User
    CREATE ROLE sa WITH SUPERUSER CREATEDB CREATEROLE LOGIN CONNECTION LIMIT -1 ENCRYPTED PASSWORD 'mysuperpass2';
    
    GRANT PERMISSIONS
    
    https://www.postgresql.org/docs/current/sql-grant.html

    EXAMPLE:
    
    -- Grant connect to my data base
    GRANT CONNECT ON DATABASE db TO new_user;

    -- Grant usage the schema
    GRANT USAGE ON SCHEMA my_schema TO new_user;

    -- Grant all table for SELECT, INSERT, UPDATE, DELETE
    GRANT SELECT, INSERT, UPDATE, DELETE ON ALL TABLES IN SCHEMA my_schema TO new_user;

    -- Grant all future tables for SELECT, INSERT, UPDATE, DELETE
    ALTER DEFAULT PRIVILEGES IN SCHEMA my_schema
      GRANT SELECT, INSERT, UPDATE, DELETE ON tables TO new_user;

    -- Grant all future sequences for SELECT, USAGE
    ALTER DEFAULT PRIVILEGES IN SCHEMA public
      GRANT SELECT, USAGE ON sequences TO new_user;

    GRANT ALL ON FUNCTION my_func TO new_user;

    --ditto for other objects...

    --Add User To Role
    GRANT my_role TO new_user;

*/




