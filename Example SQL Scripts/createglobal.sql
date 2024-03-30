create database globaldata
    with
    owner pat 
    encoding = 'utf8'
    tablespace pg_default
    lc_collate = 'en_GB.UTF-8'
    lc_ctype = 'en_GB.UTF-8'
    connection limit = -1;

comment on database iba is 'Global Data';

\connect globaldata

create table public.tblpostcode
(
    postcode character varying(50) not null constraint pk_tblpostcode PRIMARY KEY,
    latitude numeric null,
    longitude numeric null
);

COPY tblpostcode(postcode, latitude, longitude)
FROM '/mnt/share/postcode.csv'
DELIMITER ',';
