/*
    CREATE ROLE
    
    https://www.postgresql.org/docs/current/sql-createrole.html

    EXAMPLE:
    
    CREATE ROLE new_role INHERIT;
    
    GRANT PERMISSIONS
    
    https://www.postgresql.org/docs/current/sql-grant.html

    EXAMPLE:
    
    -- Grant connect to my data base
    GRANT CONNECT ON DATABASE db TO new_role;

    -- Grant usage the schema
    GRANT USAGE ON SCHEMA my_schema TO new_role;

    -- Grant all table for SELECT, INSERT, UPDATE, DELETE
    GRANT SELECT, INSERT, UPDATE, DELETE ON ALL TABLES IN SCHEMA my_schema TO new_role;

    -- Grant all future tables for SELECT, INSERT, UPDATE, DELETE
    ALTER DEFAULT PRIVILEGES IN SCHEMA my_schema
      GRANT SELECT, INSERT, UPDATE, DELETE ON tables TO new_role;

    -- Grant all future sequences for SELECT, USAGE
    ALTER DEFAULT PRIVILEGES IN SCHEMA public
      GRANT SELECT, USAGE ON sequences TO new_role;

    GRANT ALL ON FUNCTION my_func TO new_role;

    --ditto for other objects...

    --Add User To Role
    GRANT new_role TO new_user;

*/




