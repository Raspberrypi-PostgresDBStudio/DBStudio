#include <iostream>
#include <fstream>
#include <map>
#include <gtkmm/stock.h>
#include "DbStudio.h"
#include "Utility.h"
#include "DbLogin.h"
#include "Sql.h"
#include "Excel.h"
#include "DataBaseColumnTypes.h"

//Class construction and distruction
DbStudio::DbStudio(void) : mainHPaned(Gtk::ORIENTATION_HORIZONTAL), sqlPaned(Gtk::ORIENTATION_VERTICAL), noteBook()
{
    execSql.emplace("select ", &DbStudio::selectExecute);
    execSql.emplace("use ", &DbStudio::useExecute);
    execSql.emplace("refresh connections", &DbStudio::refreshExecute);
    execSql.emplace("create connection ", &DbStudio::createConnectionExecute);
    
    set_title("Postgres Studio");
    set_default_size(1024, 700);

    add(mainVBox); // put a MenuBar at the top of the box and other stuff below it.

    //Create actions for menus and toolbars:
    refActionGroup = Gtk::ActionGroup::create();

    //Database Menu
    refActionGroup->add(Gtk::Action::create("DatabaseMenu", "Database"));
    refActionGroup->add(Gtk::Action::create("OpenConnection", Gtk::Stock::CONNECT, "Open Connection","Open Database Connection"),Gtk::AccelKey("F2"), sigc::mem_fun(*this, &DbStudio::mnuDatabaseOpenConnection));
    refActionGroup->add(Gtk::Action::create("RefreshConnection", "Refresh Connection","Refresh Database Connection"),Gtk::AccelKey("F3"), sigc::mem_fun(*this, &DbStudio::mnuDatabaseRefreshConnection));
    refActionGroup->add(Gtk::Action::create("StudioQuit", Gtk::Stock::QUIT, "_Quit", "Quit Postgres Studio"), sigc::mem_fun(*this, &DbStudio::mnuStudioQuit));

    //Edit menu:
    refActionGroup->add(Gtk::Action::create("EditorMenu", "Editor"), sigc::mem_fun(*this, &DbStudio::mnuEditor));
    refActionGroup->add(Gtk::Action::create("EditorClear", Gtk::Stock::NEW, "New", "Clear SQL Editor Window"), sigc::mem_fun(*this, &DbStudio::mnuSqlEditorClear));
    refActionGroup->add(Gtk::Action::create("EditorClearMsg", Gtk::Stock::CLEAR, "Clear Messages", "Clear SQL Message Window"), sigc::mem_fun(*this, &DbStudio::mnuSqlMsgClear));
    refActionGroup->add(Gtk::Action::create("EditorUndo", Gtk::Stock::UNDO, "Undo", "Undo SQL Edit"),Gtk::AccelKey("<ctrl>Z"), sigc::mem_fun(*this, &DbStudio::mnuSqlEditorUndo));
    refActionGroup->add(Gtk::Action::create("EditorRedo", Gtk::Stock::REDO, "ReDo", "Redo SQL Edit"),Gtk::AccelKey("<ctrl>Y"), sigc::mem_fun(*this, &DbStudio::mnuSqlEditorRedo));

    refActionGroup->add(Gtk::Action::create("EditorFind", Gtk::Stock::FIND, "Find/Replace", "Find/Replace SQL Text"),Gtk::AccelKey("<ctrl>F"), sigc::mem_fun(*this, &DbStudio::mnuSqlEditorFind));
    refActionGroup->add(Gtk::Action::create("EditorToggle", Gtk::Stock::APPLY, "Toggle Case", "Toggle Selected Text"),Gtk::AccelKey("<ctrl>U"), sigc::mem_fun(*this, &DbStudio::mnuSqlEditorToggle));
   
    refActionGroup->add(Gtk::Action::create("EditorLoad", Gtk::Stock::OPEN, "SQL Open", "Load SQL File"), sigc::mem_fun(*this, &DbStudio::mnuSqlEditorOpen));
    refActionGroup->add(Gtk::Action::create("EditorSave", Gtk::Stock::SAVE, "SQL Save", "Save SQL File"), sigc::mem_fun(*this, &DbStudio::mnuSqlEditorSave));
    refActionGroup->add(Gtk::Action::create("EditorSaveAs", Gtk::Stock::SAVE_AS, "SQL Save As", "Save As SQL File"), sigc::mem_fun(*this, &DbStudio::mnuSqlEditorSaveAs));
    refActionGroup->add(Gtk::Action::create("EditorExecute", Gtk::Stock::EXECUTE, "SQL Execute", "Execute SQL"),Gtk::AccelKey("F5"), sigc::mem_fun(*this, &DbStudio::mnuSqlEditorExecute));

    //Data menu:
    refActionGroup->add( Gtk::Action::create("DataMenu", "Data"), sigc::mem_fun(*this, &DbStudio::mnuData));
    refActionGroup->add( Gtk::Action::create("JsonImport", Gtk::Stock::PROPERTIES, "Import JSON File"), sigc::mem_fun(*this, &DbStudio::mnuDataImportJSONFile));
    refActionGroup->add( Gtk::Action::create("JsonImportString", Gtk::Stock::PROPERTIES, "Import JSON String"), sigc::mem_fun(*this, &DbStudio::mnuDataImportJSONString));
    refActionGroup->add( Gtk::Action::create("CsvImport", Gtk::Stock::PROPERTIES, "Import CSV File"), sigc::mem_fun(*this, &DbStudio::mnuDataImportCSVFile));
    refActionGroup->add( Gtk::Action::create("JsonExport", Gtk::Stock::PROPERTIES, "Export JSON File"), sigc::mem_fun(*this, &DbStudio::mnuDataExportJSONFile));
    refActionGroup->add( Gtk::Action::create("CsvExport", Gtk::Stock::PROPERTIES, "Export CSV File"), sigc::mem_fun(*this, &DbStudio::mnuDataExportCSVFile));
    refActionGroup->add( Gtk::Action::create("XmlExport", Gtk::Stock::PROPERTIES, "Export Excel 2003 XML File"), sigc::mem_fun(*this, &DbStudio::mnuDataExportXMLFile));
   
    //Help menu:
    refActionGroup->add( Gtk::Action::create("HelpMenu", "Help") );
    refActionGroup->add( Gtk::Action::create("HelpAbout", Gtk::Stock::HELP), sigc::mem_fun(*this, &DbStudio::mnuHelpAbout) );

    refUIManager = Gtk::UIManager::create();
    refUIManager->insert_action_group(refActionGroup);

    add_accel_group(refUIManager->get_accel_group());

    //Layout the actions in a menubar and toolbar:
    Glib::ustring mnuMainInfo = "<ui>"
                                "  <menubar name='MenuBar'>"
                                "    <menu action='DatabaseMenu'>"
                                "      <menuitem action='OpenConnection'/>"
                                "      <menuitem action='RefreshConnection'/>"
                                "      <separator/>"
                                "      <menuitem action='StudioQuit'/>"
                                "    </menu>"
                                "    <menu action='EditorMenu'>"
                                "      <menuitem action='EditorClear'/>"
                                "      <menuitem action='EditorClearMsg'/>"
                                "      <menuitem action='EditorUndo'/>"
                                "      <menuitem action='EditorRedo'/>"
                                "      <separator/>"
                                "      <menuitem action='EditorFind'/>"
                                "      <menuitem action='EditorToggle'/>"
                                "      <separator/>"
                                "      <menuitem action='EditorLoad'/>"
                                "      <menuitem action='EditorSave'/>"
                                "      <menuitem action='EditorSaveAs'/>"
                                "      <menuitem action='EditorExecute'/>"
                                "    </menu>"
                                "    <menu action='DataMenu'>"
                                "      <menuitem action='JsonImport'/>"
                                "      <menuitem action='JsonImportString'/>"
                                "      <menuitem action='CsvImport'/>"
                                "      <menuitem action='JsonExport'/>"
                                "      <menuitem action='CsvExport'/>"
                                "      <menuitem action='XmlExport'/>"
                                "    </menu>"
                                "    <menu action='HelpMenu'>"
                                "      <menuitem action='HelpAbout'/>"
                                "    </menu>"
                                "  </menubar>"
                                "  <toolbar  name='ToolBar'>"
                                "    <toolitem action='OpenConnection'/>"
                                "    <toolitem action='EditorLoad'/>"
                                "    <toolitem action='EditorSave'/>"
                                "    <toolitem action='EditorUndo'/>"
                                "    <toolitem action='EditorRedo'/>"
                                "    <toolitem action='EditorExecute'/>"
                                "    <toolitem action='StudioQuit'/>"
                                "  </toolbar>"
                                "</ui>";

    //Server menu
    auto item = Gtk::manage(new Gtk::MenuItem("Create New Database"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuServerCreateDatabase));
    mnuServer.append(*item);
    mnuServer.accelerate(*this);
    mnuServer.show_all(); 
    
    //Database menu
    item = Gtk::manage(new Gtk::MenuItem("Create New Schema"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuDatabaseCreateSchema));
    mnuDatabase.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("SQL Use"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuDatabaseUse));
    mnuDatabase.append(*item);
    
    mnuDatabase.accelerate(*this);
    mnuDatabase.show_all(); 
    
    //Tables Menu
    item = Gtk::manage(new Gtk::MenuItem("Create New Table"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTablesCreate));
    mnuTables.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Refresh Tables"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTablesRefresh));
    mnuTables.append(*item);
    
    mnuTables.accelerate(*this);
    mnuTables.show_all(); 
    
    item = Gtk::manage(new Gtk::MenuItem("Sql Select"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTableSelect));
    mnuTable.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql Insert"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTableInsert));
    mnuTable.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql Update"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTableUpdate));
    mnuTable.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql Delete"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTableDelete));
    mnuTable.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql DDL"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTableDDL));
    mnuTable.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql Drop"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTableDrop));
    mnuTable.append(*item);

    mnuTable.accelerate(*this);
    mnuTable.show_all(); 
    
    //Table Columns
    item = Gtk::manage(new Gtk::MenuItem("Create New Column"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTablesColumnCreate));
    mnuTableColumns.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Refresh Columns"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTablesColumnRefresh));
    mnuTableColumns.append(*item);

    mnuTableColumns.accelerate(*this);
    mnuTableColumns.show_all(); 

    item = Gtk::manage(new Gtk::MenuItem("SQL DDL"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTableColumnDDL));
    mnuTableColumn.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("SQL DROP"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTableColumnDrop));
    mnuTableColumn.append(*item);

    mnuTableColumn.accelerate(*this);
    mnuTableColumn.show_all(); 

    //Table Constraints
    item = Gtk::manage(new Gtk::MenuItem("Create New Constraint"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTablesConstraintCreate));
    mnuTableConstraints.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Refresh Constaints"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTablesConstraintRefresh));
    mnuTableConstraints.append(*item);

    mnuTableConstraints.accelerate(*this);
    mnuTableConstraints.show_all(); 

    item = Gtk::manage(new Gtk::MenuItem("SQL DDL"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTableConstraintDDL));
    mnuTableConstraint.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("SQL DROP"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTableConstraintDrop));
    mnuTableConstraint.append(*item);

    mnuTableConstraint.accelerate(*this);
    mnuTableConstraint.show_all(); 
    
    //Table Indexes
    item = Gtk::manage(new Gtk::MenuItem("Create New Index"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuIndexCreate));
    mnuIndexes.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Refresh Indexes"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuIndexRefresh));
    mnuIndexes.append(*item);

    mnuIndexes.accelerate(*this);
    mnuIndexes.show_all(); 

    item = Gtk::manage(new Gtk::MenuItem("SQL DDL"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuIndexDDL));
    mnuIndex.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("SQL DROP"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuIndexDrop));
    mnuIndex.append(*item);

    mnuIndex.accelerate(*this);
    mnuIndex.show_all(); 
    
    //Table Tiggers
    item = Gtk::manage(new Gtk::MenuItem("Create New Trigger"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTiggerCreate));
    mnuTriggers.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Refresh Triggers"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTiggerRefresh));
    mnuTriggers.append(*item);

    mnuTriggers.accelerate(*this);
    mnuTriggers.show_all();    
    
    //mnuTrigger
    item = Gtk::manage(new Gtk::MenuItem("SQL DDL"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTiggerDDL));
    mnuTrigger.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("SQL DROP"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTiggerDrop));
    mnuTrigger.append(*item);

    mnuTrigger.accelerate(*this);
    mnuTrigger.show_all(); 

    //Foreign Tables Menu
    item = Gtk::manage(new Gtk::MenuItem("Create New Foreign Table"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuForeignTablesCreate));
    mnuForeignTables.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Refresh Foreign Tables"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuForeignTablesRefresh));
    mnuForeignTables.append(*item);
    
    mnuForeignTables.accelerate(*this);
    mnuForeignTables.show_all(); 
    
    item = Gtk::manage(new Gtk::MenuItem("Sql Select"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTableSelect));
    mnuForeignTable.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql Insert"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTableInsert));
    mnuForeignTable.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql Update"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTableUpdate));
    mnuForeignTable.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql Delete"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuTableDelete));
    mnuForeignTable.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql Drop"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuForeignTableDrop));
    mnuForeignTable.append(*item);

    mnuForeignTable.accelerate(*this);
    mnuForeignTable.show_all(); 
   
    //Views Menu
    item = Gtk::manage(new Gtk::MenuItem("Create New View"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuViewsCreate));
    mnuViews.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Refresh Views"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuViewsRefresh));
    mnuViews.append(*item);
    
    mnuViews.accelerate(*this);
    mnuViews.show_all(); 
    
    item = Gtk::manage(new Gtk::MenuItem("Sql Select"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuViewSelect));
    mnuView.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql DDL"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuViewDDL));
    mnuView.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql Drop"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuViewDrop));
    mnuView.append(*item);

    mnuView.accelerate(*this);
    mnuView.show_all(); 
    
    //Materialized View Menu
    item = Gtk::manage(new Gtk::MenuItem("Create New Materialized View"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuMaterializedViewsCreate));
    mnuMaterializedViews.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Refresh Materialized Views"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuMaterializedViewsRefresh));
    mnuMaterializedViews.append(*item);
    
    mnuMaterializedViews.accelerate(*this);
    mnuMaterializedViews.show_all(); 
    
    item = Gtk::manage(new Gtk::MenuItem("Sql Select"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuViewSelect));
    mnuMaterializedView.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql DDL"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuMaterializedViewDDL));
    mnuMaterializedView.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql Drop"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuMaterializedViewDrop));
    mnuMaterializedView.append(*item);

    mnuMaterializedView.accelerate(*this);
    mnuMaterializedView.show_all(); 
        
    //Index List Menu
    item = Gtk::manage(new Gtk::MenuItem("Refresh Indexes"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuIndexListRefresh));
    mnuIndexList.append(*item);

    mnuIndexList.accelerate(*this);
    mnuIndexList.show_all(); 
    
    //Function Menu
    item = Gtk::manage(new Gtk::MenuItem("Create New Function"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuFunctionsCreate));
    mnuFunctions.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Create New Procedure"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuProceduresCreate));
    mnuFunctions.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Refresh Functions"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuFunctionsRefresh));
    mnuFunctions.append(*item);
    
    mnuFunctions.accelerate(*this);
    mnuFunctions.show_all(); 
    
    item = Gtk::manage(new Gtk::MenuItem("Sql DDL"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuFunctionDDL));
    mnuFunction.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql Execute"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuFunctionExec));
    mnuFunction.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql Drop"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuFunctionDrop));
    mnuFunction.append(*item);

    mnuFunction.accelerate(*this);
    mnuFunction.show_all(); 
    
    //Sequence Menu
    item = Gtk::manage(new Gtk::MenuItem("Refresh Sequences"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuSequencesRefresh));
    mnuSequences.append(*item);
    
    mnuSequences.accelerate(*this);
    mnuSequences.show_all(); 
    
    item = Gtk::manage(new Gtk::MenuItem("Sql Last Value"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuSequenceLastValue));
    mnuSequence.append(*item);

    mnuSequence.accelerate(*this);
    mnuSequence.show_all(); 
    
    //Data Types Menu
    item = Gtk::manage(new Gtk::MenuItem("Create New Data Type"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuDataTypesCreate));
    mnuDataTypes.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Create New Data Domain"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuDataDomainCreate));
    mnuDataTypes.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Refresh Data Types"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuDataTypesRefresh));
    mnuDataTypes.append(*item);
    
    mnuDataTypes.accelerate(*this);
    mnuDataTypes.show_all(); 

    item = Gtk::manage(new Gtk::MenuItem("Sql DDL"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuDataTypeDDL));
    mnuDataType.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql Drop"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuDataTypeDrop));
    mnuDataType.append(*item);

    mnuDataType.accelerate(*this);
    mnuDataType.show_all(); 

    //Aggregate Function Menu
    item = Gtk::manage(new Gtk::MenuItem("Create New Aggregate Function"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuAggregateFunctionsCreate));
    mnuAggregateFunctions.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Refresh Aggregate Functions"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuAggregateFunctionsRefresh));
    mnuAggregateFunctions.append(*item);
    
    mnuAggregateFunctions.accelerate(*this);
    mnuAggregateFunctions.show_all(); 
    
    item = Gtk::manage(new Gtk::MenuItem("Sql DDL"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuAggregateFunctionDDL));
    mnuAggregateFunction.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql Drop"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuAggregateFunctionDrop));
    mnuAggregateFunction.append(*item);

    mnuAggregateFunction.accelerate(*this);
    mnuAggregateFunction.show_all(); 

    //User Menu
    item = Gtk::manage(new Gtk::MenuItem("Create New User"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuUsersCreate));
    mnuUsers.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Refresh Users"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuUsersRefresh));
    mnuUsers.append(*item);
    
    mnuUsers.accelerate(*this);
    mnuUsers.show_all(); 

    item = Gtk::manage(new Gtk::MenuItem("User Summary"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuRoleSummary));
    mnuUser.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql Drop"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuRoleDrop));
    mnuUser.append(*item);

    mnuUser.accelerate(*this);
    mnuUser.show_all(); 
    
    //Roles Menu
    item = Gtk::manage(new Gtk::MenuItem("Create New Role"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuRolesCreate));
    mnuRoles.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Refresh Roles"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuRolesRefresh));
    mnuRoles.append(*item);
    
    mnuRoles.accelerate(*this);
    mnuRoles.show_all(); 

    item = Gtk::manage(new Gtk::MenuItem("Role Summary"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuRoleSummary));
    mnuRole.append(*item);
    item = Gtk::manage(new Gtk::MenuItem("Sql Drop"));
    item->signal_activate().connect(sigc::mem_fun(*this, &DbStudio::mnuRoleDrop));
    mnuRole.append(*item);

    mnuRole.accelerate(*this);
    mnuRole.show_all(); 
    
    try
    {
        refUIManager->add_ui_from_string(mnuMainInfo);
    }
    catch(const Glib::Error& ex)
    {
        std::cerr << "building menus failed: " <<  ex.what();
    }

    //Get the menubar and toolbar widgets, and add them to a container widget:
    Gtk::Widget* pMenubar = refUIManager->get_widget("/MenuBar");
    if(pMenubar)
        mainVBox.pack_start(*pMenubar, Gtk::PACK_SHRINK);

    Gtk::Widget* pToolbar = refUIManager->get_widget("/ToolBar") ;
    if(pToolbar)
        mainVBox.pack_start(*pToolbar, Gtk::PACK_SHRINK);

    mainVBox.add(mainHPaned);

    //Add the TreeView, inside a ScrolledWindow:
    treeScrolledWindow.add(databaseTreeView);

    //Only show the scrollbars when they are necessary:
    treeScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    mainHPaned.add1(treeScrolledWindow);

    //Create the Tree model:
    refTreeModel = Gtk::TreeStore::create(databaseTreeColumns);
    databaseTreeView.set_model(refTreeModel);
    databaseTreeView.append_column("Database", databaseTreeColumns.entityName);
    databaseTreeView.append_column(Glib::ustring(), databaseTreeColumns.dataType);
    databaseTreeView.signal_on_button_click().connect(sigc::mem_fun(*this, &DbStudio::onTreeviewClick));
    databaseTreeView.set_has_tooltip(true);
    databaseTreeView.signal_query_tooltip().connect(sigc::mem_fun(*this, &DbStudio::onQueryTooltop));

    //Create the List model
    sqlResultScrolledWindow.add(sqlResultView);
    sqlResultScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    sqlResultVBox.add(sqlResultScrolledWindow);
    
    //Configure Notebook
    noteBook.set_tab_pos(Gtk::POS_TOP);
    noteBook.append_page(sqlEditorVBox, "SQL Editor");
    noteBook.append_page(sqlResultVBox, "SQL Result");

    btnFindFirst.set_label("Find First");
    btnFindNext.set_label("Find Next");
    btnReplace.set_label("Replace");
    btnReplaceFind.set_label("Replace & Find");
    gridFind.attach(txtFind, 0, 0, 3, 1);
    gridFind.attach(btnFindFirst, 4, 0, 1, 1);
    gridFind.attach(btnFindNext, 5, 0, 1, 1);
    gridFind.attach(txtReplace, 6, 0, 3, 1);
    gridFind.attach(btnReplace, 9, 0, 1, 1);
    gridFind.attach(btnReplaceFind, 10, 0, 1, 1);
    
    btnFindFirst.signal_clicked().connect(sigc::mem_fun(*this, &DbStudio::onFindFirst));
    btnFindNext.signal_clicked().connect(sigc::mem_fun(*this, &DbStudio::onFindNext));
    btnReplace.signal_clicked().connect(sigc::mem_fun(*this, &DbStudio::onReplace));
    btnReplaceFind.signal_clicked().connect(sigc::mem_fun(*this, &DbStudio::onReplaceFind));
    
    sqlEditorVBox.pack_start(cboActiveDatabase, Gtk::PACK_SHRINK);
    sqlEditorVBox.pack_start(gridFind, Gtk::PACK_SHRINK);
    sqlEditorVBox.add(sqlPaned);

    Glib::RefPtr<Gtk::CssProvider> css_sql = Gtk::CssProvider::create();
    css_sql->load_from_path(appPath + "SqlEditor.css");
    sqlTextView.get_style_context()->add_provider(css_sql, GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    sqlTextView.set_buffer(refSqlTextBuffer);
    refSqlTextBuffer->signal_on_undo_stack_update().connect(sigc::mem_fun(*this, &DbStudio::onUndoRedoUpdate));

    sqlScrolledWindow.add(sqlTextView);
    sqlScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    sqlScrolledWindow.set_border_width(5);
    sqlPaned.add1(sqlScrolledWindow);

    refMsgTextBuffer = Gtk::TextBuffer::create();
    msgTextView.set_buffer(refMsgTextBuffer);
    Glib::RefPtr<Gtk::CssProvider> css_msg = Gtk::CssProvider::create();
    css_msg->load_from_path(appPath + "SqlMsg.css");
    msgTextView.get_style_context()->add_provider(css_msg, GTK_STYLE_PROVIDER_PRIORITY_USER);   
    msgTextView.set_editable(false);
    msgScrolledWindow.add(msgTextView);
    msgScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    sqlPaned.add2(msgScrolledWindow);
    mainHPaned.add2(noteBook);
    
    aboutDialog.set_skip_taskbar_hint(true);
    aboutDialog.set_transient_for(*this);
    aboutDialog.set_logo(Gdk::Pixbuf::create_from_file("sql.png"));
    aboutDialog.set_program_name("SQL PostgreSQL Studio");
    aboutDialog.set_version("1.0.0");
    aboutDialog.set_copyright("Paddy Fuller 2024");

    aboutDialog.set_comments("Manage PostgreSQL Databases");
    aboutDialog.set_license_type((Gtk::License)3); //GPL_3_0_ONLY
    
    std::vector<Glib::ustring> list_authors;
    list_authors.push_back("Patrick Neal Fuller");
    aboutDialog.set_authors(list_authors);
    
    aboutDialog.signal_response().connect(sigc::mem_fun(*this, &DbStudio::onAboutDialogResponse));
    signal_show().connect(sigc::mem_fun(*this, &DbStudio::onWindowOpen));
    signal_delete_event().connect(sigc::mem_fun(this, &DbStudio::onExitClicked));

    show_all_children();
    gridFind.hide();
    appPath = utility::getBundlePath();
    
    //Set Menu/Toolbar States
    onUndoRedoUpdate(false, false);
}

DbStudio::~DbStudio(void)
{
    if(sqlColumns != nullptr)
        delete sqlColumns;
}

//Functions
void DbStudio::refreshTreeBranch(void (DbStudio::*f)(pqxx::nontransaction&, Gtk::TreeModel::Row))
{
    try
    {
        Gtk::TreeModel::Row row;
        if(deleteSelectedTreeChildren(row))
        {     
            Glib::ustring serverName, databaseName, schemaName;
            getDbTreeInfo(serverName, databaseName, schemaName);
            DbServer *db = dbList.searchServerDatabaseElement(serverName, databaseName);
            if(db != nullptr)
            {
                pqxx::connection Conn(db->connectionString());
                if (Conn.is_open())
                {
                    pqxx::nontransaction N(Conn);
                    (this->*f)(N, row);
                }
            }
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::refreshTreeBranch(void (DbStudio::*f)(pqxx::nontransaction&, const Glib::ustring& schema, Gtk::TreeModel::Row))
{
    try
    {
        Gtk::TreeModel::Row row;
        if(deleteSelectedTreeChildren(row))
        {     
            Glib::ustring serverName, databaseName, schemaName;
            getDbTreeInfo(serverName, databaseName, schemaName);
            DbServer *db = dbList.searchServerDatabaseElement(serverName, databaseName);
            if(db != nullptr)
            {
                pqxx::connection Conn(db->connectionString());
                if (Conn.is_open())
                {
                    pqxx::nontransaction N(Conn);
                    (this->*f)(N, schemaName, row);
                }
            }
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::refreshTreeBranch(Glib::ustring entityName, std::string (*f)(const Glib::ustring&, const Glib::ustring&), TreeModelColumnEntityType typeId)
{
    try
    {
        Gtk::TreeModel::Row row;
        if(deleteSelectedTreeChildren(row))
        {     
            Glib::ustring serverName, databaseName, schemaName, tableName;
            getDbTreeInfo(serverName, databaseName, schemaName);
            tableName = getSelectedEntityAncestor(1);
            DbServer *db = dbList.searchServerDatabaseElement(serverName, databaseName);
            if(db != nullptr)
            {
                pqxx::connection Conn(db->connectionString());
                if (Conn.is_open())
                {
                    std::string sql = (*f)(schemaName, tableName);
                    pqxx::nontransaction N(Conn);
                    fillEntityProperty(N, row, entityName, sql, typeId);
                }
            }
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::fillDatabaseTree(pqxx::connection& Conn, DbServer *db)
{
    try
    {
        Gtk::TreeModel::Row row;
        
        if(dbList.searchServerElement(db->serverName()) == nullptr)
        {
            row = *(refTreeModel->append());
            row[databaseTreeColumns.entityName] = db->serverName();
            row[databaseTreeColumns.dataType] = Glib::ustring();
            row[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::Server;
        }
        else
        {
            Gtk::TreeModel::Children children = refTreeModel->children();
            for(Gtk::TreeModel::Children::iterator iter = children.begin() ; iter != children.end() ; ++iter)
            {
                row = *iter;
                if(row[databaseTreeColumns.entityName] == db->serverName())
                    break;
            }
        }
                    
        cboActiveDatabase.append(db->serverName() + " (" + db->databaseName() + ")");
            
        pqxx::nontransaction N(Conn);

        Gtk::TreeModel::Row dbrow = *(refTreeModel->append(row.children()));
        dbrow[databaseTreeColumns.entityName] = db->databaseName();
        dbrow[databaseTreeColumns.dataType] = Glib::ustring();
        dbrow[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::Database;
        dbrow[databaseTreeColumns.comment] = getComment(N, sql::getDatabaseComment(db->databaseName()));
        
        pqxx::result RSchema(N.exec(sql::getSchema()));      
        for (pqxx::result::const_iterator s = RSchema.begin(); s != RSchema.end(); ++s) 
        {
            //Schema
            Gtk::TreeModel::Row schemarow = *(refTreeModel->append(dbrow.children()));
            schemarow[databaseTreeColumns.entityName] = s[0].as<std::string>();
            schemarow[databaseTreeColumns.dataType] = Glib::ustring();
            schemarow[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::Schema;
            schemarow[databaseTreeColumns.comment] = getComment(N, sql::getSchemaComment(s[0].as<std::string>()));

            //Tables
            Gtk::TreeModel::Row objectrow = *(refTreeModel->append(schemarow.children()));
            objectrow[databaseTreeColumns.entityName] = "Tables";
            objectrow[databaseTreeColumns.dataType] = Glib::ustring();
            objectrow[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::Tables;

            //List each table
            fillTables(N, s[0].as<std::string>(), objectrow);

            //ForeignTables
            objectrow = *(refTreeModel->append(schemarow.children()));
            objectrow[databaseTreeColumns.entityName] = "Foreign Tables";
            objectrow[databaseTreeColumns.dataType] = Glib::ustring();
            objectrow[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::ForeignTables;

            //List each table
            fillForeignTables(N, s[0].as<std::string>(), objectrow);

            //Views
            objectrow = *(refTreeModel->append(schemarow.children()));
            objectrow[databaseTreeColumns.entityName] = "Views";
            objectrow[databaseTreeColumns.dataType] = Glib::ustring();
            objectrow[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::Views;
            
            //list each view
            fillViews(N, s[0].as<std::string>(), objectrow);

            //Materialized Views
            objectrow = *(refTreeModel->append(schemarow.children()));
            objectrow[databaseTreeColumns.entityName] = "Materialized Views";
            objectrow[databaseTreeColumns.dataType] = Glib::ustring();
            objectrow[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::MaterializedViews;
            
            //list each Materialized view
            fillMaterializedViews(N, s[0].as<std::string>(), objectrow);
                    
            //Indexes
            objectrow = *(refTreeModel->append(schemarow.children()));
            objectrow[databaseTreeColumns.entityName] = "Indexes";
            objectrow[databaseTreeColumns.dataType] = Glib::ustring();
            objectrow[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::Indexes;

            fillIndexList(N, s[0].as<std::string>(), objectrow);

            //Functions 
            objectrow = *(refTreeModel->append(schemarow.children()));
            objectrow[databaseTreeColumns.entityName] = "Functions";
            objectrow[databaseTreeColumns.dataType] = Glib::ustring();
            objectrow[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::Functions;
            
            fillFunctions(N, s[0].as<std::string>(), objectrow);
            
            //Sequences
            objectrow = *(refTreeModel->append(schemarow.children()));
            objectrow[databaseTreeColumns.entityName] = "Sequences";
            objectrow[databaseTreeColumns.dataType] = Glib::ustring();
            objectrow[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::Sequences;
            
            fillSequences(N, s[0].as<std::string>(), objectrow);
            
            //getDataTypeSql
            objectrow = *(refTreeModel->append(schemarow.children()));
            objectrow[databaseTreeColumns.entityName] = "Data Types";
            objectrow[databaseTreeColumns.dataType] = Glib::ustring();
            objectrow[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::DataTypes;
            
            fillDataTypes(N, s[0].as<std::string>(), objectrow);
            
            //Aggregate Function
            objectrow = *(refTreeModel->append(schemarow.children()));
            objectrow[databaseTreeColumns.entityName] = "Aggregate Function";
            objectrow[databaseTreeColumns.dataType] = Glib::ustring();
            objectrow[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::AggregateFunctions;
            
            fillAggregateFunctions(N, s[0].as<std::string>(), objectrow);
        }
        //Users
        Gtk::TreeModel::Row rolerow = *(refTreeModel->append(dbrow.children()));
        rolerow[databaseTreeColumns.entityName] = "Users";
        rolerow[databaseTreeColumns.dataType] = Glib::ustring();
        rolerow[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::Users;
        
        fillUsers(N, rolerow);
        
        //Roles
        rolerow = *(refTreeModel->append(dbrow.children()));
        rolerow[databaseTreeColumns.entityName] = "Roles";
        rolerow[databaseTreeColumns.dataType] = Glib::ustring();
        rolerow[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::Roles;
        
        fillRoles(N, rolerow);
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::fillTables(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent)
{
    try
    {
        pqxx::result rst(N.exec(sql::getTable(schema)));
        for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
        {
            Gtk::TreeModel::Row row = *(refTreeModel->append(parent.children()));
            row[databaseTreeColumns.entityName] = r[0].as<std::string>();
            row[databaseTreeColumns.dataType] = Glib::ustring();
            row[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::Table;
            row[databaseTreeColumns.comment] = getComment(N, sql::getEntityComment(schema, r[0].as<std::string>()));
            
            fillEntityProperty(N, row, "Columns", TreeModelColumnEntityType::TableColumns, sql::getColumn(schema, r[0].as<std::string>()), TreeModelColumnEntityType::TableColumn);
            fillEntityProperty(N, row, "Constraints", TreeModelColumnEntityType::TableConstraints, sql::getTableConstraint(schema, r[0].as<std::string>()), TreeModelColumnEntityType::TableConstraint);
            fillEntityProperty(N, row, "Foreign Keys", TreeModelColumnEntityType::TableForeignKeys, sql::getTableForeignKey(schema, r[0].as<std::string>()), TreeModelColumnEntityType::TableForeignKey);
            fillEntityProperty(N, row, "Indexes", TreeModelColumnEntityType::TableIndexs, sql::getIndex(schema, r[0].as<std::string>()), TreeModelColumnEntityType::TableIndex);
            fillEntityProperty(N, row, "References", TreeModelColumnEntityType::TableReferences, sql::getTableReferences(r[0].as<std::string>()), TreeModelColumnEntityType::TableReference);
            fillEntityProperty(N, row, "Triggers", TreeModelColumnEntityType::TableTriggers, sql::getTrigger(schema, r[0].as<std::string>()), TreeModelColumnEntityType::TableTrigger);
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::fillForeignTables(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent)
{
    try
    {
        pqxx::result rst(N.exec(sql::getForeignTable(schema)));
        for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
        {
            Gtk::TreeModel::Row row = *(refTreeModel->append(parent.children()));
            row[databaseTreeColumns.entityName] = r[0].as<std::string>();
            row[databaseTreeColumns.dataType] = Glib::ustring();
            row[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::ForeignTable;
            row[databaseTreeColumns.comment] = getComment(N, sql::getEntityComment(schema, r[0].as<std::string>()));
            
            fillEntityProperty(N, row, "Columns", TreeModelColumnEntityType::ForeignTableColumns, sql::getColumn(schema, r[0].as<std::string>()), TreeModelColumnEntityType::ForeignTableColumn);
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::fillViews(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent)
{ 
    try
    {
        pqxx::result rst(N.exec(sql::getView(schema)));
        for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
        {
            Gtk::TreeModel::Row row = *(refTreeModel->append(parent.children()));
            row[databaseTreeColumns.entityName] = r[0].as<std::string>();
            row[databaseTreeColumns.dataType] = Glib::ustring();
            row[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::View;
            row[databaseTreeColumns.comment] = getComment(N, sql::getEntityComment(schema, r[0].as<std::string>()));

            fillEntityProperty(N, row, "Columns", TreeModelColumnEntityType::ViewColumns, sql::getColumn(schema, r[0].as<std::string>()), TreeModelColumnEntityType::ViewColumn);
            fillEntityProperty(N, row, "Triggers", TreeModelColumnEntityType::ViewTriggers, sql::getTrigger(schema,r[0].as<std::string>()), TreeModelColumnEntityType::ViewTrigger);
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::fillMaterializedViews(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent)
{
    try
    {
        pqxx::result rst(N.exec(sql::getMaterializedView(schema)));
        for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
        {
            Gtk::TreeModel::Row row = *(refTreeModel->append(parent.children()));
            row[databaseTreeColumns.entityName] = r[0].as<std::string>();
            row[databaseTreeColumns.dataType] = Glib::ustring();
            row[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::MaterializedView;
            row[databaseTreeColumns.comment] = getComment(N, sql::getEntityComment(schema, r[0].as<std::string>()));

            fillEntityProperty(N, row, "Columns", TreeModelColumnEntityType::MaterializedViewColumns, sql::getColumn(schema, r[0].as<std::string>()), TreeModelColumnEntityType::MaterializedViewColumn);
            fillEntityProperty(N, row, "Indexes", TreeModelColumnEntityType::MaterializedViewIndexes, sql::getIndex(schema, r[0].as<std::string>()), TreeModelColumnEntityType::MaterializedViewIndex);
        }    
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::fillIndexList(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent)
{
    try
    {
        pqxx::result rst(N.exec(sql::getIndex(schema)));
        for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
        {
            Gtk::TreeModel::Row row = *(refTreeModel->append(parent.children()));
            row[databaseTreeColumns.entityName] = r[0].as<std::string>();
            row[databaseTreeColumns.dataType] = Glib::ustring();
            row[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::Index;
            row[databaseTreeColumns.comment] = getComment(N, sql::getEntityComment(schema, r[0].as<std::string>()));
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::fillFunctions(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent)
{
    try
    {
        pqxx::result rst(N.exec(sql::getFunction(schema)));
        for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
        {
            Gtk::TreeModel::Row row = *(refTreeModel->append(parent.children()));
            row[databaseTreeColumns.entityName] = r[0].as<std::string>();
            row[databaseTreeColumns.dataType] = Glib::ustring();
            row[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::Function;
            row[databaseTreeColumns.comment] = getComment(N, sql::getFunctionComment(schema, r[0].as<std::string>()));
            
            fillEntityProperty(N, row, "Function parameters", TreeModelColumnEntityType::FunctionParameters, sql::getFunctionParameter(schema, r[0].as<std::string>()), TreeModelColumnEntityType::FunctionParameter);              
            fillEntityProperty(N, row, "Function returns", TreeModelColumnEntityType::FunctionReturns, sql::getFunctionReturn(schema, r[0].as<std::string>()), TreeModelColumnEntityType::FunctionReturn);
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::fillSequences(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent)
{
    try
    {
        pqxx::result rst(N.exec(sql::getSequence(schema)));
        for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
        {
            Gtk::TreeModel::Row row = *(refTreeModel->append(parent.children()));
            row[databaseTreeColumns.entityName] = r[0].as<std::string>();
            row[databaseTreeColumns.dataType] = Glib::ustring();
            row[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::Sequence;
            row[databaseTreeColumns.comment] = getComment(N, sql::getEntityComment(schema, r[0].as<std::string>()));
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }    
}

void DbStudio::fillDataTypes(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent)
{
    try
    {
        pqxx::result rst(N.exec(sql::getDataType(schema)));
        for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
        {
            Gtk::TreeModel::Row row = *(refTreeModel->append(parent.children()));
            row[databaseTreeColumns.entityName] = r[0].as<std::string>();
            row[databaseTreeColumns.dataType] = Glib::ustring();
            row[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::DataType;
            row[databaseTreeColumns.comment] = getComment(N, sql::getDataTypeComment(schema, r[0].as<std::string>()));
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }    
}

void DbStudio::fillAggregateFunctions(pqxx::nontransaction& N, const Glib::ustring& schema, Gtk::TreeModel::Row parent)
{
    try
    {
        pqxx::result rst(N.exec(sql::getAggregateFunction(schema)));
        for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
        {
            Gtk::TreeModel::Row row = *(refTreeModel->append(parent.children()));
            row[databaseTreeColumns.entityName] = r[0].as<std::string>();
            row[databaseTreeColumns.dataType] = Glib::ustring();
            row[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::AggregateFunction;
            row[databaseTreeColumns.comment] = getComment(N, sql::getAggregateFunctionComment(schema, r[0].as<std::string>()));
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }   
}

void DbStudio::fillUsers(pqxx::nontransaction& N, Gtk::TreeModel::Row parent)
{
    try
    {
        pqxx::result rst(N.exec(sql::getRoleList(true)));
        for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
        {
            Gtk::TreeModel::Row row = *(refTreeModel->append(parent.children()));
            row[databaseTreeColumns.entityName] = r[0].as<std::string>();
            row[databaseTreeColumns.dataType] = Glib::ustring();
            row[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::User;
            row[databaseTreeColumns.comment] = getComment(N, sql::getRoleComment(r[0].as<std::string>()));
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::fillRoles(pqxx::nontransaction& N, Gtk::TreeModel::Row parent)
{
    try
    {
        pqxx::result rst(N.exec(sql::getRoleList(false)));
        for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
        {
            Gtk::TreeModel::Row row = *(refTreeModel->append(parent.children()));
            row[databaseTreeColumns.entityName] = r[0].as<std::string>();
            row[databaseTreeColumns.dataType] = Glib::ustring();
            row[databaseTreeColumns.entityTypeId] = TreeModelColumnEntityType::Role;
            row[databaseTreeColumns.comment] = getComment(N, sql::getRoleComment(r[0].as<std::string>()));
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::fillEntityProperty(pqxx::nontransaction& N, Gtk::TreeModel::Row parent, Glib::ustring entityName, TreeModelColumnEntityType headingTypeId, std::string sql, TreeModelColumnEntityType typeId)
{
    Gtk::TreeModel::Row row = *(refTreeModel->append(parent.children()));
    row[databaseTreeColumns.entityName] = entityName;
    row[databaseTreeColumns.dataType] = Glib::ustring();
    row[databaseTreeColumns.entityTypeId] = headingTypeId;
    fillEntityProperty(N, row, entityName, sql, typeId);
}

void DbStudio::fillEntityProperty(pqxx::nontransaction& N, Gtk::TreeModel::Row propertiesrow, Glib::ustring entityName, std::string sql, TreeModelColumnEntityType typeId)
{
    try
    {
        pqxx::result rst(N.exec(sql));
        if(!rst.empty())
        {
            for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
            {
                if(!r[0].is_null())
                {
                    Gtk::TreeModel::Row row = *(refTreeModel->append(propertiesrow.children()));

                    row[databaseTreeColumns.entityName] = r[0].as<std::string>();
                    row[databaseTreeColumns.entityTypeId] = typeId;
                    
                    if(entityName == "Columns")
                    {
                        std::stringstream ss;
                        ss << r[1].as<std::string>() << (r[2].as<std::string>() == "t" ? " {Not Null}" : " {Null}");
                        row[databaseTreeColumns.dataType] = ss.str();
                        
                        Gtk::TreeModel::iterator iter = propertiesrow;
                        Gtk::TreePath path = refTreeModel->get_path(iter);
                        path.up();
                        iter = refTreeModel->get_iter(path);
                        Glib::ustring entity = (*iter)[databaseTreeColumns.entityName];
                        path.up();
                        path.up();
                        iter = refTreeModel->get_iter(path);
                        Glib::ustring schema = (*iter)[databaseTreeColumns.entityName];
                        row[databaseTreeColumns.comment] = getComment(N, sql::getTableColumnComment(schema, entity, r[0].as<std::string>()));
                    }
                    else
                        row[databaseTreeColumns.dataType] = Glib::ustring();
                }
            }
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

bool DbStudio::checkMapKey(std::map<std::string, std::string> m, std::string key)
{
    if(m.find(key) == m.end())
    {
        std::stringstream ss;
        ss << "Key " << key << " NOt found!\n\n";
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), ss.str());
        return false;
    }
    else
        return true;
}

void DbStudio::clearConnections(void)
{
    try
    {
        msgBox.show(*this, "Reading Database Structure");
        sqlTextView.grab_focus();
        databaseTreeView.unset_model();
        databaseTreeView.remove_all_columns();
        refTreeModel->clear();
        databaseTreeView.set_model(refTreeModel);
        databaseTreeView.append_column("Database", databaseTreeColumns.entityName);
        databaseTreeView.append_column(Glib::ustring(), databaseTreeColumns.dataType);
        cboActiveDatabase.remove_all();
        dbList.clear();
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::createConnectionExecute(pqxx::nontransaction& N, std::string sql)
{
    try
    {
        //create connection hostaddr=127.0.0.1; port = 5432 ; dbname = testdb;user=pi; password=testpwd;
        std::map<std::string, std::string> login = utility::deserializeKeyValue(sql.substr(18, sql.size() - 18));
        
        if(!checkMapKey(login, "hostaddr") || !checkMapKey(login, "port") || !checkMapKey(login, "dbname") || !checkMapKey(login, "user") || !checkMapKey(login, "password")) 
            return;
        
        DbServer *db = new DbServer(login.find("hostaddr")->second, login.find("port")->second, login.find("dbname")->second, login.find("user")->second, login.find("password")->second);
        pqxx::connection conn(db->connectionString());
        if(conn.is_open())
        {
            fillDatabaseTree(conn, db);
            dbList.addEnd(db);
            cboActiveDatabase.set_active(dbList.count() - 1);
            std::stringstream ss;
            ss << "Open connection " << login.find("hostaddr")->second << "." << login.find("dbname")->second << std::endl << std::endl;
            refMsgTextBuffer->insert(refMsgTextBuffer->begin(), ss.str());
        }
        else
        {
            delete db;
            std::stringstream ss;
            ss << "Unable to open connection " << login.find("hostaddr")->second << "." << login.find("dbname")->second << std::endl << std::endl;
            refMsgTextBuffer->insert(refMsgTextBuffer->begin(), ss.str());
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::refreshExecute(pqxx::nontransaction& N, std::string sql)
{
    try
    {
        int index = cboActiveDatabase.get_active_row_number();
        clearConnections();
        onRefreshDbStructure(index);  
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::useExecute(pqxx::nontransaction& N, std::string sql)
{
    try
    {
        int dbIndex = dbList.getServerDatabaseIndex(sql.substr(4, sql.size() - 4));
        if(dbIndex >= 0)
        {
            cboActiveDatabase.set_active(dbIndex);
            std::stringstream ss;
            ss << "Open connection " << sql.substr(4, sql.size() - 4) << std::endl << std::endl;
            refMsgTextBuffer->insert(refMsgTextBuffer->begin(), ss.str()); 
        }
        else
        {
           refMsgTextBuffer->insert(refMsgTextBuffer->begin(), "Unable to open connection\n"); 
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::selectExecute(pqxx::nontransaction& N, std::string sql)
{
    try
    {
        pqxx::result rst(N.exec(sql));
        if(!rst.empty())
        {
            //remove data from previous query
            sqlResultView.unset_model();    
            sqlResultView.remove_all_columns();
            sqlResultColumns.clear();
            refListModel.clear();
            for (unsigned int i = 0 ; i< sqlResultColumns.size() ; i++)
            {
                delete ((Gtk::TreeModelColumn<Glib::ustring> *)sqlResultColumns[i]);
            } 
            sqlResultColumns.clear();
            if(sqlColumns != nullptr)
                delete sqlColumns;
            
            //setup new query
            sqlColumns = new Gtk::TreeModel::ColumnRecord();
            Gtk::TreeModelColumn<Glib::ustring> *c;
            
            for(pqxx::row_size_type i = 0 ; i < rst.columns() ; i++)
            {
                c = new Gtk::TreeModelColumn<Glib::ustring>();
                sqlColumns->add(*c);
                sqlResultView.append_column(utility::strReplace(rst.column_name(i), "_", "__"), *c);
                sqlResultColumns.push_back(c);
            }
            refListModel = Gtk::TreeStore::create(*sqlColumns);         
            sqlResultView.set_model(refListModel);
            
            unsigned int rowCount = 0;
            for (pqxx::result::const_iterator dt = rst.begin() ; dt != rst.end() ; ++dt) 
            {
                if(++rowCount >= 10000)
                    break;
                Gtk::TreeModel::Row row = *(refListModel->append());
                for(pqxx::row_size_type i = 0 ; i < rst.columns() ; i++)
                {
                    //might need to format data types
                    if(!dt[i].is_null())
                        row.set_value(i, dt[i].as<std::string>());
                }
            }
            std::stringstream ss;
            unsigned long returnedRows = rst.size();
            ss << returnedRows << " Row" << (returnedRows == 1 ? " " : "s ") << "Returned\n\n";
            refMsgTextBuffer->insert(refMsgTextBuffer->begin(), ss.str());
            noteBook.set_current_page(1);   
        }
        else
            refMsgTextBuffer->insert(refMsgTextBuffer->begin(), "No Matching Records Found\n\n");   
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

bool DbStudio::getDbTreeInfo(Glib::ustring& serverName, Glib::ustring& databaseName, Glib::ustring& schemaName)
{
    auto refSelection = databaseTreeView.get_selection();
    if(refSelection)
    {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if(iter)
        {
            Gtk::TreePath path = refTreeModel->get_path(iter);

            std::vector<std::string> v = utility::strSplit(path.to_string(), ":");
            if(v.size() == 1)
                v.push_back("0");
            
            Gtk::TreeModel::Row row;
            Gtk::TreeModel::Children children = refTreeModel->children();
            Gtk::TreeModel::Children::iterator iter = children.begin();
            for(int i = 0 ; i < std::stoi(v[0]) ; ++i)
                ++iter;       
            serverName = (*iter)[databaseTreeColumns.entityName];
            
            row = *iter;
            children = row->children();
            iter = children.begin();
            for(int i = 0 ; i < std::stoi(v[1]) ; ++i)
                ++iter;  
            databaseName = (*iter)[databaseTreeColumns.entityName];
            
            if(v.size() == 2)
                schemaName = "public";
            else
            {
                row = *iter;
                children = row->children();
                iter = children.begin();
                for(int i = 0 ; i < std::stoi(v[2]) ; ++i)
                    ++iter;  
                schemaName = (*iter)[databaseTreeColumns.entityName];
            }
            return true;
        }
    }
    return false;
}

DbServer *DbStudio::getDbTreeInfo(Glib::ustring& serverName, Glib::ustring& databaseName, Glib::ustring& schemaName, Glib::ustring& entityName)
{
    auto refSelection = databaseTreeView.get_selection();
    if(refSelection)
    {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if(iter)
        {
            entityName = (*iter)[databaseTreeColumns.entityName];
            if(getDbTreeInfo(serverName, databaseName, schemaName))
                return dbList.searchServerDatabaseElement(serverName, databaseName);
        }
    }
    return nullptr;
}
Glib::ustring DbStudio::getSelectedEntityAncestor(int AncestorLevel)
{
    auto refSelection = databaseTreeView.get_selection();
    if(refSelection)
    {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if(iter)
        {
            Gtk::TreePath path = refTreeModel->get_path(iter);
            for(int i = 0 ; i < AncestorLevel ; ++i)
                path.up();
            iter = refTreeModel->get_iter(path);
            return (*iter)[databaseTreeColumns.entityName];
        }
    }
    return Glib::ustring ();
}

bool DbStudio::deleteSelectedTreeChildren(Gtk::TreeModel::Row& row)
{
    auto refSelection = databaseTreeView.get_selection();
    if(refSelection)
    {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if(iter)
        {
            std::vector<Gtk::TreePath> v;
            row = *iter;
            for(Gtk::TreeModel::iterator i = row.children().begin(); i != row.children().end(); ++i)
            {
                v.push_back(refTreeModel->get_path(i));
            }
            for(int p = v.size() - 1 ; p >= 0 ; p--)
            {
                Gtk::TreeModel::iterator i = refTreeModel->get_iter(v[p]);
                refTreeModel->erase(i);
            }
            return true;
        }
    }
    return false;
}

Glib::ustring DbStudio::getSqlTemplate(Glib::ustring filename)
{
    std::stringstream ss;
    ss << appPath << "/SqlTemplates/" << filename;
    std::ifstream file(ss.str());
    std::string s;
    ss.str(std::string());
    
    while (std::getline(file, s))
    {
        ss << s << std::endl;
    }
    return ss.str();
}

Glib::ustring DbStudio::getSqlTemplate(Glib::ustring filename, std::map<std::string, std::string>& m)
{
    std::string s = getSqlTemplate(filename);
    for( auto const& [key, val] : m )
    {
        s = utility::strReplace(s, key, val);
    }
    return s;
}

std::string DbStudio::getSaveAsFilename(std::string title, Glib::RefPtr<Gtk::FileFilter> filter)
{
    Gtk::FileChooserDialog dialog(title, Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*this);

    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Save", Gtk::RESPONSE_OK);
    dialog.set_do_overwrite_confirmation();

    dialog.add_filter(filter);

    Glib::RefPtr<Gtk::FileFilter> filter_any = Gtk::FileFilter::create();
    filter_any->set_name("Any files");
    filter_any->add_pattern("*");
    dialog.add_filter(filter_any);

    if(dialog.run() == Gtk::RESPONSE_OK)
        return dialog.get_filename();
    return std::string();
}

std::string DbStudio::getOpenFilename(std::string title, Glib::RefPtr<Gtk::FileFilter> filter)
{
    try
    {
        Gtk::FileChooserDialog dialog(title, Gtk::FILE_CHOOSER_ACTION_OPEN);
        dialog.set_transient_for(*this);

        dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
        dialog.add_button("_Open", Gtk::RESPONSE_OK);

        dialog.add_filter(filter);

        auto filter_any = Gtk::FileFilter::create();
        filter_any->set_name("Any files");
        filter_any->add_pattern("*");
        dialog.add_filter(filter_any);

        if(dialog.run() == Gtk::RESPONSE_OK)
            return dialog.get_filename();
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
    return std::string();
}

//Signal Handlers
void DbStudio::onUndoRedoUpdate(bool hasUndo, bool hasRedo)
{
    Gtk::Widget* mnu = refUIManager->get_widget("/ui/MenuBar/EditorMenu/EditorUndo");
    mnu->set_sensitive(hasUndo);
    mnu = refUIManager->get_widget("/ui/MenuBar/EditorMenu/EditorRedo");
    mnu->set_sensitive(hasRedo);
    
    Gtk::Widget* tool = refUIManager->get_widget("/ui/ToolBar/EditorUndo");
    tool->set_sensitive(hasUndo);
    tool = refUIManager->get_widget("/ui/ToolBar/EditorRedo");
    tool->set_sensitive(hasRedo);
}

void DbStudio::onTreeviewClick(GdkEventButton* button_event)
{
    auto refSelection = databaseTreeView.get_selection();
    if(refSelection)
    {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if(iter)
        {
            switch((*iter)[databaseTreeColumns.entityTypeId])
            {
                case TreeModelColumnEntityType::Server:
                    mnuServer.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::Database:
                    mnuDatabase.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::Schema:
                    break;
                    
                case TreeModelColumnEntityType::Tables:
                    mnuTables.popup_at_pointer((GdkEvent*)button_event);
                    break;

                case TreeModelColumnEntityType::Table:
                    mnuTable.popup_at_pointer((GdkEvent*)button_event);
                    break;

                case TreeModelColumnEntityType::TableColumns:
                    mnuTableColumns.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::TableColumn:
                    mnuTableColumn.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::TableConstraints:
                    mnuTableConstraints.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::TableConstraint:
                    mnuTableConstraint.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::TableForeignKeys:
                case TreeModelColumnEntityType::TableForeignKey:
                    break;
                    
                case TreeModelColumnEntityType::TableIndexs:
                    mnuIndexes.popup_at_pointer((GdkEvent*)button_event);
                    break;

                case TreeModelColumnEntityType::TableIndex:
                    mnuIndex.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::TableReferences:
                case TreeModelColumnEntityType::TableReference:
                    break;
                    
                case TreeModelColumnEntityType::TableTriggers:
                    mnuTriggers.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::TableTrigger:
                    mnuTrigger.popup_at_pointer((GdkEvent*)button_event);
                    break;
                 
                    
                case TreeModelColumnEntityType::ForeignTables:
                    mnuForeignTables.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::ForeignTable:
                    mnuForeignTable.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::ForeignTableColumns:
                case TreeModelColumnEntityType::ForeignTableColumn:                
                    break;

                case TreeModelColumnEntityType::Views:
                    mnuViews.popup_at_pointer((GdkEvent*)button_event);
                    break;
                
                case TreeModelColumnEntityType::View:
                    mnuView.popup_at_pointer((GdkEvent*)button_event);
                    break;
                
                case TreeModelColumnEntityType::ViewColumns:
                case TreeModelColumnEntityType::ViewColumn:
                    break;
                    
                case TreeModelColumnEntityType::ViewTriggers:
                    mnuTriggers.popup_at_pointer((GdkEvent*)button_event);
                    break;
                
                case TreeModelColumnEntityType::ViewTrigger:
                    mnuTrigger.popup_at_pointer((GdkEvent*)button_event);
                    break;

                case TreeModelColumnEntityType::MaterializedViews:
                    mnuMaterializedViews.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::MaterializedView:
                    mnuMaterializedView.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::MaterializedViewColumns:
                case TreeModelColumnEntityType::MaterializedViewColumn:
                    break;
                    
                case TreeModelColumnEntityType::MaterializedViewIndexes:
                    mnuIndexes.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::MaterializedViewIndex:
                    mnuIndex.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::Indexes:
                    mnuIndexList.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::Index:
                    mnuIndex.popup_at_pointer((GdkEvent*)button_event);
                    break;

                case TreeModelColumnEntityType::Functions:
                    mnuFunctions.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::Function:
                    mnuFunction.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::FunctionParameters:
                case TreeModelColumnEntityType::FunctionParameter:
                case TreeModelColumnEntityType::FunctionReturns:
                case TreeModelColumnEntityType::FunctionReturn:
                    break;
                    
                case TreeModelColumnEntityType::Sequences:
                    mnuSequences.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::Sequence:
                    mnuSequence.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::DataTypes:
                    mnuDataTypes.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::DataType:
                    mnuDataType.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::AggregateFunctions:
                    mnuAggregateFunctions.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::AggregateFunction:
                    mnuAggregateFunction.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::Users:
                    mnuUsers.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::User:
                    mnuUser.popup_at_pointer((GdkEvent*)button_event);
                    break;
                
                case TreeModelColumnEntityType::Roles:
                    mnuRoles.popup_at_pointer((GdkEvent*)button_event);
                    break;
                    
                case TreeModelColumnEntityType::Role:
                    mnuRole.popup_at_pointer((GdkEvent*)button_event);
                    break;
            }
        }
    }
}

void DbStudio::mnuServerCreateDatabase(void)
{
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("CreateDatabase.sql"));   
}

void DbStudio::mnuDatabaseCreateSchema(void)
{
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("CreateSchema.sql"));
}

void DbStudio::mnuDatabaseUse(void)
{
    Glib::ustring serverName, databaseName, schemaName;
    if(getDbTreeInfo(serverName, databaseName, schemaName))
    {
        std::stringstream ss;
        ss << "use " << serverName << "." << databaseName << std::endl << "go" << std::endl;
        refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
    }
}

//Table Menus
void DbStudio::mnuTablesCreate(void)
{
    Glib::ustring schemaName = getSelectedEntityAncestor(1);
    std::map<std::string,std::string> m{ {"<schema>", schemaName} };
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("CreateTable.sql", m));
}

void DbStudio::mnuTablesRefresh(void)
{
    refreshTreeBranch(&DbStudio::fillTables);
}

void DbStudio::mnuTableSelect(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, tableName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, tableName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                std::stringstream ss;
                ss << "select ";

                pqxx::nontransaction N(Conn);
                int colCount = 0;               
                pqxx::result rst(N.exec(sql::getColumn(schemaName, tableName)));  
                for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
                {
                    if(colCount++ > 0)
                        ss << ", ";
                    ss << r[0].as<std::string>();                               
                }
                ss << " from " << schemaName << "." << tableName << ";" << std::endl;
                refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
            }
         }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::mnuTableInsert(void)
{
    try
    {
       Glib::ustring serverName, databaseName, schemaName, tableName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, tableName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                std::stringstream ss, sv;
                ss << "insert into " << schemaName << "." << tableName << "(";

                pqxx::nontransaction N(Conn);
                int colCount = 0;               
                pqxx::result rst(N.exec(sql::getColumn(schemaName, tableName)));  
                for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
                {
                    if(colCount++ > 0)
                    {
                        ss << ", ";
                        sv << ", ";
                    }
                    ss << r[0].as<std::string>();
                    sv << (r[1].as<std::string>().substr(0, 4) == "char" || r[1].as<std::string>().substr(0, 4) == "text" ? "''" : (r[1].as<std::string>().substr(0, 4) == "time" ? "'yyyy-mm-dd'::timestamp::date" : "0"));
                }
                ss << ") VALUES (" << sv.str() << ");" << std::endl;
                refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
            }
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::mnuTableUpdate(void)
{
    try
    {
       Glib::ustring serverName, databaseName, schemaName, tableName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, tableName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                std::stringstream ss;
                ss << "update " << schemaName << "." << tableName << " set ";

                pqxx::nontransaction N(Conn);
                int colCount = 0;               
                pqxx::result rst(N.exec(sql::getColumn(schemaName, tableName)));  
                for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
                {
                    if(colCount++ > 0)
                        ss << ", ";
                    ss << r[0].as<std::string>() << " = " << (r[1].as<std::string>().substr(0, 4) == "char" || r[1].as<std::string>().substr(0, 4) == "text" ? "''" : (r[1].as<std::string>().substr(0, 4) == "time" ? "'yyyy-mm-dd'::timestamp::date" : "0"));
                }
                ss << " where ";
                colCount = 0;
                pqxx::result rPk(N.exec(sql::getTablePrimaryKey(schemaName, tableName)));  
                for (pqxx::result::const_iterator r = rPk.begin(); r != rPk.end(); ++r) 
                {
                    if(colCount++ > 0)
                        ss << " and ";
                    ss << r[0].as<std::string>() << " = " << (r[1].as<std::string>().substr(0, 4) == "char" || r[1].as<std::string>().substr(0, 4) == "text" ? "''" : (r[1].as<std::string>().substr(0, 4) == "time" ? "'yyyy-mm-dd'::timestamp::date" : "0"));                     
                }
                ss << ';' << std::endl; 
                refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
            }
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::mnuTableDelete(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, tableName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, tableName);
        if(db != nullptr)
        {
            std::stringstream ss;
            ss << "delete from " << schemaName << "." << tableName << " where ";                
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                pqxx::nontransaction N(Conn);
                int colCount = 0;               
                pqxx::result rst(N.exec(sql::getTablePrimaryKey(schemaName, tableName)));  
                for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
                {
                    if(colCount++ > 1)
                        ss << " and ";
                    ss << r[0].as<std::string>() << " = " << (r[1].as<std::string>().substr(0, 4) == "char" || r[1].as<std::string>().substr(0, 4) == "text" ? "''" : (r[1].as<std::string>().substr(0, 4) == "time" ? "'yyyy-mm-dd'::timestamp::date" : "0"));                     
                }
                ss << ';' << std::endl; 
                refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
            }
        }        
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::mnuTableDDL(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, tableName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, tableName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                pqxx::nontransaction N(Conn);
                
                std::stringstream ss;
                
                //main create table
                {
                    pqxx::result rst(N.exec(sql::getTableCreate(schemaName, tableName))); 
                    if(!rst.empty())
                    { 
                        pqxx::result::const_iterator r = rst.begin();
                        ss << utility::strReplace(r[0].as<std::string>(), " || '\n'::text", " || E'\\n'::text") << std::endl;
                    }
                }
                //add constraints
                {
                    pqxx::result rst(N.exec(sql::getTableCreateConstraint(schemaName, tableName)));  
                    for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
                    {
                         ss << "alter table " << schemaName << "." << tableName << " add constraint " << r[0].as<std::string>() << " " << r[1].as<std::string>() << ";" << std::endl;
                    }
                }
                //add indexes
                {
                    pqxx::result rst(N.exec(sql::getTableCreateIndex(schemaName, tableName)));  
                    for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
                    {
                         ss << r[0].as<std::string>() << ";" << std::endl;
                    }
                }
                refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
            }
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::mnuTableDrop(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, tableName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, tableName);
        if(db != nullptr)
        {
            std::stringstream ss;
            ss << "drop table " << schemaName << "." << tableName << " [CASCADE];" << std::endl;                    
            refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }    
}

//Table Columns Menu
void DbStudio::mnuTablesColumnCreate(void)
{
    Glib::ustring schemaName = getSelectedEntityAncestor(3);
    Glib::ustring tableName = getSelectedEntityAncestor(1);
    std::map<std::string,std::string> m{ {"<schema>", schemaName}, {"<table>", tableName} };
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("CreateTableColumn.sql", m));
}

void DbStudio::mnuTablesColumnRefresh(void)
{
    refreshTreeBranch("Columns", &sql::getColumn, TreeModelColumnEntityType::TableColumn);
}

void DbStudio::mnuTableColumnDDL(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, tableName, columnName;
        tableName = getSelectedEntityAncestor(2);
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, columnName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                pqxx::nontransaction N(Conn);
                
                std::stringstream ss;
                pqxx::result rst(N.exec(sql::getTableColumnDDL(schemaName, tableName, columnName))); 
                if(!rst.empty())
                {
                    pqxx::result::const_iterator r = rst.begin(); 

                    ss << "alter table " << schemaName << "." << tableName << std::endl << "add column " << columnName << " " << r[0].as<std::string>();
                    if(!r[1].is_null())
                        ss << " (" << r[1].as<std::string>() << ")";
                    
                    ss << ((r[2].as<std::string>() == "NO") ? " not null " : " null ");    
                    
                    if(!r[3].is_null())
                        ss << " default " << r[3].as<std::string>() << ";" << std::endl;
                    else
                        ss << utility::strReplace(r[4].as<std::string>(), " || '\n'::text", " || E'\\n'::text") << ";" << std::endl; 
                }
                refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
            }
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::mnuTableColumnDrop(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, tableName, columnName;
        tableName = getSelectedEntityAncestor(2);
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, columnName);
        if(db != nullptr)
        {
            std::stringstream ss;
            ss << "alter table " << schemaName << "." << tableName << " drop column " << columnName << ";" << std::endl;                    
            refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }    
}

//Table Constraits Menu
void DbStudio::mnuTablesConstraintCreate(void)
{
    Glib::ustring schemaName = getSelectedEntityAncestor(3);
    Glib::ustring tableName = getSelectedEntityAncestor(1);
    std::map<std::string,std::string> m{ {"<schema>", schemaName}, {"<table>", tableName} };
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("CreateTableConstraint.sql", m));
}

void DbStudio::mnuTablesConstraintRefresh(void)
{
    refreshTreeBranch("Constraints", &sql::getTableConstraint, TreeModelColumnEntityType::TableConstraint);
}

void DbStudio::mnuTableConstraintDDL(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, tableName, constraintName;
        tableName = getSelectedEntityAncestor(2);
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, constraintName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                pqxx::nontransaction N(Conn);
                
                std::stringstream ss;
                pqxx::result rst(N.exec(sql::getTableConstraintDDL(schemaName, tableName, constraintName)));  
                if(!rst.empty())
                {
                    pqxx::result::const_iterator r = rst.begin();
                    ss << "alter table " << schemaName << "." << tableName << " add constraint " << r[0].as<std::string>() << " " << r[1].as<std::string>() << ";" << std::endl;
                } 
                refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
            }
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }    
}

void DbStudio::mnuTableConstraintDrop(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, tableName, constraintName;
        tableName = getSelectedEntityAncestor(2);
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, constraintName);
        if(db != nullptr)
        {
            std::stringstream ss;
            ss << "alter table " << schemaName << "." << tableName << " drop constraint " << constraintName << ";" << std::endl;                    
            refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }    
}

//Table Menu Indexes
void DbStudio::mnuIndexCreate(void)
{
    Glib::ustring schemaName = getSelectedEntityAncestor(3);
    Glib::ustring tableName = getSelectedEntityAncestor(1);
    std::map<std::string,std::string> m{ {"<schema>", schemaName}, {"<table>", tableName} };
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("CreateTableIndex.sql", m));   
}

void DbStudio::mnuIndexRefresh(void)
{
    refreshTreeBranch("Indexes", &sql::getIndex, TreeModelColumnEntityType::TableIndex);
}

void DbStudio::mnuIndexDDL(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, tableName, indexName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, indexName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                pqxx::nontransaction N(Conn);
                
                std::stringstream ss;
                pqxx::result rst(N.exec(sql::getIndexDDL(schemaName, indexName))); 
                if(!rst.empty())
                {
                    pqxx::result::const_iterator r = rst.begin(); 
                    ss << r[0].as<std::string>() << ";" << std::endl;
                } 
                refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
            }
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }    
}

void DbStudio::mnuIndexDrop(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, indexName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, indexName);
        if(db != nullptr)
        {
            std::stringstream ss;
            ss << "drop index [ concurrently ] " << schemaName << "." << indexName << "[cascade | restrict];" << std::endl;                 
            refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

//Table Menu Triggers
void DbStudio::mnuTiggerCreate(void)
{
    Glib::ustring schemaName = getSelectedEntityAncestor(3);
    Glib::ustring tableName = getSelectedEntityAncestor(1);
    std::map<std::string,std::string> m{ {"<schema>", schemaName}, {"<table>", tableName} };
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("CreateTrigger.sql", m));
}

void DbStudio::mnuTiggerRefresh(void)
{
    refreshTreeBranch("Triggers", &sql::getTrigger, TreeModelColumnEntityType::TableTrigger);
}

void DbStudio::mnuTiggerDDL(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, tableName, triggerName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, triggerName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                pqxx::nontransaction N(Conn);
                
                std::stringstream ss;
                pqxx::result rst(N.exec(sql::getTriggerDDL(schemaName, triggerName))); 
                if(!rst.empty())
                {
                    //trigger_schema, trigger_name, event_object_schema, event_object_table, action_condition, action_statement, action_orientation, action_timing, event
                    pqxx::result::const_iterator r = rst.begin();
                    ss << "create trigger " << r[1].as<std::string>() << std::endl;
                    ss << r[7].as<std::string>() << " " << r[8].as<std::string>() << std::endl;
                    ss << "on " << r[2].as<std::string>() << "." << r[3].as<std::string>() << std::endl;
                    ss << "for each " << r[6].as<std::string>() << std::endl;
                    if(!r[4].is_null())
                    ss << "when " << r[4].as<std::string>() << std::endl;
                    ss << r[5].as<std::string>() << ";" << std::endl;
                } 
                refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
            }
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }    
}

void DbStudio::mnuTiggerDrop(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, tableName, triggerName;
        tableName = getSelectedEntityAncestor(2);
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, triggerName);
        if(db != nullptr)
        {
            std::stringstream ss;
            ss << "drop trigger " << triggerName << " on " << schemaName << "." << tableName << " [cascade | restrict];" << std::endl;                  
            refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }   
}

//Foreign Tables Menu
void DbStudio::mnuForeignTablesCreate(void)
{
    Glib::ustring schemaName = getSelectedEntityAncestor(1);
    std::map<std::string,std::string> m{ {"<schema>", schemaName} };
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("CreateForeignTable.sql", m)); 
}

void DbStudio::mnuForeignTablesRefresh(void)
{
    refreshTreeBranch(&DbStudio::fillForeignTables);
}

void DbStudio::mnuForeignTableDrop(void)
{
    Glib::ustring tableName = getSelectedEntityAncestor(0);
    Glib::ustring schemaName = getSelectedEntityAncestor(2);
    std::map<std::string,std::string> m{ {"<schema>", schemaName}, {"<table>", tableName} };
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("DropForeignTable.sql", m));
}

//Views Menu
void DbStudio::mnuViewsCreate(void)
{
    Glib::ustring schemaName = getSelectedEntityAncestor(1);
    std::map<std::string,std::string> m{ {"<schema>", schemaName} };
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("CreateView.sql", m));
}

void DbStudio::mnuViewsRefresh(void)
{
    refreshTreeBranch(&DbStudio::fillViews);
}

void DbStudio::mnuViewSelect(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, viewName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, viewName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                std::stringstream ss;
                ss << "select ";

                pqxx::nontransaction N(Conn);
                int colCount = 0;               
                pqxx::result rst(N.exec(sql::getColumn(schemaName, viewName)));  
                for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
                {
                    if(colCount++ > 0)
                        ss << ", ";
                    ss << r[0].as<std::string>();                               
                }
                ss << " from " << schemaName << "." << viewName << ";" << std::endl;
                refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
            }
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::mnuViewDrop(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, viewName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, viewName);
        if(db != nullptr)
        {
            std::stringstream ss;
            ss << "drop view " << schemaName << "." << viewName << " [cascade | restrict];" << std::endl;                   
            refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }    
}

void DbStudio::mnuViewDDL(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, viewName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, viewName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                pqxx::nontransaction N(Conn);
                
                std::stringstream ss;
                
                pqxx::result rst(N.exec(sql::getViewCreate(schemaName, viewName)));  
                if(!rst.empty())
                {
                    pqxx::result::const_iterator r = rst.begin();
                    ss << r[0].as<std::string>() << std::endl << r[1].as<std::string>() << std::endl;
                }
                
                refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
            }
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }    
}

//Materialized Views Menu
void DbStudio::mnuMaterializedViewsCreate(void)
{
    Glib::ustring schemaName = getSelectedEntityAncestor(1);
    std::map<std::string,std::string> m{ {"<schema>", schemaName} };
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("CreateMaterializedView.sql", m));
}
void DbStudio::mnuMaterializedViewsRefresh(void)
{
    refreshTreeBranch(&DbStudio::fillMaterializedViews);
}

void DbStudio::mnuMaterializedViewDrop(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, viewName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, viewName);
        if(db != nullptr)
        {
            std::stringstream ss;
            ss << "drop materialized view " << schemaName << "." << viewName << " [cascade | restrict];" << std::endl;                  
            refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::mnuMaterializedViewDDL(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, viewName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, viewName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                pqxx::nontransaction N(Conn);
                
                std::stringstream ss;
                
                pqxx::result rst(N.exec(sql::getMaterializedViewCreate(schemaName, viewName)));  
                if(!rst.empty())
                {
                    pqxx::result::const_iterator r = rst.begin();
                     ss << r[0].as<std::string>() << std::endl << r[1].as<std::string>() << std::endl;
                }

                refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
            }
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }    
}

//Index List Menu
void DbStudio::mnuIndexListRefresh(void)
{
    refreshTreeBranch(&DbStudio::fillIndexList);
}

//Function Menu
void DbStudio::mnuFunctionsCreate(void)
{
    Glib::ustring schemaName = getSelectedEntityAncestor(1);
    std::map<std::string,std::string> m{ {"<schema>", schemaName} };
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("CreateFunction.sql", m));
}

void DbStudio::mnuProceduresCreate(void)
{
    Glib::ustring schemaName = getSelectedEntityAncestor(1);
    std::map<std::string,std::string> m{ {"<schema>", schemaName} };
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("CreateProcedure.sql", m));
}

void DbStudio::mnuFunctionsRefresh(void)
{
    refreshTreeBranch(&DbStudio::fillFunctions);
}

void DbStudio::mnuFunctionDDL(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, functionName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, functionName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                pqxx::nontransaction N(Conn);
                
                std::stringstream ss;
                
                pqxx::result rst(N.exec(sql::getFunctionCreate(schemaName, functionName))); 
                if(!rst.empty())
                {
                    pqxx::result::const_iterator r = rst.begin();
                    //arguments, returns, definition, language, attributes, comment, sql_identifier
                    bool b = (r[1].as<std::string>() == "void");
                    ss << "create or replace " << (b ? "procedure " : "function ") << schemaName << "." << functionName << " (" << r[0].as<std::string>() << ")" << std::endl;
                    if(b)
                        ss << "as $$";
                    else
                        ss << "returns " << r[1].as<std::string>() << " as $$";
                    ss << r[2].as<std::string>();
                    ss << "$$ language " << r[3].as<std::string>() << " " << (!b ? r[4].as<std::string>() : std::string()) << ";" << std::endl;
                    if(!r[5].is_null())
                    ss << "comment on " << (b ? "procedure " : "function ") << schemaName << "." << functionName << " is '" << r[5].as<std::string>() << "';" << std::endl;

                    refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
                }
            }
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }        
}
void DbStudio::mnuFunctionExec(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, functionName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, functionName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                pqxx::nontransaction N(Conn);
                
                std::stringstream ss;
                
                pqxx::result rst(N.exec(sql::getFunctionCreate(schemaName, functionName)));  
                if(!rst.empty())
                {
                    //arguments, returns, definition, language, attributes, comment, sql_identifier
                    pqxx::result::const_iterator r = rst.begin();
                    if(r[1].as<std::string>() == "void")
                        ss << "call " << schemaName << "." << r[6].as<std::string>() << ";" << std::endl;
                    else
                        ss << "select " << schemaName << "." << r[6].as<std::string>() << ";" << std::endl;
                    refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
                }
            }
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }    
}

void DbStudio::mnuFunctionDrop(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, functionName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, functionName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                pqxx::nontransaction N(Conn);
                
                std::stringstream ss;
                
                pqxx::result rst(N.exec(sql::getFunctionCreate(schemaName, functionName)));
                if(!rst.empty())
                {
                    //arguments, returns, definition, language, attributes, comment, sql_identifier
                    pqxx::result::const_iterator r = rst.begin();
                    if(r[1].as<std::string>() == "void")
                        ss << "drop procedure " << schemaName << "." << functionName << " [cascade | restrict];" << std::endl;
                    else
                        ss << "drop function " << schemaName << "." << functionName << " [cascade | restrict];" << std::endl;

                    refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
                }
            }
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

//Sequences Menu
void DbStudio::mnuSequencesRefresh(void)
{
    refreshTreeBranch(&DbStudio::fillSequences);
}

void DbStudio::mnuSequenceLastValue(void)
{
    auto refSelection = databaseTreeView.get_selection();
    if(refSelection)
    {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if(iter)
        {
            Glib::ustring entityName = (*iter)[databaseTreeColumns.entityName];
            Glib::ustring schema = getSelectedEntityAncestor(2);
            std::stringstream ss;
            ss << "select last_value from " << schema << "." << entityName << ";" << std::endl;
            refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
        }
    }
}

//Data Types Menu
void DbStudio::mnuDataTypesCreate(void)
{
    Glib::ustring schemaName = getSelectedEntityAncestor(1);
    std::map<std::string,std::string> m{ {"<schema>", schemaName} };
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("CreateDataType.sql", m));
}

void DbStudio::mnuDataDomainCreate(void)
{
    Glib::ustring schemaName = getSelectedEntityAncestor(1);
    std::map<std::string,std::string> m{ {"<schema>", schemaName} };
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("CreateDataDomain.sql", m));
}

void DbStudio::mnuDataTypesRefresh(void)
{
    refreshTreeBranch(&DbStudio::fillDataTypes);
}

void DbStudio::mnuDataTypeDDL(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, typeName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, typeName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                pqxx::nontransaction N(Conn);
                pqxx::result rst(N.exec(sql::getDataTypeDDL(schemaName, typeName))); 
                if(!rst.empty())
                {
                    pqxx::result::const_iterator r = rst.begin();
                    refSqlTextBuffer->insert(refSqlTextBuffer->end(), r[0].as<std::string>());
                }
                else
                {
                    std::stringstream ss;
                    pqxx::result rst(N.exec(sql::getDataDomainDDL(schemaName, typeName))); 
                    if(!rst.empty())
                    {
                        pqxx::result::const_iterator r = rst.begin();
                        ss << "create domain " << schemaName << "." << typeName << " as" << std::endl;
                        ss << r[0].as<std::string>() << " " << r[2].as<std::string>();
                        if(!r[3].is_null())
                            ss << " default " << r[3].as<std::string>();
                        if(!r[4].is_null())
                            ss << " " << r[4].as<std::string>();
                        ss << ";" << std::endl;    
                        refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
                    }
                }
            }
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }        
}

void DbStudio::mnuDataTypeDrop(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, typeName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, typeName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                std::stringstream ss;
                pqxx::nontransaction N(Conn);
                pqxx::result rst(N.exec(sql::getDataTypeDDL(schemaName, typeName))); 
                if(!rst.empty())
                {
                    ss << "drop type " << schemaName << "." << typeName << " [cascade | restrict];" << std::endl;
                    refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
                }
                else
                {
                    ss << "drop domain " << schemaName << "." << typeName << " [cascade | restrict];" << std::endl;
                    refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
                }
            }
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }        
}

//Aggregate Functions Menu
void DbStudio::mnuAggregateFunctionsCreate(void)
{
    Glib::ustring schemaName = getSelectedEntityAncestor(1);
    std::map<std::string,std::string> m{ {"<schema>", schemaName} };
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("CreateAggregateFunction.sql", m));
}

void DbStudio::mnuAggregateFunctionsRefresh(void)
{
    refreshTreeBranch(&DbStudio::fillAggregateFunctions);
}

void DbStudio::mnuAggregateFunctionDDL(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, tableName, func;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, func);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                pqxx::nontransaction N(Conn);
                
                std::stringstream ss;
                pqxx::result rst(N.exec(sql::getAggregateFunctionDDL(schemaName, func))); 
                if(!rst.empty())
                {
                    pqxx::result::const_iterator r = rst.begin(); 
                    ss << r[0].as<std::string>() << std::endl;
                } 
                refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
            }
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }    
}

void DbStudio::mnuAggregateFunctionDrop(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, typeName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, typeName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                std::stringstream ss;
                pqxx::nontransaction N(Conn);
                pqxx::result rst(N.exec(sql::getAggregateFunctionDrop(schemaName, typeName))); 
                if(!rst.empty())
                {
                    pqxx::result::const_iterator r = rst.begin();
                    ss << "drop aggregate " << r[0].as<std::string>() << " [cascade | restrict];" << std::endl;
                    refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
                }
            }
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }         
}

//User Menu
void DbStudio::mnuUsersCreate(void)
{
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("CreateUser.sql"));   
}

void DbStudio::mnuUsersRefresh(void)
{
    refreshTreeBranch(&DbStudio::fillUsers);
}

//Role Menu
void DbStudio::mnuRolesCreate(void)
{
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("CreateUser.sql")); 
}

void DbStudio::mnuRolesRefresh(void)
{
    refreshTreeBranch(&DbStudio::fillRoles);
}

void DbStudio::mnuRoleSummary(void)
{
    try
    {
        Glib::ustring serverName, databaseName, schemaName, roleName;
        DbServer *db = getDbTreeInfo(serverName, databaseName, schemaName, roleName);
        if(db != nullptr)
        {
            pqxx::connection Conn(db->connectionString());
            if (Conn.is_open())
            {
                std::stringstream ss;
                ss << "/*" << std::endl << "SUMMARY FOR " << roleName << std::endl << std::endl;
                pqxx::nontransaction N(Conn);
                pqxx::result rst(N.exec(sql::getRoleAttributes(roleName))); 
                if(!rst.empty())
                {
                    ss << "Attributes:";
                    pqxx::result::const_iterator r = rst.begin();
                    if(r[1].as<std::string>() == "t")
                        ss << " superuser";
                    if(r[2].as<std::string>() == "t")
                        ss << " inherit";
                    if(r[3].as<std::string>() == "t")
                        ss << " createrole";
                    if(r[4].as<std::string>() == "t")
                        ss << " createdb";
                    if(r[5].as<std::string>() == "t")
                    {
                        ss << " login";
                        if(r[6].as<std::string>() == "-1")
                            ss << " no connection limit";
                        else
                            ss << " connection limit of " << r[6].as<std::string>();
                    
                        if(!r[7].is_null())
                            ss << " valid until " << r[7].as<std::string>();
                    }
                    ss << std::endl << std::endl;
                    
                    if(r[8].as<std::string>() != "{}")
                        ss << "Member Of: " << r[8].as<std::string>() << std::endl << std::endl;
                }
                pqxx::result rstP(N.exec(sql::getRoleObjectPermissions(roleName))); 
                if(!rstP.empty())
                {
                    std::string type = std::string();
                    ss << "Permissions:";
                    for (pqxx::result::const_iterator r = rstP.begin(); r != rstP.end(); ++r) 
                    {
                        if(type != r[0].as<std::string>())
                        {
                            ss << std::endl << r[0].as<std::string>() << std::endl;
                            type = r[0].as<std::string>();
                        }
                        std::string t = r[1].as<std::string>();
                        utility::padTo(t, 60);
                        ss << "\t" << t << r[2].as<std::string>() << std::endl;
                    }
                }
                ss << "*/" << std::endl;
                refSqlTextBuffer->insert(refSqlTextBuffer->end(), ss.str());
            }
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }        
}

void DbStudio::mnuRoleDrop(void)
{
    Glib::ustring role = getSelectedEntityAncestor(0);
    std::map<std::string,std::string> m{ {"<role>", role} };
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("DropRole.sql", m));
}

//Main Menu
void DbStudio::mnuDatabaseOpenConnection(void)
{
    try
    {
        DbLogin dbLogin;
        if(dbLogin.getLogin(*this))
        {
            msgBox.show(*this, "Reading Database Structure");
            loginDetail ld = {dbLogin.getDatabaseName(), dbLogin.getUserName(), dbLogin.getPassword(),
                              dbLogin.getServerName(), dbLogin.getPort(), dbLogin.getConnectionString()};           
            loginData.push_back(ld);
            
            sigc::slot<bool> dbSlot = sigc::bind(sigc::mem_fun(*this,&DbStudio::onReadDbStructure), 0);
            Glib::signal_timeout().connect(dbSlot, 1000);
        }
    }
    catch(std::exception& e)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::mnuDatabaseRefreshConnection(void)
{
    try
    {
        int index = cboActiveDatabase.get_active_row_number();
        clearConnections();
        sigc::slot<bool> dbSlot = sigc::bind(sigc::mem_fun(*this,&DbStudio::onRefreshDbStructure), index);
        Glib::signal_timeout().connect(dbSlot, 1000);
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

bool DbStudio::askSaveData(void)
{
    Gtk::MessageDialog dialog(*this, "SQL Editor", false /* use_markup */, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
    dialog.set_secondary_text("Editor window text will be lost do you wish to continue.");

    int result = dialog.run();
    switch(result)
    {
        case(Gtk::RESPONSE_OK):
        {
            return true;
        }
        
        default:
        {
            return false;
        }
    } 
}

std::string DbStudio::getComment(pqxx::nontransaction& N, std::string sql)
{
    try
    {
        pqxx::result rst(N.exec(sql));
        if(!rst.empty())
        {
            pqxx::result::const_iterator r = rst.begin();
            if(!r[0].is_null())
                return r[0].as<std::string>();
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
    return std::string();
}

void DbStudio::mnuSqlEditorClear(void)
{
    if(!refSqlTextBuffer->get_modified() || askSaveData())
    {
       refSqlTextBuffer->set_text(Glib::ustring());
       refSqlTextBuffer->ClearUdoRedoStack(); 
       refSqlTextBuffer->set_modified(false);
       sqlFilename = std::string();
       onUndoRedoUpdate(false, false);
    }
}

void DbStudio::mnuSqlMsgClear(void)
{
    refMsgTextBuffer->set_text(Glib::ustring());
}

void DbStudio::mnuSqlEditorUndo(void)
{
    refSqlTextBuffer->Undo();
}

void DbStudio::mnuSqlEditorRedo(void)
{
    refSqlTextBuffer->Redo();
}

void DbStudio::mnuSqlEditorFind(void)
{
    if(gridFind.is_visible())
        gridFind.hide();
    else
    {
        gridFind.show();
        txtFind.grab_focus();
    }
}

void DbStudio::mnuSqlEditorToggle(void)
{
    try
    {
        Gtk::TextBuffer::iterator start, end;
        refSqlTextBuffer->get_selection_bounds(start, end);
        std::string s = refSqlTextBuffer->get_text(start, end);
        if(isupper(s.at(0)))
            utility::strToLower(s);
        else
            utility::strToUpper(s);
        Gtk::TextBuffer::iterator iter = refSqlTextBuffer->erase(start, end);
        int index = iter.get_offset();
        refSqlTextBuffer->insert(iter, s);
        refSqlTextBuffer->select_range(refSqlTextBuffer->get_iter_at_offset(index), refSqlTextBuffer->get_iter_at_offset(index + s.length()));
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::mnuSqlEditorOpen(void)
{
    if(!refSqlTextBuffer->get_modified() || askSaveData())
    {
        auto filter = Gtk::FileFilter::create();
        filter->set_name("SQL files");
        filter->add_pattern("*.sql");
        
        std::string filename = getOpenFilename("Please choose an SQL file", filter);
        if(!filename.empty())
        {
            try
            {
                std::ifstream file(filename);
                std::string content;
                std::stringstream ss;
                
                while (std::getline(file, content))
                {
                    ss << content << std::endl;
                }
                refSqlTextBuffer->set_text(ss.str());
                refSqlTextBuffer->set_modified(false);
                sqlFilename = filename;
            }
            catch (const std::exception &e) 
            {
                refMsgTextBuffer->insert(refMsgTextBuffer->begin(), "Failed To Open SQL File.");
            }
        }
    }
}

void DbStudio::mnuSqlEditorSave(void)
{
    if(sqlFilename.empty())
        mnuSqlEditorSaveAs();
    else
    {
        std::ofstream file;
        file.open (sqlFilename);
        file << refSqlTextBuffer->get_text();
        file.close();
        refSqlTextBuffer->set_modified(false);
    }
}

void DbStudio::mnuSqlEditorSaveAs(void)
{
    auto filter = Gtk::FileFilter::create();
    filter->set_name("SQL files");
    filter->add_pattern("*.sql");

    sqlFilename = getSaveAsFilename("Please enter SQL filename", filter);
    
    if(!sqlFilename.empty())
        mnuSqlEditorSave();
}

void DbStudio::mnuSqlEditorExecute(void)
{
    try 
    {
        Glib::RefPtr <Gdk::Window> mainWindow = get_window();
        Glib::RefPtr<Gdk::Cursor> m_Cursor = Gdk::Cursor::create(get_display(), "wait");
        mainWindow->set_cursor(m_Cursor);

        if(cboActiveDatabase.get_active_row_number() != -1 && refSqlTextBuffer->get_char_count() > 0)
        {               
            std::string sql;
            
            if(refSqlTextBuffer->get_has_selection())
            {
                Gtk::TextBuffer::iterator a, b;
                refSqlTextBuffer->get_selection_bounds(a, b);
                sql = refSqlTextBuffer->get_text(a, b);
            }
            else
            {
                sql = refSqlTextBuffer->get_text();
            }

            std::vector<std::string> v = utility::strSplit(std::string(sql), "\ngo\n");
            for (std::string &i : v ) 
            {
                DbServer *db = dbList[cboActiveDatabase.get_active_row_number()];
                pqxx::connection Conn(db->connectionString());
                if (Conn.is_open())
                {
                    pqxx::nontransaction N(Conn);
                    i = utility::strTrim(i);
                    bool processed = false;
                    for (script_map::iterator it = execSql.begin(); it != execSql.end(); it++)
                    {
                        if(it->first == utility::strToLower(i.substr(0, it->first.length())))
                        {
                            (*this.*it->second)(N, i);
                            processed = true;
                            break;
                        }
                    }
                    if(!processed && i.length() > 0)
                    {
                        pqxx::result r(N.exec(i));
                        unsigned long affectedRows = r.affected_rows();
                        if(affectedRows == 0)
                            refMsgTextBuffer->insert(refMsgTextBuffer->begin(), i.substr(0, 40) += "...Completed Successfully\n\n");
                        else
                        {
                            std::stringstream ss;
                            ss << affectedRows << " Record" << (affectedRows == 1 ? " " : "s ") << "Successfully Updated\n\n";
                            refMsgTextBuffer->insert(refMsgTextBuffer->begin(), ss.str());
                        }
                    }
                }
            }
        }
        mainWindow->set_cursor();
    }
    catch (const std::exception &e) 
    {
        Glib::RefPtr <Gdk::Window> mainWindow = get_window();
        mainWindow->set_cursor();
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::mnuData(void)
{
    bool b = false;
    if(refSqlTextBuffer->get_char_count() > 0)
    {               
        if(refSqlTextBuffer->get_has_selection())
        {
            Gtk::TextBuffer::iterator start, end;
            refSqlTextBuffer->get_selection_bounds(start, end);
            std::string sql = refSqlTextBuffer->get_text(start, end);
            sql = utility::strTrim(sql);
            std::string s = sql.substr(0, 7);
            utility::strToLower(s);
            if(s == "select ")
                b = true;
        }
    }

    Gtk::Widget* mnu = refUIManager->get_widget("/ui/MenuBar/DataMenu/JsonExport");
    mnu->set_sensitive(b);
    mnu = refUIManager->get_widget("/ui/MenuBar/DataMenu/CsvExport");
    mnu->set_sensitive(b);
    mnu = refUIManager->get_widget("/ui/MenuBar/DataMenu/XmlExport");
    mnu->set_sensitive(b);
}

void DbStudio::mnuEditor(void)
{
    bool b = false;
    if(refSqlTextBuffer->get_char_count() > 0 && refSqlTextBuffer->get_has_selection())
        b = true;
 
    Gtk::Widget* mnu = refUIManager->get_widget("/ui/MenuBar/EditorMenu/EditorToggle");
    mnu->set_sensitive(b);
}

void DbStudio::mnuDataImportJSONFile(void)
{
    auto filter = Gtk::FileFilter::create();
    filter->set_name("JSON files");
    filter->add_pattern("*.json");
    
    std::string filename = getOpenFilename("Please choose an JSON file", filter);
    if(!filename.empty())
    {
        std::map<std::string,std::string> m{ {"<filename>", filename} };
        refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("ImportJSON.sql", m));  
    }
}

void DbStudio::mnuDataImportJSONString(void)
{
    refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("Import JSON From Text.sql"));
}

void DbStudio::mnuDataImportCSVFile(void)
{
    auto filter = Gtk::FileFilter::create();
    filter->set_name("CSV files");
    filter->add_pattern("*.csv");
    
    std::string filename = getOpenFilename("Please choose an CSV file", filter);
    if(!filename.empty())
    {
        std::map<std::string,std::string> m{ {"<filename>", filename} };
        refSqlTextBuffer->insert(refSqlTextBuffer->end(), getSqlTemplate("ImportCSV.sql", m)); 
    }
}

void DbStudio::mnuDataExportJSONFile(void)
{
    try
    {
        Gtk::TextBuffer::iterator start, end;
        refSqlTextBuffer->get_selection_bounds(start, end);
        Glib::ustring sql = refSqlTextBuffer->get_text(start, end);
        sql = utility::strTrim(utility::strReplace(sql, ";", std::string()));
            
        auto filter = Gtk::FileFilter::create();
        filter->set_name("JSON files");
        filter->add_pattern("*.json");

        std::string jsonFilename = getSaveAsFilename("Please enter JSON filename", filter);
        
        if(!jsonFilename.empty())
        {
            std::stringstream ss;
            ss << "copy (select row_to_json(t) from (" << sql << ") t ) to '" << jsonFilename << "';";
            Gtk::TextBuffer::iterator iter = refSqlTextBuffer->erase(start, end);
            refSqlTextBuffer->insert(iter, ss.str());
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::mnuDataExportCSVFile(void)
{
    try
    {
        Gtk::TextBuffer::iterator start, end;
        refSqlTextBuffer->get_selection_bounds(start, end);
        Glib::ustring sql = refSqlTextBuffer->get_text(start, end);
        sql = utility::strTrim(utility::strReplace(sql, ";", std::string()));
            
        auto filter = Gtk::FileFilter::create();
        filter->set_name("CSV files");
        filter->add_pattern("*.csv");

        std::string csvFilename = getSaveAsFilename("Please enter CSV filename", filter);
        
        if(!csvFilename.empty())
        {
            std::stringstream ss;
            ss << "copy (" << sql << ") to '" << csvFilename << "' delimiter ',' csv header;";
            Gtk::TextBuffer::iterator iter = refSqlTextBuffer->erase(start, end);
            refSqlTextBuffer->insert(iter, ss.str());
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::mnuDataExportXMLFile(void)
{
    try
    {
        Gtk::TextBuffer::iterator start, end;
        refSqlTextBuffer->get_selection_bounds(start, end);
        std::string sql = refSqlTextBuffer->get_text(start, end);
        DbServer *db = dbList[cboActiveDatabase.get_active_row_number()];
        pqxx::connection Conn(db->connectionString());
        if (Conn.is_open())
        {
            pqxx::nontransaction N(Conn);
            pqxx::result rst(N.exec(sql));
            if(!rst.empty())
            {
                auto filter = Gtk::FileFilter::create();
                filter->set_name("XML files");
                filter->add_pattern("*.xml");

                std::string xmlFilename = getSaveAsFilename("Please enter XML filename", filter);
                
                if(!xmlFilename.empty())
                {
                    excel::XmlWriter xml(xmlFilename);
                    xml.AddSheet("Data Dump");

                    //Add Styles
                    std::map<std::string, std::string> styles;
                    styles["title"] = xml.AddStyle(excel::TitleStyle().getStyle());
                    for(pqxx::row_size_type i = 0 ; i < rst.columns() ; i++)
                    {
                        switch (rst.column_type(i))
                        {
                            case pqxxDataType::T_char:
                            case pqxxDataType::T_text:
                            case pqxxDataType::T__char:
                            case pqxxDataType::T__text:
                            case pqxxDataType::T__varchar:
                            case pqxxDataType::T_bpchar:
                            case pqxxDataType::T_varchar:
                            case pqxxDataType::T__cstring:
                            case pqxxDataType::T_cstring:
                                {
                                    xml.Sheet(0)->AddColumnDef(excel::XmlWriterColumn(i, 60));
                                    auto found = styles.find("string");
                                    if(found == styles.end()) 
                                        styles["string"] = xml.AddStyle(excel::BaseStyle().getStyle());
                                }
                                break;

                            case pqxxDataType::T_date:
                            case pqxxDataType::T__date:
                                {
                                    xml.Sheet(0)->AddColumnDef(excel::XmlWriterColumn(i, 15));
                                    auto found = styles.find("date");
                                    if(found == styles.end()) 
                                        styles["date"] = xml.AddStyle(excel::DateStyle().getStyle());
                                }
                                break;
                                
                            case pqxxDataType::T_timestamp:
                            case pqxxDataType::T__timestamp:
                            case pqxxDataType::T_timestamptz:
                            case pqxxDataType::T__timestamptz:
                                {
                                    xml.Sheet(0)->AddColumnDef(excel::XmlWriterColumn(i, 20));
                                    auto found = styles.find("datetime");
                                    if(found == styles.end()) 
                                        styles["datetime"] = xml.AddStyle(excel::DateTimeStyle().getStyle());
                                }
                                break;

                            case pqxxDataType::T_money:
                            case pqxxDataType::T__money:
                                {
                                    xml.Sheet(0)->AddColumnDef(excel::XmlWriterColumn(i, 15));
                                    auto found = styles.find("money");
                                    if(found == styles.end()) 
                                        styles["money"] = xml.AddStyle(excel::MoneyStyle().getStyle());
                                }
                                break;
                            
                            case pqxxDataType::T_int8:
                            case pqxxDataType::T_int2:
                            case pqxxDataType::T_int2vector:
                            case pqxxDataType::T_int4:
                            case pqxxDataType::T__int8:
                            case pqxxDataType::T__int2:
                            case pqxxDataType::T__int2vector:
                            case pqxxDataType::T__int4:
                                {
                                    xml.Sheet(0)->AddColumnDef(excel::XmlWriterColumn(i, 15));
                                    auto found = styles.find("integer");
                                    if(found == styles.end()) 
                                        styles["integer"] = xml.AddStyle(excel::IntegerStyle().getStyle());
                                }
                                break;
                                
                            case pqxxDataType::T_numeric:
                            case pqxxDataType::T__numeric:
                                {
                                    xml.Sheet(0)->AddColumnDef(excel::XmlWriterColumn(i, 20));
                                    auto found = styles.find("decimal");
                                    if(found == styles.end()) 
                                        styles["decimal"] = xml.AddStyle(excel::DecimalStyle().getStyle());
                                }
                                break;
                                
                            default:
                                {
                                    std::cout << rst.column_name(i) << ": " << rst.column_type(i) << std::endl;
                                    xml.Sheet(0)->AddColumnDef(excel::XmlWriterColumn(i, 15));
                                    auto found = styles.find("number");
                                    if(found == styles.end()) 
                                        styles["number"] = xml.AddStyle(excel::NumberStyle().getStyle());
                                }
                            break;
                        } 
                    }
                    unsigned int rowCount = 0;
                    //Add headers
                    for(pqxx::row_size_type i = 0 ; i < rst.columns() ; i++)
                    {
                        xml.Sheet(0)->Cell(rowCount, i)->Value(rst.column_name(i), styles["title"], "String");
                    }
                    //Add Data
                    rowCount++;
                    for (pqxx::result::const_iterator dt = rst.begin() ; dt != rst.end() ; ++dt) 
                    {
                        for(pqxx::row_size_type i = 0 ; i < rst.columns() ; i++)
                        {
                            if(!dt[i].is_null())
                            {
                                switch (rst.column_type(i))
                                {
                                    case pqxxDataType::T_char:
                                    case pqxxDataType::T_text:
                                    case pqxxDataType::T__char:
                                    case pqxxDataType::T__text:
                                    case pqxxDataType::T__varchar:
                                    case pqxxDataType::T_bpchar:
                                    case pqxxDataType::T_varchar:
                                    case pqxxDataType::T__cstring:
                                    case pqxxDataType::T_cstring:
                                        xml.Sheet(0)->Cell(rowCount, i)->Value(utility::escapeXml(dt[i].as<std::string>()), styles["string"], "String");
                                        break;

                                    case pqxxDataType::T_date:
                                    case pqxxDataType::T__date:
                                        xml.Sheet(0)->Cell(rowCount, i)->Value(std::to_string(utility::getExcelDate(dt[i].as<std::string>())), styles["date"], "Number");
                                        break;
                                        
                                    case pqxxDataType::T_timestamp:
                                    case pqxxDataType::T__timestamp:
                                    case pqxxDataType::T_timestamptz:
                                    case pqxxDataType::T__timestamptz:
                                        xml.Sheet(0)->Cell(rowCount, i)->Value(std::to_string(utility::getExcelDateTime(dt[i].as<std::string>())), styles["datetime"], "Number");
                                        break;

                                    case pqxxDataType::T_money:
                                    case pqxxDataType::T__money:
                                        xml.Sheet(0)->Cell(rowCount, i)->Value(utility::stripCurrencyDecoration(dt[i].as<std::string>()), styles["money"], "Number");
                                        break;
                                    
                                    case pqxxDataType::T_int8:
                                    case pqxxDataType::T_int2:
                                    case pqxxDataType::T_int2vector:
                                    case pqxxDataType::T_int4:
                                    case pqxxDataType::T__int8:
                                    case pqxxDataType::T__int2:
                                    case pqxxDataType::T__int2vector:
                                    case pqxxDataType::T__int4:
                                        xml.Sheet(0)->Cell(rowCount, i)->Value(dt[i].as<std::string>(), styles["integer"], "Number");
                                        break;
                                        
                                    case pqxxDataType::T_numeric:
                                    case pqxxDataType::T__numeric:
                                        xml.Sheet(0)->Cell(rowCount, i)->Value(utility::stripCurrencyDecoration(dt[i].as<std::string>()), styles["decimal"], "Number");
                                        break;
                                        
                                    default:
                                        std::cout << rst.column_name(i) << ": " << rst.column_type(i) << std::endl;
                                        xml.Sheet(0)->Cell(rowCount, i)->Value(dt[i].as<std::string>(), styles["number"], "Number");
                                    break;
                                }
                            }
                        }
                        rowCount++;
                    }
                    xml.WriteToFile();
                    std::stringstream ss;
                    ss << (rowCount - 1) << " Row" << (rowCount == 1 ? " " : "s ") << "Returned\n\n";
                    refMsgTextBuffer->insert(refMsgTextBuffer->begin(), ss.str());
                }
            }
            else
                refMsgTextBuffer->insert(refMsgTextBuffer->begin(), "No Matching Records Found\n\n");   
        }
    }
    catch(const std::exception &e) 
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), e.what());
    }
}

void DbStudio::mnuHelpAbout(void)
{
  aboutDialog.run();
}

void DbStudio::mnuStudioQuit(void)
{
    if(!refSqlTextBuffer->get_modified() || askSaveData())
    {
        hide(); //Closes the main window to stop the Gtk::Main::run().
    }
}

void DbStudio::onWindowOpen(void)
{
    maximize();
    sigc::slot<bool> dbSlot = sigc::bind(sigc::mem_fun(*this,&DbStudio::onOpen), 1);
    Glib::signal_timeout().connect(dbSlot, 1000);
    mnuDatabaseOpenConnection();
}

bool DbStudio::onExitClicked(GdkEventAny* event) 
{
    if(!refSqlTextBuffer->get_modified() || askSaveData())
        return false;
    else
        return true;
}

bool DbStudio::onQueryTooltop(int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip)
{
    Gtk::TreeModel::iterator iter;

    if(databaseTreeView.get_tooltip_context_iter(x, y, keyboard_tooltip, iter))
    {
        if((*iter)[databaseTreeColumns.comment] != std::string())
        {
            tooltip->set_markup("<span foreground=\"blue\"><b><i>" + (*iter)[databaseTreeColumns.comment] + "</i></b></span>");
            return true;
        }
    }
    return false;       
}

void DbStudio::onAboutDialogResponse(int response_id)
{
    switch (response_id)
    {
        case Gtk::RESPONSE_CLOSE:
        case Gtk::RESPONSE_CANCEL:
        case Gtk::RESPONSE_DELETE_EVENT:
            aboutDialog.hide();
            break;
            
        default:
            refMsgTextBuffer->insert(refMsgTextBuffer->begin(), "Unexpected response!");
            break;
    }
}

bool DbStudio::onReadDbStructure(int timerNumber)
{
    try
    {
        if (!loginData.empty())
        {
            loginDetail ld = loginData.back();

            pqxx::connection Conn(utility::strReplace(ld.connParam, "<dbName>", ld.dbname));
            if (Conn.is_open())
            {
                pqxx::nontransaction N(Conn);   
                pqxx::result rst(N.exec(sql::getDatabaseList()));  
                for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
                {
                    try
                    {
                        pqxx::connection c(utility::strReplace(ld.connParam, "<dbName>", r[0].as<std::string>()));
                        if(c.is_open())
                        {
                            DbServer *db = new DbServer(ld.hostaddr, ld.port, r[0].as<std::string>(), ld.user, ld.password);
                            fillDatabaseTree(c, db);
                            dbList.addEnd(db);
                        }
                    }
                    catch(std::exception& ex)
                    {
                        //silently fail if the user does not have permission on that Database
                        //refMsgTextBuffer->insert(refMsgTextBuffer->begin(), ex.what());
                    }
                }
                int i = dbList.getServerDatabaseIndex(ld.hostaddr + "." + ld.dbname);
                if(i == -1)
                    cboActiveDatabase.set_active(0);
                else
                {
                    cboActiveDatabase.set_active(i);
                }
                msgBox.hide();
            }
            else
            {
                loginData.erase(loginData.end());
            }
            std::stringstream ss;
            ss << "Successfully Logged into " << cboActiveDatabase.get_active_text() << std::endl;
            refMsgTextBuffer->insert(refMsgTextBuffer->begin(), ss.str() );
        }
    }
    catch(std::exception& ex)
    {
        refMsgTextBuffer->insert(refMsgTextBuffer->begin(), ex.what());
    }
    return false;
}

bool DbStudio::onRefreshDbStructure(int index)
{
    int numdb = 0;
    for (loginDetail &ld : loginData) 
    {
        try
        {
            pqxx::connection Conn(utility::strReplace(ld.connParam, "<dbName>", ld.dbname));
            if (Conn.is_open())
            {
                pqxx::nontransaction N(Conn);   
                pqxx::result rst(N.exec(sql::getDatabaseList()));
                for (pqxx::result::const_iterator r = rst.begin(); r != rst.end(); ++r) 
                {
                    try
                    {
                        pqxx::connection c(utility::strReplace(ld.connParam, "<dbName>", r[0].as<std::string>()));
                        if(c.is_open())
                        {
                            DbServer *db = new DbServer(ld.hostaddr, ld.port, r[0].as<std::string>(), ld.user, ld.password);
                            fillDatabaseTree(c, db);
                            dbList.addEnd(db);
                            numdb++;
                        }
                    }
                    catch(std::exception& ex)
                    {
                        //silently fail if the user does not have permission on that Database
                    }
                } 
            } 
            refMsgTextBuffer->insert(refMsgTextBuffer->begin(), "Refreshed connection\n\n");
        }   
        catch(std::exception& ex)
        {
            refMsgTextBuffer->insert(refMsgTextBuffer->begin(), ex.what());
        }
    }
    if(numdb > index)
        cboActiveDatabase.set_active(index);
    else
        if(numdb > 0)
            cboActiveDatabase.set_active(0);
    msgBox.hide();
    return false;
}

bool DbStudio::onOpen(int timerNumber)
{
    mainHPaned.set_position(mainHPaned.get_width() * 0.2);
    sqlPaned.set_position(sqlPaned.get_height() * 0.8); 
    return false;
}

void DbStudio::onFindFirst(void)
{
    if(!txtFind.get_text().empty())
    {
        std::string search = txtFind.get_text();
        unsigned int pos = refSqlTextBuffer->Find(search, 0);
        if(pos == std::string::npos)
        {
            std::stringstream ss;
            ss << "End Of Search For " << search << std::endl;
            refMsgTextBuffer->insert(refMsgTextBuffer->begin(), ss.str());
        }
        else
        {
            Gtk::TextBuffer::iterator Iter = refSqlTextBuffer->get_iter_at_offset(pos);
            sqlTextView.scroll_to(Iter, 0, 0, 0.5);
        }
    }
}

void DbStudio::onFindNext(void)
{
    if(!txtFind.get_text().empty())
    {
        std::string search = txtFind.get_text();
        unsigned int start = refSqlTextBuffer->property_cursor_position() + search.length();
        unsigned int pos = refSqlTextBuffer->Find(search, start);
        if(pos == std::string::npos)
        {
            std::stringstream ss;
            ss << "End Of Search For " << search << std::endl;
            refMsgTextBuffer->insert(refMsgTextBuffer->begin(), ss.str());
        }
        else
        {
            Gtk::TextBuffer::iterator Iter = refSqlTextBuffer->get_iter_at_offset(pos);
            sqlTextView.scroll_to(Iter, 0, 0, 0.5);
       }
    }
}

void DbStudio::onReplace(void)
{
    if(refSqlTextBuffer->get_has_selection())
    {
        Gtk::TextBuffer::iterator start, end;
        refSqlTextBuffer->get_selection_bounds(start, end);
        Gtk::TextBuffer::iterator iter = refSqlTextBuffer->erase(start, end);
        int index = iter.get_offset();
        refSqlTextBuffer->insert(iter, txtReplace.get_text());
        refSqlTextBuffer->select_range(refSqlTextBuffer->get_iter_at_offset(index), refSqlTextBuffer->get_iter_at_offset(index + txtReplace.get_text().length()));
    }
}

void DbStudio::onReplaceFind(void)
{
    onReplace();
    onFindNext();
}
