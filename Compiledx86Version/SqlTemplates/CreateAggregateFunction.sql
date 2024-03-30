/*
    CUSTOM AGGREGATE FUNCTION
    
    https://www.postgresql.org/docs/current/sql-createaggregate.html
    
    EXAMPLE:
    
    For this example, we will use a simple data schema that only contains an amount column and id column to provide ordering.
    
    create table <schema>.entries(
      id serial primary key,
      amount float8 not null
    );
    
    select setseed(0);
    
    insert into <schema>.entries(amount)
    select (2000 * random()) - 1000
    from generate_series(1, 1000000);
    
    The greatest_running_total function doesn't exist, but PostgreSQL gives us the functionality to create our own aggregate functions. 
    In this case, our greatest_running_total aggregate should accept float8 values and return a float8.
    
    create function <schema>.grt_sfunc(agg_state point, el float8)
    returns point
    immutable
    language plpgsql
    as $$
    declare
      greatest_sum float8;
      current_sum float8;
    begin
      current_sum := agg_state[0] + el;
      if agg_state[1] < current_sum then
        greatest_sum := current_sum;
      else
        greatest_sum := agg_state[1];
      end if;

      return point(current_sum, greatest_sum);
    end;
    $$;    

    The point agg_state is used as a 2-element, zero-based array. agg_state[0] is the current sum; agg_state[1] is the greatest sum the aggregate has seen. 
    We simply add agg_state[0] and the current row value el to get the new current sum. 
    The new greatest sum is the greater of the old greatest sum (agg_state[1]) and the new current sum. Finally, 
    we return a new point value with the new current and greatest sums.

    Because our aggregate's internal state is of type point and the output of our aggregate is float8, 
    we need an aggregate final function that takes the final value of the aggregate's internal state and converts it to a float8.

    create function <schema>.grt_finalfunc(agg_state point)
    returns float8
    immutable
    strict
    language plpgsql
    as $$
    begin
      return agg_state[1];
    end;
    $$;
    
    Lastly, we have to create the aggregate by providing the state transition function, internal aggregate state type, and the final function.
    
    create aggregate <schema>.greatest_running_total (float8)
    (
        sfunc = grt_sfunc,
        stype = point,
        finalfunc = grt_finalfunc
    );
    
    Try our new function.
    
    select <schema>.greatest_running_total(amount order by id asc)
    from <schema>.entries;
*/
