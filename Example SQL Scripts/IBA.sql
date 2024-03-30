create database iba
    with
    owner pat 
    encoding = 'utf8'
    tablespace pg_default
    lc_collate = 'en_GB.UTF-8'
    lc_ctype = 'en_GB.UTF-8'
    connection limit = -1;
go

comment on database iba is 'insurance brokerage application';
go

create connection hostaddr=127.0.0.1;port=5432; dbname=iba;user=pat;password=testing;
go

begin work;

create table public.tblclientstatus
(
    clientstatusid integer not null constraint pk_tblclientstatus primary key,
    clientstatus character varying(50) null
);

insert into public.tblclientstatus(clientstatusid, clientstatus) values (1, 'prospect');
insert into public.tblclientstatus(clientstatusid, clientstatus) values (2, 'live');
insert into public.tblclientstatus(clientstatusid, clientstatus) values (3, 'dead');

create table public.tblclient
(
    clientid integer not null constraint pk_tblclient primary key,
    clientname character varying(100) null,
    url character varying(200) null,
    statusid integer not null constraint fk_tblclient_statusid references tblclientstatus(clientstatusid),
    createdate date not null default current_date,
    vatregnumber character varying(50) null,
    businessdescription text null,
    companyreg character varying(8) null
);

create index idxtblclient_clientname on public.tblclient(clientname);

create table public.tblcontact
(
    contactid integer not null constraint pk_tblcontact primary key,
    title character varying(50) null,
    forename character varying(200) null,
    middlename character varying(200) null,
    surname character varying(200) null,
    formalsalutation character varying(200) null,
    informalsalutation character varying(200) null,
    fullname character varying(653) generated always as ((case when title is not null then (title || ' ') else '' end) || (case when forename is not null then (forename || ' ') else '' end) || (case when middlename is not null then (middlename || ' ') else '' end) || (case when surname is not null then (surname || ' ') else '' end)) stored,
    sortname character varying(653) generated always as ((case when surname is not null then (surname || ' ') else '' end) || (case when middlename is not null then (middlename || ' ') else '' end) || (case when forename is not null then (forename || ' ') else '' end) || (case when title is not null then (title || ' ') else '' end)) stored
);

create index idxtblcontact_title on public.tblcontact(title);
create index idxtblcontact_forename on public.tblcontact(forename);
create index idxtblcontact_surname on public.tblcontact(surname);

create table public.tblcontactorgtype
(
    contactorgtypeid integer not null constraint pk_tblcontactorgtype primary key,
    contactorgtype character varying(50) null
);

insert into public.tblcontactorgtype(contactorgtypeid, contactorgtype) values (1, 'n/a');
insert into public.tblcontactorgtype(contactorgtypeid, contactorgtype) values (2, 'client');
insert into public.tblcontactorgtype(contactorgtypeid, contactorgtype) values (3, 'supplier');
insert into public.tblcontactorgtype(contactorgtypeid, contactorgtype) values (4, 'professional advisor');
insert into public.tblcontactorgtype(contactorgtypeid, contactorgtype) values (5, 'accounts');

create table public.tblcontactlinktype
(
    contactlinktypeid integer not null constraint pk_tblcontactlinktype primary key,
    contactlinktype character varying(50) null
);

insert into public.tblcontactlinktype(contactlinktypeid, contactlinktype) values (1, 'client');

create table public.tblcontactlink
(
    contactlinkid integer not null constraint pk_tblcontactlink primary key,
    contactid integer not null constraint fk_tblcontactlink_contactid references tblcontact(contactid),
    linkedid integer not null,
    contactlinktypeid integer not null constraint fk_tblcontactlink_contactlinktypeid references tblcontactlinktype(contactlinktypeid),
    contactorgtypeid integer not null constraint fk_tblcontactlink_contactorgtypeid references tblcontactorgtype(contactorgtypeid),
    maincontact character(1) not null default 'n'::bpchar constraint tblcontactlink_maincontact check ((maincontact = any (array['y'::bpchar, 'n'::bpchar, 'u'::bpchar]))),
    password character varying(32) null
);

create table public.tblcontactgrouptype
(
    contactgrouptypeid integer not null constraint pk_tblcontactgrouptype primary key,
    contactgrouptype character varying(50) null
);

insert into public.tblcontactgrouptype(contactgrouptypeid, contactgrouptype) values (1, 'family');
insert into public.tblcontactgrouptype(contactgrouptypeid, contactgrouptype) values (2, 'business');
insert into public.tblcontactgrouptype(contactgrouptypeid, contactgrouptype) values (3, 'accountant');

create table public.tblcontactgroup
(
    contactgroupid integer not null constraint pk_tblcontactgroup primary key,
    contactgrouptypeid integer not null constraint fk_tblcontactgroup_contactgrouptypeid references tblcontactgrouptype(contactgrouptypeid),
    contactgroup character varying(200) null
);

create table public.tblcontactgrouplinktype
(
    contactgrouplinktypeid integer not null constraint pk_tblcontactgrouplinktype primary key,
    contactgrouplinktype character varying(50) null
);

insert into public.tblcontactgrouplinktype(contactgrouplinktypeid, contactgrouplinktype) values (1, 'contact');
insert into public.tblcontactgrouplinktype(contactgrouplinktypeid, contactgrouplinktype) values (2, 'group');

create table public.tblcontactgrouplink
(
    contactgrouplinkid integer not null constraint pk_tblcontactgrouplink primary key,
    contactgroupid integer not null constraint fk_tblcontactgrouplink_contactgroupid references tblcontactgroup(contactgroupid),
    usedbyid integer not null,
    contactgrouplinktypeid integer not null constraint fk_tblcontactgrouplink_contactgrouplinktypeid references tblcontactgrouplinktype(contactgrouplinktypeid)
);

create table public.tbllocationtype
(
    locationtypeid integer not null constraint pk_tbllocationtype PRIMARY KEY,
    locationtype character varying(50) null
);

insert into public.tbllocationtype(locationtypeid, locationtype) VALUES (1, 'Business');
insert into public.tbllocationtype(locationtypeid, locationtype) VALUES (2, 'Home');

create table public.tblusedby
(
    usedbyid integer not null constraint pk_tblusedby primary key,
    usedby character varying(50) null
);

insert into public.tblusedby(usedbyid, usedby) values (1, 'contact');

create table public.tbladdress
(
    addressid integer not null constraint pk_tbladdress PRIMARY KEY,
    address1 character varying(200) null,
    address2 character varying(200) null,
    address3 character varying(200) null,
    address4 character varying(200) null,
    address5 character varying(200) null,
    postcode character varying(50) null,
    organisation character varying(200) null,
    fulladdress character varying(1256) generated always as ((case when address1 is not null then (address1 || E'\n') else '' end) || (case when address2 is not null then (address2 || E'\n') else '' end) || (case when address3 is not null then (address3 || E'\n') else '' end) || (case when address4 is not null then (address4 || E'\n') else '' end) || (case when address5 is not null then (address5 || E'\n') else '' end) || (coalesce(postcode, ''))) stored,
    latitude numeric null,
    longitude numeric null
);

create index idxtbladdress_address1 on public.tbladdress(address1);
create index idxtbladdress_address2 on public.tbladdress(address2);
create index idxtbladdress_address3 on public.tbladdress(address3);
create index idxtbladdress_address4 on public.tbladdress(address4);
create index idxtbladdress_address5 on public.tbladdress(address5);
create index idxtbladdress_postcode on public.tbladdress(postcode);

create table public.tbladdresslink
(
    addresslinkid integer not null constraint pk_tbladdresslink PRIMARY KEY,
    addressid integer not null constraint fk_tbladdresslink_addressid references tbladdress(addressid),
    entitylinkedtoid integer not null,
    usedbyid integer not null constraint fk_tbladdresslink_usedbyid references tblusedby(usedbyid),
    locationtypeid integer not null constraint fk_tbladdresslink_locationtypeid references tbllocationtype(locationtypeid),
    main character(1) not null default 'Y'::bpchar check ((main = ANY (ARRAY['Y'::bpchar, 'N'::bpchar, 'U'::bpchar])))
);

create table public.tblemailaddress
(
    emailaddressid integer not null constraint pk_tblemailaddress PRIMARY KEY,
    emailaddress character varying(200) not null
);

create index idxtblemailaddress_emailaddress on public.tblemailaddress(emailaddress);

create table public.tblemailaddresslink
(
    emailaddresslinkid integer not null constraint pk_tblemailaddresslink PRIMARY KEY,
    emailaddressid integer not null constraint fk_tblemailaddresslink_emailaddressid references tblemailaddress(emailaddressid),
    entitylinkedtoid integer not null,
    usedbyid integer not null constraint fk_tblemailaddresslink_usedbyid references tblusedby(usedbyid),
    locationtypeid integer not null constraint fk_tblemailaddresslink_locationtypeid references tbllocationtype(locationtypeid),
    main character(1) not null default 'Y'::bpchar check ((main = ANY (ARRAY['Y'::bpchar, 'N'::bpchar, 'U'::bpchar])))
);

create table public.tbltelephonetype
(
    telephonetypeid integer not null constraint pk_tbltelephonetype PRIMARY KEY,
    telephonetype character varying(50) null
);

insert into public.tbltelephonetype(telephonetypeid, telephonetype) VALUES (1, 'Main');
insert into public.tbltelephonetype(telephonetypeid, telephonetype) VALUES (2, 'Mobile');
insert into public.tbltelephonetype(telephonetypeid, telephonetype) VALUES (3, 'Fax');

create table public.tbltelephone
(
    telephoneid integer not null constraint pk_tbltelephone PRIMARY KEY,
    telephonetypeid integer not null constraint fk_tbltelephone_telephonetypeid references tbltelephonetype(telephonetypeid),
    telephone character varying(50) null
);

create index idxtbltelephone_telephone on public.tbltelephone(telephone);

create table public.tbltelephonelink
(
    telephonelinkid integer not null constraint pk_tbltelephonelink PRIMARY KEY,
    telephoneid integer not null constraint fk_tbltelephonelink_telephoneid references tbltelephone(telephoneid),
    entitylinkedtoid integer not null,
    usedbyid integer not null constraint fk_tbltelephonelink_usedbyid references tblusedby(usedbyid),
    locationtypeid integer not null constraint fk_tbltelephonelink_locationtypeid references tbllocationtype(locationtypeid),
    main character(1) not null default 'Y'::bpchar check ((main = ANY (ARRAY['Y'::bpchar, 'N'::bpchar, 'U'::bpchar])))
);

commit work;
go

do $$
begin  
  if not exists(select * from pg_extension where extname='postgres_fdw') then
     CREATE EXTENSION postgres_fdw;

     CREATE SERVER GlobalData
     FOREIGN DATA WRAPPER postgres_fdw
     OPTIONS (dbname 'PadV3', host '192.168.1.213', port '5432');

     CREATE USER MAPPING FOR CURRENT_USER
     SERVER GlobalData
     OPTIONS (user 'pi', password 'TestPassword');

    IMPORT FOREIGN SCHEMA "public" limit to (tblpostcode) FROM SERVER GlobalData INTO public;

  end if;
end $$
go

refresh connections
go

use 127.0.0.1.iba
go

