/*
    IMPORT JSON STRING

    EXAMPLE:
    
    with customer_json (doc) as (
       values 
        ('[
          {
            "id": 23635,
            "name": "Jerry Green",
            "comment": "Imported from facebook."
          },
          {
            "id": 23636,
            "name": "John Wayne",
            "comment": "Imported from facebook."
          }
        ]'::json)
    )
    INSERT INTO customer (id, name, comment)
    SELECT p.*
    FROM customer_json l
    CROSS JOIN LATERAL json_populate_recordset(null::customer, doc) as p
    ON conflict (id) do update 
    SET name = excluded.name, 
    comment = excluded.comment;
*/
