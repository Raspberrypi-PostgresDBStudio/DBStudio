/*
    IMPORT JSON FILE
    
    https://www.postgresqltutorial.com/postgresql-json/
    
    EXAMPLE:
    
    Json File
    {
    "EId":"104111",
    "Category":"(0)",
    "Mac":"ABV",
    "Path":"chemin2",
    "ID":"System.Byte"
    }
    {
    "EId":"104112",
    "Category":"(1)",
    "Mac":"CBV",
    "Path":"chemin3",
    "ID":"System.Byte"
    }

    CREATE TABLE public.customer
    (
        eid integer NULL,
        category character varying(100) NULL,
        mac character varying(100) NULL,
        path character varying(200) NOT NULL,
        id character varying(100) NOT NULL
    );

    BEGIN;

    CREATE TEMP TABLE temp_json (values text) ON COMMIT DROP;
    COPY temp_json FROM '<filename>';

    CREATE TEMP TABLE t (j jsonb) ON COMMIT DROP;

    INSERT INTO t (j)
    SELECT regexp_split_to_table(replace(v, $$"}{"$$, $$"}djue748wBc,l;09{"$$), 'djue748wBc,l;09')::jsonb
    FROM 
    (
        SELECT string_agg(values, '') as v
        FROM temp_json
    ) s;

    INSERT INTO public.customer (eid, category, mac, path, id) 
    SELECT CAST(j->>'EId' AS int) as eid,
           j->>'Category' as category,
           j->>'Mac' as mac  ,
           j ->> 'Path' as path,
           j ->> 'ID' as id
    FROM t
    WHERE j->>'EId' IS NOT NULL;

    COMMIT;
*/

