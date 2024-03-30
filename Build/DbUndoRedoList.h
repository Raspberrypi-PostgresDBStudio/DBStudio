#ifndef DBUNDOREDOLIST_H
#define DBUNDOREDOLIST_H

#include <gtkmm.h>

enum UndoRedoType
{
    Insert,
    Delete
};

class UndoRedoNode
{
    private:
        UndoRedoType type;
        int beginOffset;
        int endOffset;
        Glib::ustring text;
        UndoRedoNode *prev;
        
    public:
        UndoRedoNode(UndoRedoType t, int begin, int end, Glib::ustring s, UndoRedoNode *p);
        virtual ~UndoRedoNode(void);
        UndoRedoType EntryType(void) { return type; }
        int BeginOffset(void) { return beginOffset; }
        int EndOffset(void) { return endOffset; }
        Glib::ustring Text(void) { return text; }
        UndoRedoNode *PreviousNode(void) { return prev; }
        void SetPreviousNode(UndoRedoNode *p) { prev = p; }
};

class DbUndoRedoList
{
    private:
        UndoRedoNode *Undotos = nullptr;
        UndoRedoNode *Redotos = nullptr;
        void RedoPush(UndoRedoNode *n);
        int StackDepth = 0;
        
    public:
        DbUndoRedoList(void);
        virtual ~DbUndoRedoList(void);
        void UndoPush(UndoRedoType t, int begin, int end, Glib::ustring text);
        UndoRedoNode *UndoPop(void);
        UndoRedoNode *RedoPop(void);
        bool CanUndo(void) { return Undotos != nullptr; }
        bool CanRedo(void) { return Redotos != nullptr; }
        void ClearUndo(void);
        void ClearRedo(void);
};

#endif
