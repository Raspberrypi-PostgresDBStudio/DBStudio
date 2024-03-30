#ifndef DBSTUDIO_H
#define DBSTUDIO_H

#include <gtkmm.h>
#include <sigc++/sigc++.h>
#include <pqxx/pqxx>
#include <map>
#include "DbServer.h"
#include "DbServerList.h"
#include "DbTreeModelColumns.h"
#include "DbTreeview.h"
#include "DbTextBuffer.h"
#include "DbMessage.h"

class DbStudio : public Gtk::Window
{
    private:
        struct loginDetail
        {
            Glib::ustring dbname;
            Glib::ustring user;
            Glib::ustring password; 
            Glib::ustring hostaddr;
            Glib::ustring port;
            Glib::ustring connParam;
        };
        std::vector<loginDetail> loginData;
        TreeModelColumns databaseTreeColumns;
        Gtk::TreeModel::ColumnRecord *sqlColumns = nullptr;      
        DbServerList dbList;
        std::vector<Gtk::TreeModelColumn<Glib::ustring>*> sqlResultColumns;
        Glib::ustring appPath;
        std::string sqlFilename;
        Gtk::AboutDialog aboutDialog;
        DbMessage msgBox;
        typedef void (DbStudio::*scriptFunction)(pqxx::nontransaction& N, std::string sql);
        typedef std::unordered_map<std::string, scriptFunction> script_map;
        script_map execSql;
        
        //Child widgets:
        Gtk::Paned mainHPaned;
        Gtk::Paned sqlPaned;
        Gtk::Notebook noteBook;
        Gtk::TextView sqlTextView;
        Gtk::TextView msgTextView;
        Gtk::VBox mainVBox;
        Gtk::VBox sqlEditorVBox;
        Gtk::VBox sqlResultVBox;
        Gtk::ComboBoxText cboActiveDatabase;
        Gtk::ScrolledWindow treeScrolledWindow;
        Gtk::ScrolledWindow sqlScrolledWindow;
        Gtk::ScrolledWindow msgScrolledWindow;
        DbTreeView databaseTreeView;
        Gtk::ScrolledWindow sqlResultScrolledWindow;
        Gtk::TreeView sqlResultView;
        Gtk::Entry txtFind;
        Gtk::Button btnFindFirst, btnFindNext;
        Gtk::Entry txtReplace;
        Gtk::Button btnReplace, btnReplaceFind;
        Gtk::Grid gridFind;
        
        //Smart Pointers
        Glib::RefPtr<Gtk::UIManager> refUIManager;
        Glib::RefPtr<Gtk::ActionGroup> refActionGroup;
        Glib::RefPtr<Gtk::TreeStore> refTreeModel;
        Glib::RefPtr<DbTextBuffer> refSqlTextBuffer { new DbTextBuffer };
        Glib::RefPtr<Gtk::TextBuffer> refMsgTextBuffer;
        Glib::RefPtr<Gtk::TreeStore> refListModel;
              
        //Functions
        bool getDbTreeInfo(Glib::ustring& serverName, Glib::ustring& databaseName, Glib::ustring& schemaName);
        DbServer *getDbTreeInfo(Glib::ustring& serverName, Glib::ustring& databaseName, Glib::ustring& schemaName, Glib::ustring& entityName);
        Glib::ustring getSelectedEntityAncestor(int AncestorLevel);
        void fillDatabaseTree(pqxx::connection& Conn, DbServer *db);
        void fillTables(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent);
        void fillForeignTables(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent);
        void fillViews(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent);
        void fillMaterializedViews(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent);
        void fillIndexList(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent);
        void fillFunctions(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent);
        void fillSequences(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent);
        void fillDataTypes(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent);
        void fillAggregateFunctions(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent);
        void fillUsers(pqxx::nontransaction& N, Gtk::TreeModel::Row parent);
        void fillRoles(pqxx::nontransaction& N, Gtk::TreeModel::Row parent);
        void fillEntityProperty(pqxx::nontransaction& N, Gtk::TreeModel::Row parent, Glib::ustring entityName, TreeModelColumnEntityType headingTypeId, std::string sql, TreeModelColumnEntityType typeId);
        void fillEntityProperty(pqxx::nontransaction& N, Gtk::TreeModel::Row propertiesrow, Glib::ustring entityName, std::string sql, TreeModelColumnEntityType typeId);
        void selectExecute(pqxx::nontransaction& N, std::string sql);
        void useExecute(pqxx::nontransaction& N, std::string sql);
        void refreshExecute(pqxx::nontransaction& N, std::string sql);
        void createConnectionExecute(pqxx::nontransaction& N, std::string sql);
        bool checkMapKey(std::map<std::string, std::string> m, std::string key);
        void clearConnections(void);
        bool askSaveData(void);
        bool deleteSelectedTreeChildren(Gtk::TreeModel::Row& row);
        Glib::ustring getSqlTemplate(Glib::ustring filename);
        Glib::ustring getSqlTemplate(Glib::ustring filename, std::map<std::string, std::string>& m);
        void refreshTreeBranch(void (DbStudio::*f)(pqxx::nontransaction&, Gtk::TreeModel::Row));
        void refreshTreeBranch(void (DbStudio::*f)(pqxx::nontransaction&, const Glib::ustring& schema, Gtk::TreeModel::Row));
        void refreshTreeBranch(Glib::ustring entityName, std::string (*f)(const Glib::ustring&, const Glib::ustring&), TreeModelColumnEntityType typeId);
        std::string getComment(pqxx::nontransaction& N, std::string sql);
        std::string getSaveAsFilename(std::string title, Glib::RefPtr<Gtk::FileFilter> filter_text);
        std::string getOpenFilename(std::string title, Glib::RefPtr<Gtk::FileFilter> filter);
        
        //Signal Handlers
        void onTreeviewClick(GdkEventButton* button_event);
        void onUndoRedoUpdate(bool hasUndo, bool hasRedo);
        void onWindowOpen(void);
        bool onExitClicked(GdkEventAny* event);
        bool onQueryTooltop(int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip);
        void onAboutDialogResponse(int response_id);
        bool onReadDbStructure(int timerNumber);
        bool onRefreshDbStructure(int timerNumber);
        bool onOpen(int timerNumber);
        void onFindFirst(void);
        void onFindNext(void);
        void onReplace(void);
        void onReplaceFind(void);
        
        //Popup Menu Signal Handlers
        void mnuServerCreateDatabase(void);
        void mnuDatabaseCreateSchema(void);
        void mnuDatabaseUse(void);
        void mnuTablesCreate(void);
        void mnuTablesRefresh(void);
        void mnuSchema(void);
        void mnuTableSelect(void);
        void mnuTableInsert(void);
        void mnuTableUpdate(void);
        void mnuTableDelete(void);
        void mnuTableDrop(void);
        void mnuTableDDL(void);
        void mnuTablesColumnCreate(void);
        void mnuTablesColumnRefresh(void);
        void mnuTableColumnDDL(void);
        void mnuTableColumnDrop(void);
        void mnuTablesConstraintCreate(void);
        void mnuTablesConstraintRefresh(void);
        void mnuTableConstraintDDL(void);
        void mnuTableConstraintDrop(void);
        void mnuIndexCreate(void);
        void mnuIndexRefresh(void);
        void mnuIndexDDL(void);
        void mnuIndexDrop(void);
        void mnuTiggerCreate(void);
        void mnuTiggerRefresh(void);
        void mnuTiggerDDL(void);
        void mnuTiggerDrop(void);
        void mnuForeignTablesCreate(void);
        void mnuForeignTablesRefresh(void);
        void mnuForeignTableDrop(void);
        void mnuViewsCreate(void);
        void mnuViewsRefresh(void);
        void mnuViewSelect(void);
        void mnuViewDrop(void);
        void mnuViewDDL(void);
        void mnuMaterializedViewsCreate(void);
        void mnuMaterializedViewsRefresh(void);
        void mnuMaterializedViewDrop(void);
        void mnuMaterializedViewDDL(void);
        void mnuIndexListRefresh(void);
        void mnuFunctionsCreate(void);
        void mnuProceduresCreate(void);
        void mnuFunctionsRefresh(void);
        void mnuFunctionDDL(void);
        void mnuFunctionExec(void);
        void mnuFunctionDrop(void);
        void mnuSequencesRefresh(void);
        void mnuSequenceLastValue(void);
        void mnuDataTypesCreate(void);
        void mnuDataDomainCreate(void);
        void mnuDataTypesRefresh(void);
        void mnuDataTypeDDL(void);
        void mnuDataTypeDrop(void);
        void mnuAggregateFunctionsCreate(void);
        void mnuAggregateFunctionsRefresh(void);
        void mnuAggregateFunctionDDL(void);
        void mnuAggregateFunctionDrop(void);
        void mnuUsersCreate(void);
        void mnuUsersRefresh(void);
        void mnuRolesCreate(void);
        void mnuRolesRefresh(void);
        void mnuRoleSummary(void);
        void mnuRoleDrop(void);
        
        //Main Menus Signal Handlers
        void mnuDatabaseOpenConnection(void);
        void mnuDatabaseRefreshConnection(void);
        void mnuStudioQuit(void);
        void mnuEditor(void);
        void mnuSqlEditorClear(void);
        void mnuSqlMsgClear(void);
        void mnuSqlEditorUndo(void);
        void mnuSqlEditorRedo(void);
        void mnuSqlEditorFind(void);
        void mnuSqlEditorToggle(void);
        void mnuSqlEditorOpen(void);
        void mnuSqlEditorSave(void);
        void mnuSqlEditorSaveAs(void);
        void mnuSqlEditorExecute(void);
        void mnuData(void);
        void mnuDataImportJSONFile(void);
        void mnuDataImportJSONString(void);
        void mnuDataImportCSVFile(void);
        void mnuDataExportJSONFile(void);
        void mnuDataExportCSVFile(void);
        void mnuDataExportXMLFile(void);
        void mnuHelpAbout(void);

        //Popup Tree Menus
        Gtk::Menu mnuServer;
        Gtk::Menu mnuDatabase;
        Gtk::Menu mnuTables;
        Gtk::Menu mnuTable;
        Gtk::Menu mnuTableColumns;
        Gtk::Menu mnuTableColumn;
        Gtk::Menu mnuTableConstraints;
        Gtk::Menu mnuTableConstraint;
        Gtk::Menu mnuIndexes;
        Gtk::Menu mnuIndex;
        Gtk::Menu mnuTriggers;
        Gtk::Menu mnuTrigger;
        Gtk::Menu mnuForeignTables;
        Gtk::Menu mnuForeignTable;
        Gtk::Menu mnuViews;
        Gtk::Menu mnuView;
        Gtk::Menu mnuMaterializedViews;
        Gtk::Menu mnuMaterializedView;
        Gtk::Menu mnuIndexList;
        Gtk::Menu mnuFunctions;
        Gtk::Menu mnuFunction;
        Gtk::Menu mnuSequences;
        Gtk::Menu mnuSequence;
        Gtk::Menu mnuDataTypes;
        Gtk::Menu mnuDataType;
        Gtk::Menu mnuAggregateFunctions;
        Gtk::Menu mnuAggregateFunction;
        Gtk::Menu mnuUsers;
        Gtk::Menu mnuUser;
        Gtk::Menu mnuRoles;
        Gtk::Menu mnuRole;
        
    public:
        DbStudio(void);
        virtual ~DbStudio(void);
};

#endif
