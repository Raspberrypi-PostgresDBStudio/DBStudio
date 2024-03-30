#ifndef DBTREEMODELCOLUMNS_H
#define DBTREEMODELCOLUMNS_H

#include <gtkmm.h>

enum TreeModelColumnEntityType
{
    Server,
    Database,
    Schema,
    Tables,
    Table,
    TableColumns,
    TableColumn,
    TableConstraints,
    TableConstraint,
    TableForeignKeys,
    TableForeignKey,
    TableIndexs,
    TableIndex,
    TableReferences,
    TableReference,
    TableTriggers,
    TableTrigger,
    ForeignTables,
    ForeignTable,
    ForeignTableColumns,
    ForeignTableColumn,
    Views,
    View,
    ViewColumns,
    ViewColumn,
    ViewTriggers,
    ViewTrigger,
    MaterializedViews,
    MaterializedView,
    MaterializedViewColumns,
    MaterializedViewColumn,
    MaterializedViewIndexes,
    MaterializedViewIndex,
    Indexes,
    Index,
    Functions,
    Function,
    FunctionParameters,
    FunctionParameter,
    FunctionReturns,
    FunctionReturn,
    Sequences,
    Sequence,
    DataTypes,
    DataType,
    AggregateFunctions,
    AggregateFunction,
    Users,
    User,
    Roles,
    Role
};

class TreeModelColumns : public Gtk::TreeModel::ColumnRecord
{
    public:
        TreeModelColumns(void)
        { 
            add(entityName); 
            add(dataType);
            add(entityTypeId);
            add(comment);
        }

        Gtk::TreeModelColumn<Glib::ustring> entityName;
        Gtk::TreeModelColumn<Glib::ustring> dataType;
        Gtk::TreeModelColumn<TreeModelColumnEntityType> entityTypeId;
        Gtk::TreeModelColumn<Glib::ustring> comment;
};

#endif
