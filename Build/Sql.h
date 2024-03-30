#ifndef SQL_H
#define SQL_H

#include <gtkmm.h>

namespace sql
{
    std::string getDatabaseList(void);
    std::string getServerVersion(void);
    std::string getDatabaseComment(const Glib::ustring& database);
    std::string getSchema(void);
    std::string getSchemaComment(const Glib::ustring& schema);
    std::string getTable(const Glib::ustring& schema);
    std::string getForeignTable(const Glib::ustring& schema);
    std::string getTableColumnComment(const Glib::ustring& schema, const Glib::ustring& entity, const Glib::ustring& column);
    std::string getEntityComment(const Glib::ustring& schema, const Glib::ustring& entity);
    std::string getColumn(const Glib::ustring& schema, const Glib::ustring& table);
    std::string getTableColumnDDL(const Glib::ustring& schema, const Glib::ustring& table, const Glib::ustring& column);
    std::string getTableConstraint(const Glib::ustring& schema, const Glib::ustring& table);
    std::string getTableConstraintDDL(const Glib::ustring& schema, const Glib::ustring& table, const Glib::ustring& constraint);
    std::string getTableForeignKey(const Glib::ustring& schema, const Glib::ustring& table);
    std::string getIndex(const Glib::ustring& schema, const Glib::ustring& table);
    std::string getIndexDDL(const Glib::ustring& schema, const Glib::ustring& constraint);
    std::string getTrigger(const Glib::ustring& schema, const Glib::ustring& table);
    std::string getTriggerDDL(const Glib::ustring& schema, const Glib::ustring& trigger);
    std::string getTablePrimaryKey(const Glib::ustring& schema, const Glib::ustring& table);
    std::string getTableReferences(const Glib::ustring& table);
    std::string getTableCreate(const Glib::ustring& schema, const Glib::ustring& table);
    std::string getTableCreateConstraint(const Glib::ustring& schema, const Glib::ustring& table);
    std::string getTableCreateIndex(const Glib::ustring& schema, const Glib::ustring& table);
    std::string getView(const Glib::ustring& schema);
    std::string getViewCreate(const Glib::ustring& schema, const Glib::ustring& view);
    std::string getMaterializedView(const Glib::ustring& schema);
    std::string getMaterializedViewCreate(const Glib::ustring& schema, const Glib::ustring& view);
    std::string getIndex(const Glib::ustring& schema);
    std::string getFunction(const Glib::ustring& schema);
    std::string getFunctionComment(const Glib::ustring& schema, const Glib::ustring& func);
    std::string getFunctionParameter(const Glib::ustring& schema, const Glib::ustring& func);
    std::string getFunctionReturn(const Glib::ustring& schema, const Glib::ustring& func);
    std::string getFunctionCreate(const Glib::ustring& schema, const Glib::ustring& func);
    std::string getSequence(const Glib::ustring& schema);
    std::string getDataType(const Glib::ustring& schema);
    std::string getDataTypeComment(const Glib::ustring& schema, const Glib::ustring& datatype);
    std::string getDataTypeDDL(const Glib::ustring& schema, const Glib::ustring& datatype);
    std::string getDataDomainDDL(const Glib::ustring& schema, const Glib::ustring& domain);
    std::string getAggregateFunction(const Glib::ustring& schema);
    std::string getAggregateFunctionDDL(const Glib::ustring& schema, const Glib::ustring& func);
    std::string getAggregateFunctionDrop(const Glib::ustring& schema, const Glib::ustring& func);
    std::string getAggregateFunctionComment(const Glib::ustring& schema, const Glib::ustring& func);
    std::string getRoleList(const bool& canLogin);
    std::string getRoleComment(const Glib::ustring& role);
    std::string getRoleAttributes(const Glib::ustring& role);
    std::string getRoleObjectPermissions(const Glib::ustring& role);
}
#endif
