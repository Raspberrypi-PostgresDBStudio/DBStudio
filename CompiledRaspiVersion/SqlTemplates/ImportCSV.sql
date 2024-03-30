/*
    IMPORT CSV FILE
    
    https://www.postgresqltutorial.com/import-csv-file-into-posgresql-table/
    
    EXAMPLE:
    
    CREATE TABLE persons
    (
      id serial NOT NULL,
      first_name character varying(50),
      last_name character varying(50),
      dob date,
      email character varying(255),
      CONSTRAINT pk_persons PRIMARY KEY (id)
    );
    
    COPY persons(id, first_name,last_name,dob,email) 
    FROM '<filename>' DELIMITER ',' CSV HEADER;
*/
