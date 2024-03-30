#include "Sql.h"

namespace sql
{
    std::string getDatabaseList(void)
    {
        return "select d.datname as db from pg_catalog.pg_database d order by 1;";
    }
    std::string getDatabaseComment(const Glib::ustring& database)
    {
        std::stringstream ss;
        ss << "select description from pg_shdescription \
               join pg_database on objoid = pg_database.oid \
               where datname = '" << database << "';";
        return ss.str();
    }
    
    std::string getServerVersion(void)
    {
        return "select cast(current_setting('server_version_num') as integer) as version;";
    }
    
    std::string getSchema(void)
    {
        return "select n.nspname as schema \
               from pg_catalog.pg_namespace n \
               where n.nspname !~ '^pg_' and n.nspname <> 'information_schema' \
               order by 1;";
    }

    std::string getSchemaComment(const Glib::ustring& schema)
    {
        std::stringstream ss;
        ss << "select obj_description('" << schema << "'::regnamespace, 'pg_namespace');";
        return ss.str();
    }
    
    std::string getTable(const Glib::ustring& schema)
    {
        std::stringstream ss;
        ss << "select c.relname as Name from pg_catalog.pg_class c \
               left join pg_catalog.pg_namespace n on n.oid = c.relnamespace \
               where c.relkind in('r','p','') \
               and n.nspname = '" << schema << "' \
               order by 1;";
        return ss.str();
    }
    
    std::string getForeignTable(const Glib::ustring& schema)
    {
        std::stringstream ss;
        ss << "select c.relname as Name from pg_catalog.pg_class c \
               left join pg_catalog.pg_namespace n on n.oid = c.relnamespace \
               where c.relkind in('f') \
               and n.nspname = '" << schema << "' \
               order by 1;";
        return ss.str();
    }
    
    std::string getEntityComment(const Glib::ustring& schema, const Glib::ustring& entity)
    {
        std::stringstream ss;
        ss << "select description from pg_description \
               join pg_class on pg_description.objoid = pg_class.oid \
               join pg_namespace on pg_class.relnamespace = pg_namespace.oid \
               where relname = '" << entity << "' and nspname='" << schema << "'";
        return ss.str();
    }

    std::string getColumn(const Glib::ustring& schema, const Glib::ustring& table)
    {
        std::stringstream ss;
        
        ss << "select a.attname, pg_catalog.format_type(a.atttypid, a.atttypmod), a.attnotnull \
                from pg_attribute a join pg_class t on a.attrelid = t.oid \
                join pg_namespace s on t.relnamespace = s.oid \
                where a.attnum > 0 and not a.attisdropped \
                and t.relname = '" << table << "' \
                and s.nspname = '" << schema << "' \
                order by a.attnum;";
        
        return ss.str();
    }
    
    std::string getTableColumnComment(const Glib::ustring& schema, const Glib::ustring& entity, const Glib::ustring& column)
    {
        std::stringstream ss;
        ss << "select pgd.description, c.column_name \
                from pg_description pgd join pg_class pcl on pgd.objoid = pcl.oid \
                join pg_namespace n on pcl.relnamespace = n.oid \
                join information_schema.columns c on (pgd.objsubid=c.ordinal_position and c.table_schema= n.nspname and c.table_name=pcl.relname) \
                where c.table_schema = '" << schema << "' and c.table_name = '" << entity << "' and c.column_name = '" << column << "';";
        
        return ss.str();
    }
    
    std::string getTableColumnDDL(const Glib::ustring& schema, const Glib::ustring& table, const Glib::ustring& column)
    {
        std::stringstream ss;
        ss << "select data_type, character_maximum_length, is_nullable, column_default, case when is_generated = 'ALWAYS' then concat('generated always as ', generation_expression, ' stored') else '' end as generated \
              from information_schema.columns \
              where (table_schema, table_name) = ('" << schema << "', '" << table << "')  and column_name = '" << column << "';";
        
        return ss.str();
    }

    std::string getTableConstraint(const Glib::ustring& schema, const Glib::ustring& table)
    {
        std::stringstream ss;
        ss << "select con.conname \
               from pg_catalog.pg_constraint con \
               join pg_catalog.pg_class rel on rel.oid = con.conrelid \
               join pg_catalog.pg_namespace nsp on nsp.oid = connamespace \
               where nsp.nspname = '" << schema << "' and rel.relname = '" << table << "' and con.contype in('c', 'p', 'u') \
               order by 1;";
        return ss.str();
    }

    std::string getTableConstraintDDL(const Glib::ustring& schema, const Glib::ustring& table, const Glib::ustring& constraint)
    {
        std::stringstream ss;
        ss << "select con.conname, pg_get_constraintdef(con.oid) as constrainddef \
               from pg_catalog.pg_constraint con \
               join pg_catalog.pg_class rel on rel.oid = con.conrelid \
               join pg_catalog.pg_namespace nsp on nsp.oid = connamespace \
               where nsp.nspname ='" << schema << "' and rel.relname = '" << table << "' and con.conname = '" << constraint << "';";
        return ss.str();
    }

    std::string getTableForeignKey(const Glib::ustring& schema, const Glib::ustring& table)
    {
        std::stringstream ss;
        ss << "select con.conname \
               from pg_catalog.pg_constraint con \
               join pg_catalog.pg_class rel on rel.oid = con.conrelid \
               join pg_catalog.pg_namespace nsp on nsp.oid = connamespace \
               where nsp.nspname = '" << schema << "' and rel.relname = '" << table << "' and con.contype = 'f' \
               order by 1;";
        return ss.str();
    }

    std::string getIndex(const Glib::ustring& schema, const Glib::ustring& table)
    {
        std::stringstream ss;
        ss << "select indexname \
               from pg_indexes \
               where schemaname = '" << schema << "' and tablename = '" << table << "' \
               order by 1;";
        return ss.str();
    }

    std::string getIndexDDL(const Glib::ustring& schema, const Glib::ustring& index)
    {
        std::stringstream ss;
        ss << "select indexdef \
               from pg_indexes \
               where schemaname = '" << schema << "' and indexname = '" << index << "';";
        return ss.str();
    }

    std::string getTableReferences(const Glib::ustring& table)
    {
        std::stringstream ss;
        ss << "select c.conname \
               from pg_constraint c \
               where c.confrelid = (select oid from pg_class where relname = '" << table << "') \
               order by 1";
        return ss.str();
    }

    std::string getTrigger(const Glib::ustring& schema, const Glib::ustring& table)
    {
        std::stringstream ss;
        ss << "select trigger_name from information_schema.triggers where trigger_schema = '" << schema << "' and event_object_table = '" << table << "' order by 1;";
        return ss.str();
    }

    std::string getTriggerDDL(const Glib::ustring& schema, const Glib::ustring& trigger)
    {
        std::stringstream ss;
               
        ss << "select trigger_schema, trigger_name, event_object_schema, event_object_table, action_condition, action_statement, action_orientation, action_timing, \
               array_to_string(array_agg(event_manipulation), ' OR ') as event \
               from information_schema.triggers \
               where trigger_schema = '" << schema << "' and trigger_name = '" << trigger << "' \
               group by trigger_schema, trigger_name, event_object_schema, event_object_table, action_condition, action_statement, action_orientation, action_timing;";       
               
        return ss.str();
    }

    std::string getTablePrimaryKey(const Glib::ustring& schema, const Glib::ustring& table)
    {
        std::stringstream ss;
        ss << "select a.attname, format_type(a.atttypid, a.atttypmod) as data_type \
               from pg_index i join   pg_attribute a on a.attrelid = i.indrelid  and a.attnum = ANY(i.indkey) \
               where i.indrelid = '" << schema << "." << table << "'::regclass \
               and i.indisprimary;";
        return ss.str();
    }

    std::string getTableCreate(const Glib::ustring& schema, const Glib::ustring& table)
    {
        std::stringstream ss;
 
        ss << "select 'create table ' || table_schema || '.' || table_name || E'\n(\n' || \
                array_to_string(array_agg('    ' || column_name || ' ' ||  data_type || ' '|| not_null || column_default || generated) , E',\n') || E'\n);\n' \
                from \
                ( \
                select table_schema, table_name, column_name, \
                concat(data_type, case when character_maximum_length is not null then concat('(', character_maximum_length, ')') else '' end) as data_type, is_nullable, \
                case when is_nullable = 'NO' then 'not null' else 'null' end as not_null, \
                case when column_default is null then '' else concat(' default ', column_default) end as column_default, \
                case when is_generated = 'ALWAYS' then concat(' generated always as ', generation_expression, ' stored') else '' end as generated \
                from information_schema.columns \
                where (table_schema, table_name) = ('" << schema << "', '" << table << "') \
                order by ordinal_position \
                ) as tabledefinition \
                group by table_schema, table_name";

        return ss.str(); 
    }

    std::string getTableCreateConstraint(const Glib::ustring& schema, const Glib::ustring& table)
    {
        std::stringstream ss;
        ss << "select con.conname,pg_get_constraintdef(con.oid) as constrainddef \
                from pg_catalog.pg_constraint con \
                join pg_catalog.pg_class rel on rel.oid = con.conrelid \
                join pg_catalog.pg_namespace nsp on nsp.oid = connamespace \
                where nsp.nspname = '" << schema << "' and rel.relname = '" << table << "' and con.contype IN('c', 'p', 'f') \
                order by 1;";
        return ss.str(); 
    }

    std::string getTableCreateIndex(const Glib::ustring& schema, const Glib::ustring& table)
    {
        std::stringstream ss;
        ss << "select indexdef \
               from pg_indexes i \
               where i.schemaname = '" << schema << "' and i.tablename = '" << table << "' and indexdef not LIKE 'CREATE UNIQUE INDEX%'";
        return ss.str(); 
    }

    std::string getView(const Glib::ustring& schema)
    {
        std::stringstream ss;
        ss << "select c.relname as views \
               from pg_catalog.pg_class c left join pg_catalog.pg_namespace n on n.oid = c.relnamespace \
               where c.relkind IN ('v','') and n.nspname = '" << schema << "' and pg_catalog.pg_table_is_visible(c.oid) \
               order by 1;";
        return ss.str();
    }
    
    std::string getViewCreate(const Glib::ustring& schema, const Glib::ustring& view)
    {
        std::stringstream ss;
        ss << "select 'create or replace view " << schema << "." << view << "  as ' as def, \
              A.definition \
              from \
              ( \
                select definition from pg_views where viewname = '" << view << "' and schemaname = '" << schema << "' \
              ) A;";
        return ss.str(); 
    }

    std::string getMaterializedView(const Glib::ustring& schema)
    {
        std::stringstream ss;
        ss << "select c.relname as views \
               from pg_catalog.pg_class c left join pg_catalog.pg_namespace n on n.oid = c.relnamespace \
               where c.relkind IN ('m','') and n.nspname = '" << schema << "' and pg_catalog.pg_table_is_visible(c.oid) \
               order by 1;";
        return ss.str();
    }

    std::string getMaterializedViewCreate(const Glib::ustring& schema, const Glib::ustring& view)
    {
        std::stringstream ss;
        ss << "select 'create materialized view " << schema << "." << view << "  as ' as def, \
              A.pggetviewdef \
              from \
              ( \
                select pg_get_viewdef('" << schema << "." << view << "') as pggetviewdef \
              ) A;";
        return ss.str(); 
    }

    std::string getIndex(const Glib::ustring& schema)
    {
        std::stringstream ss;
        ss << "select indexname \
               from pg_indexes \
               where schemaname = '" << schema << "' \
               order by 1;";
        return ss.str();
    }

    std::string getFunction(const Glib::ustring& schema)
    {
        std::stringstream ss;
        ss << "select p.proname as funcName \
               from pg_catalog.pg_proc p left join pg_catalog.pg_namespace n on n.oid = p.pronamespace \
               where pg_catalog.pg_function_is_visible(p.oid) and n.nspname = '" << schema << "' and p.prosrc not like '%aggregate_dummy%' \
               order by 1;";
        return ss.str();
    }

    std::string getFunctionComment(const Glib::ustring& schema, const Glib::ustring& func)
    {
        std::stringstream ss;
        ss << "select d.description \
               from pg_proc p \
               left join pg_description d  on d.objoid = p.oid \
               left join pg_namespace s on s.oid = p.pronamespace \
               where p.proname ='" << func << "' and s.nspname = '" << schema << "';";
        return ss.str();
    }
    
    std::string getFunctionParameter(const Glib::ustring& schema, const Glib::ustring& func)
    {
        std::stringstream ss;
        ss << "select pg_catalog.pg_get_function_arguments(p.oid) as ArgumentDataTypes \
               from pg_catalog.pg_proc p left join pg_catalog.pg_namespace n on n.oid = p.pronamespace \
               where pg_catalog.pg_function_is_visible(p.oid) and n.nspname = '" << schema << "' and p.proname = '" << func << "' and pg_catalog.pg_get_function_arguments(p.oid) != '' \
               order by 1;";
        return ss.str();
    }

    std::string getFunctionReturn(const Glib::ustring& schema, const Glib::ustring& func)
    {
        std::stringstream ss;
        ss << "select pg_catalog.pg_get_function_result(p.oid) as ResultDataType \
               from pg_catalog.pg_proc p left join pg_catalog.pg_namespace n on n.oid = p.pronamespace \
               where pg_catalog.pg_function_is_visible(p.oid) and n.nspname = '" << schema << "' and p.proname = '" << func << "' \
               order by 1;";
        return ss.str();
    }

    std::string getFunctionCreate(const Glib::ustring& schema, const Glib::ustring& func)
    {
        std::stringstream ss;
        ss << "select pg_get_function_arguments(p.oid) as arguments, \
               text(p.prorettype::regtype) as returns, \
               p.prosrc as definition, \
               l.lanname as language, \
               case p.provolatile \
                    when 'i'::\"char\" then 'IMMUTABLE'::text \
                    when 's'::\"char\" then 'STABLE'::text \
                    when 'v'::\"char\" then 'VOLATILE'::text \
                    else null::text \
                end as attributes, \
            pg_description.description as comment, \
            p.oid::regprocedure::text as sql_identifier \
            from pg_proc p \
            left join pg_namespace s on s.oid = p.pronamespace \
            left join pg_language l on l.oid = p.prolang \
            left join pg_roles u on p.proowner = u.oid \
            left join pg_description on p.oid = pg_description.objoid \
            where s.nspname = '" << schema << "' and p.proname = '" << func << "';";
        return ss.str();
    }
    
    std::string getSequence(const Glib::ustring& schema)
    {
        std::stringstream ss;
        ss << "select c.relname sequence_name from pg_class c \
               left join pg_catalog.pg_namespace n on n.oid = c.relnamespace \
               where relkind = 'S' and n.nspname = '" << schema << "' \
               order by 1;";
        return ss.str();
    }

    std::string getDataType(const Glib::ustring& schema)
    {
        std::stringstream ss;
        ss << "select pg_catalog.format_type(t.oid, null) as type \
               from pg_catalog.pg_type t left join pg_catalog.pg_namespace n on n.oid = t.typnamespace \
               where (t.typrelid = 0 or \
               (\
               select c.relkind = 'c' \
               from pg_catalog.pg_class c \
               where c.oid = t.typrelid \
               )) and not exists(select 1 from pg_catalog.pg_type el where el.oid = t.typelem and el.typarray = t.oid) \
               and pg_catalog.pg_type_is_visible(t.oid) and n.nspname = '" << schema << "' \
               order by 1;";
        return ss.str();
    }

    std::string getDataTypeComment(const Glib::ustring& schema, const Glib::ustring& datatype)
    {
        std::stringstream ss;
        ss << "select pg_catalog.obj_description(t.oid, 'pg_type') as comment \
               from pg_catalog.pg_type t left join pg_catalog.pg_namespace n on n.oid = t.typnamespace \
               where n.nspname = '" << schema << "' and pg_catalog.format_type(t.oid, null) = '" << datatype << "'";
        return ss.str();
    }
    
    std::string getAggregateFunction(const Glib::ustring& schema)
    {
        std::stringstream ss;
        ss << "select p.proname as agg \
               from pg_catalog.pg_proc p left join pg_catalog.pg_namespace n on n.oid = p.pronamespace \
               where p.prokind = 'a' and n.nspname = '" << schema << "' and pg_catalog.pg_function_is_visible(p.oid) \
               order by 1;";
        return ss.str();
    }
    
    std::string getAggregateFunctionDDL(const Glib::ustring& schema, const Glib::ustring& func)
    {
        std::stringstream ss;
        ss << "select \
                format(\
                E'create aggregate %s (\n%s\n);'\
                , (pg_identify_object('pg_proc'::regclass, aggfnoid, 0)).identity \
                , array_to_string(\
                array[\
                format(E'\tsfunc = %s', aggtransfn::regproc)\
                , format(E'\tstype = %s', format_type(aggtranstype, null))\
                , case aggfinalfn when '-'::regproc then null else format(E'\tfinalfunc = %s',aggfinalfn::text) end\
                , case aggsortop when 0 then null else format(E'\tSORTOP = %s', oprname) end\
                , case when agginitval IS null then null else format(E'\tinitcond = %s', agginitval) end\
                ]\
                , E',\n'\
                )\
                )\
                from pg_aggregate \
                left join pg_operator on pg_operator.oid = aggsortop \
                where aggfnoid = '" << schema << "." << func << "'::regproc;";
        return ss.str();
    }
    
    std::string getAggregateFunctionDrop(const Glib::ustring& schema, const Glib::ustring& func)
    {
        std::stringstream ss;
        ss << "select (pg_identify_object('pg_proc'::regclass, aggfnoid, 0)).identity \
                from pg_aggregate \
                where aggfnoid = '" << schema << "." << func << "'::regproc;";
        return ss.str();
    }
    
    std::string getAggregateFunctionComment(const Glib::ustring& schema, const Glib::ustring& func)
    {
        std::stringstream ss;
        ss << "select d.description \
               from pg_aggregate p \
               left join pg_description d  on d.objoid = p.aggfnoid \
               where aggfnoid = '" << schema << "." << func << "'::regproc;";
        return ss.str();
    }
    
    std::string getDataTypeDDL(const Glib::ustring& schema, const Glib::ustring& datatype)
    {
        std::stringstream ss;
        ss << "select A.statement from (select 'create type " << schema << "." << datatype << " as ' || E'\n(\n' || \
               array_to_string(array_agg('    ' || column_name || ' ' ||  type) , E',\n') || E'\n);\n' as statement \
               from \
                ( \
                select A.attname as column_name, pg_catalog.format_type(A.atttypid, A.atttypmod) as type \
                from pg_attribute A join pg_catalog.pg_type T on A.attrelid = T.typrelid join \
                pg_catalog.pg_namespace N on N.oid = T.typnamespace \
                where T.typname = '" << datatype << "' and N.nspname = '" << schema << "' \
                ) as tabledefinition) A where A.statement is not null;";
        return ss.str();
    }
    
    std::string getDataDomainDDL(const Glib::ustring& schema, const Glib::ustring& domain)
    {
        std::stringstream ss;
        ss << "select pg_catalog.format_type(t.typbasetype, t.typtypmod) as Type, \
               (select c.collname from pg_catalog.pg_collation c, pg_catalog.pg_type bt \
                where c.oid = t.typcollation and bt.oid = t.typbasetype and t.typcollation <> bt.typcollation) as Collation, \
               case when t.typnotnull then 'not null' end as Nullable, \
               t.typdefault as Default, \
               pg_catalog.array_to_string(array( \
                 select pg_catalog.pg_get_constraintdef(r.oid, true) from pg_catalog.pg_constraint r where t.oid = r.contypid \
               ), ' ') as Check \
               from pg_catalog.pg_type t \
               join pg_catalog.pg_namespace n on n.oid = t.typnamespace \
               where t.typtype = 'd' and n.nspname = '" << schema << "' and t.typname = '" << domain << "'";
        return ss.str();
    }

    std::string getRoleList(const bool& canLogin)
    {
        std::stringstream ss;
        
        ss << "select r.rolname as rolename \
                from pg_catalog.pg_roles r join information_schema.role_table_grants t on r.rolname = t.grantee \
                where " << (canLogin ? std::string() : "not ")  << "r.rolcanlogin and r.rolname !~ '^pg_' \
                group by r.rolname \
                order by 1;";
        return ss.str();
    }
    
    std::string getRoleComment(const Glib::ustring& role)
    {
        std::stringstream ss;
        ss << "select pg_catalog.shobj_description(r.oid, 'pg_authid') as description \
                from pg_catalog.pg_roles r \
                where r.rolname ='" << role << "';";
        return ss.str();
    }
    
    std::string getRoleAttributes(const Glib::ustring& role)
    {
        std::stringstream ss;
        ss << "select r.rolname, r.rolsuper, r.rolinherit, \
                r.rolcreaterole, r.rolcreatedb, r.rolcanlogin, \
                r.rolconnlimit, r.rolvaliduntil, \
                array(select b.rolname \
                from pg_catalog.pg_auth_members m \
                join pg_catalog.pg_roles b on (m.roleid = b.oid) \
                where m.member = r.oid) as memberof, \
                r.rolreplication, \
                r.rolbypassrls \
                from pg_catalog.pg_roles r \
                where r.rolname = '" << role << "' \
                order by 1;";
        return ss.str();
    }
    
    std::string getRoleObjectPermissions(const Glib::ustring& role)
    {
        std::stringstream ss;
        ss << "select * \
                from \
                ( \
                select 'TABLE' as Type, concat(t.table_schema, '.', t.table_name) as table, \
                case \
                when count(t.privilege_type) = 7 then 'ALL' \
                else array_to_string(array_agg(t.privilege_type), ', ') \
                end as grants, 1 as DisplayOrder \
                from information_schema.role_table_grants t join \
                pg_catalog.pg_roles r on t.grantee = r.rolname \
                where t.grantee = '" << role << "' \
                group by t.table_name, t.table_schema \
                union all \
                select 'FUNCTION' as Type, concat(t.specific_schema, '.', t.routine_name) as table, \
                case \
                when count(t.privilege_type) = 7 then 'ALL' \
                else array_to_string(array_agg(t.privilege_type), ', ') \
                end as grants, 6 as DisplayOrder \
                from information_schema.role_routine_grants t join \
                pg_catalog.pg_roles r on t.grantee = r.rolname \
                where t.grantee = '" << role << "' \
                group by t.routine_name, t.specific_schema \
                union all \
                select 'COLUMN' as Type, concat(t.table_schema, '.', t.table_name, '.', t.column_name) as table, \
                case \
                when count(t.privilege_type) = 7 then 'ALL' \
                else array_to_string(array_agg(t.privilege_type), ', ') \
                end as grants, 2 as DisplayOrder \
                from information_schema.role_column_grants t join \
                pg_catalog.pg_roles r on t.grantee = r.rolname \
                where t.grantee = '" << role << "' \
                group by t.column_name, t.table_name, t.table_schema \
                union all \
                select 'UDT' as Type, concat(t.udt_schema, '.', t.udt_name) as table, \
                case \
                when count(t.privilege_type) = 8 then 'ALL' \
                else array_to_string(array_agg(t.privilege_type), ', ') \
                end as grants, 7 as DisplayOrder \
                from information_schema.role_udt_grants t join \
                pg_catalog.pg_roles r on t.grantee = r.rolname \
                where t.grantee = '" << role << "' and t.udt_name !~ '^pg_' \
                group by t.udt_name, t.udt_schema \
                union all \
                select t.object_type as Type, concat(t.object_schema, '.', t.object_name) as table, \
                case \
                when count(t.privilege_type) = 7 then 'ALL' \
                else array_to_string(array_agg(t.privilege_type), ', ') \
                end as grants, 9 as DisplayOrder \
                from information_schema.role_usage_grants t join \
                pg_catalog.pg_roles r on t.grantee = r.rolname \
                where t.grantee = '" << role << "' \
                group by t.object_type, t.object_name, t.object_schema \
                ) A \
                order by 4, 2;";
        return ss.str();        
    }
}
