/*
    CREATE TABLE COLUMN CONSTRAINT
    
    https://www.postgresql.org/docs/current/ddl-constraints.html

    EXAMPLES:
    
    ALTER TABLE <schema>.<table> ADD CONSTRAINT constraint_name PRIMARY KEY (column_name);

    ALTER TABLE <schema>.<table> ADD CONSTRAINT constraint_name FOREIGN KEY (c1) REFERENCES parent_table (p1);

    ALTER TABLE <schema>.<table> ADD CONSTRAINT age_constraint CHECK (age >= 10);

    ALTER TABLE <schema>.<table> ADD CONSTRAINT v CHECK ((column_name = ANY (ARRAY['Y'::bpchar, 'N'::bpchar, 'U'::bpchar])));

    ALTER TABLE <schema>.<table> ADD CONSTRAINT constraint_name UNIQUE (column1, column2, ... column_n);

    ALTER TABLE <schema>.<table> ALTER COLUMN c SET NOT NULL;    
*/
