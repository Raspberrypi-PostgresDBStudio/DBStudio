/*
    CREATE TABLE COLUMN
    
    https://www.postgresqltutorial.com/postgresql-add-column/
    
    EXAMPLES:
    
    ALTER TABLE <schema>.Book ADD author VARCHAR(50);
    
    ALTER TABLE <schema>.Book RENAME COLUMN author TO book_author;
    
    ALTER TABLE <schema>.Book ALTER COLUMN book_author SET DEFAULT 'Nicholas Samuel';
    
    ALTER TABLE <schema>.Book ADD CHECK (book_author IN ('Nicholas', 'Samuel'));
*/

ALTER TABLE <schema>.<table>
ADD COLUMN column1 VARCHAR(100) NOT NULL DEFAULT 'foo';
