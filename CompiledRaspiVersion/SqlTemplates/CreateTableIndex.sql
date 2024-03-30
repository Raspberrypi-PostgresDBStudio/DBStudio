/*
    CREATE INDEX

    https://www.postgresql.org/docs/current/sql-createindex.html

    EXAMPLE:

    CREATE INDEX idx<table>_column_name ON <schema>.<table>
    (
        column_name [ASC | DESC] [NULLS {FIRST | LAST }],
        ...
    );

    CREATE UNIQUE INDEX idx<table>_column_name on <schema>.<table> (column_name);

    CREATE INDEX idx<table>_column_name on <schema>.<table> (conditional_expression);
*/
