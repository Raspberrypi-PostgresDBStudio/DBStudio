/*
    CREATE MATERIALIZED VIEW

    https://www.postgresql.org/docs/current/sql-creatematerializedview.html

    EXAMPLE:

    CREATE MATERIALIZED VIEW <schema>.sales_summary AS
        SELECT
        seller_no,
        invoice_date,
        sum(invoice_amt)::numeric(13,2) as sales_amt
        FROM invoice
        WHERE invoice_date < CURRENT_DATE
        GROUP BY
        seller_no,
        invoice_date
        ORDER BY
        seller_no,
        invoice_date;

    CREATE UNIQUE INDEX <schema>.sales_summary_seller ON <schema>.sales_summary (seller_no, invoice_date);

    REFRESH MATERIALIZED VIEW <schema>.sales_summary;

    SELECT * FROM <schema>.sales_summary;
*/
