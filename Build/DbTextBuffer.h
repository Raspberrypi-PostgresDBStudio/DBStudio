#ifndef DBTEXTBUFFER_H
#define DBTEXTBUFFER_H

#include <gtkmm.h>
#include <sigc++/sigc++.h>
#include "DbUndoRedoList.h"

#define STACK_SIZE 1000

class DbTextBuffer : public Gtk::TextBuffer
{
    private:
        DbUndoRedoList DbUndoStack;
        bool recordUndo = true;
        sigc::signal<void, bool, bool> undoStackUpdate;

    protected:
        virtual void on_erase(const Gtk::TextBuffer::iterator& range_start, const Gtk::TextBuffer::iterator& range_end);
        virtual void on_insert(const Gtk::TextBuffer::iterator& pos, const Glib::ustring& text, int bytes);
        
    public:
        DbTextBuffer(void);
        virtual ~DbTextBuffer(void);
        void Undo(void);
        void Redo(void);
        bool UndoIsEmpty(void) {return !DbUndoStack.CanUndo(); }
        bool RedoIsEmpty(void) {return !DbUndoStack.CanRedo(); }
        sigc::signal<void, bool, bool> signal_on_undo_stack_update();
        void ClearUdoRedoStack(void) { DbUndoStack.ClearUndo(); DbUndoStack.ClearRedo(); }
        unsigned int Find(std::string& search, unsigned int start);
};

#endif
