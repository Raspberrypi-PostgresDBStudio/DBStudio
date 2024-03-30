create table tblauthor 
(
    author_id integer not null constraint pk_tblauthor primary key generated by default as identity,
    forename character varying(200) null,
    surname character varying(200) null
);

create table tblpublisher 
(
    publisher_id int not null constraint pk_tblpublisher primary key generated by default as identity,
    publisher character varying(200) null
);

create table tblgenre
(
    genre_id int not null constraint pk_tblgenre primary key generated by default as identity,
    genre character varying(50) null
);

create table tblmedia
(
    media_id int not null constraint pk_tblmedia primary key generated by default as identity,
    media character varying(50) null
);

create table tblbook 
(
    book_id integer not null constraint pk_tblbook primary key generated by default as identity,
    author_id integer not null constraint fk_tblbook_author_id references tblauthor(author_id),
    publisher_id integer not null constraint fk_tblbook_publisher_id references tblpublisher(publisher_id),
    genre_id integer not null constraint fk_tblbook_genre_id references tblgenre(genre_id),
    media_id integer not null constraint fk_tblbook_media_id references tblmedia(media_id),
    title character varying(200) null,
    series character varying(200) not null default 'N/A',
    series_no integer null,
    date_read date null
);

insert into public.tblmedia(media) VALUES 
('Paper'),
('Kindle'),
('Audible');

insert into public.tblpublisher(publisher) VALUES ('Unknown');

insert into public.tblgenre(genre) VALUES ('Unknown');
