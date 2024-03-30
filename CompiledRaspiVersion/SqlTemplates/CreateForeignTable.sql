/*
	CREATE FOREIGN TABLE

	https://www.postgresql.org/docs/current/sql-createforeigntable.html

	EXAMPLE:

	--If Extension not already installed
	CREATE EXTENSION postgres_fdw;

	--If Not already exists
	CREATE SERVER anyname
	FOREIGN DATA WRAPPER postgres_fdw
	OPTIONS (dbname '<database>', host '192.168.1.213', port '5432');

	--If Not already exists
	CREATE USER MAPPING FOR CURRENT_USER
	SERVER anyname
	OPTIONS (user '<username>', password '<password>');

	--for each table to be created
	IMPORT FOREIGN SCHEMA "<host schema>" limit to (tblbankaccount) FROM SERVER anyname INTO <schema>;
*/
