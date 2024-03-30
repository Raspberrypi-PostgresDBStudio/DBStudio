/*
    CREATE TRIGGER
    
    https://www.postgresql.org/docs/current/sql-createtrigger.html
    
    EXAMPLE:
    
    CREATE TABLE <schema>.employees(
       id SERIAL PRIMARY KEY,
       first_name VARCHAR(40) NOT NULL,
       last_name VARCHAR(40) NOT NULL
    );

    CREATE TABLE <schema>.employee_audits (
       id SERIAL PRIMARY KEY,
       employee_id INT NOT NULL,
       last_name VARCHAR(40) NOT NULL,
       changed_on TIMESTAMP(6) NOT NULL
    );

    --First, define a new function called log_last_name_changes
    CREATE OR REPLACE FUNCTION <schema>.log_last_name_changes()
    RETURNS trigger AS
    $BODY$
    BEGIN
        IF NEW.last_name <> OLD.last_name THEN
             INSERT INTO <schema>.employee_audits(employee_id,last_name,changed_on)
             VALUES(OLD.id,OLD.last_name,now());
        END IF;

        RETURN NEW;
    END;
    $BODY$

    --Bind the trigger function to the employees table. The trigger name is last_name_changes. 
    --Before the value of the last_name column is updated, the trigger function is automatically invoked to log the changes.
    CREATE TRIGGER <schema>.last_name_changes
      BEFORE UPDATE
      ON <schema>.employees
      FOR EACH ROW
      EXECUTE PROCEDURE log_last_name_changes();
  
*/

CREATE TRIGGER <schema>.trigger_name 
{BEFORE | AFTER | INSTEAD OF} {event [OR ...]}
   ON <schema>.<table>
   [FOR [EACH] {ROW | STATEMENT}]
       EXECUTE PROCEDURE trigger_function
