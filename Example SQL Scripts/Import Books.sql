insert into tblauthor(forename, surname)
select *
from
(
select split_part(author, ' ', 1) as forename, split_part(author, ' ', 2) as surname from public.kindle
union all
select trim(BOTH FROM author_forename), trim(BOTH FROM author_surname) from public.bookread
) A
group by 1, 2
order by 2, 1
go

insert into public.tblbook(author_id, publisher_id, genre_id, media_id, title, date_read)
select A.author_id, 1, 1, 1, B.book, B.dateread
from tblauthor A join bookread B on A.forename = trim(BOTH FROM B.author_forename) and A.surname = trim(BOTH FROM author_surname)
order by B.dateread
go

insert into public.tblbook(author_id, publisher_id, genre_id, media_id, title)
select A.author_id, 1, 1, 2, K.book
from tblauthor A join kindle K on A.forename = split_part(K.author, ' ', 1) and A.surname = split_part(K.author, ' ', 2)
go
