/*
    DROP ROLE
    
    https://www.postgresql.org/docs/current/sql-droprole.html
    
    A role cannot be removed if it is still referenced in any database of the cluster; 
    an error will be raised if so. 

    https://www.postgresql.org/docs/current/sql-revoke.html
    https://www.postgresql.org/docs/current/sql-reassign-owned.html
    https://www.postgresql.org/docs/current/sql-drop-owned.html
    
    Before dropping the role, you must drop all the objects it owns (or reassign their ownership) and 
    revoke any privileges the role has been granted. The REASSIGN OWNED and DROP OWNED commands can be useful for this purpose.
    
    EXAMPLE:
    
    DROP ROLE [IF EXISTS] <role>;
*/

DROP ROLE <role>;
