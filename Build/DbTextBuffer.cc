#include "DbTextBuffer.h"

DbTextBuffer::DbTextBuffer(void)
{
}

DbTextBuffer::~DbTextBuffer(void)
{
}

sigc::signal<void, bool, bool> DbTextBuffer::signal_on_undo_stack_update()
{
    return undoStackUpdate;
}

void DbTextBuffer::Undo(void)
{
    if(!UndoIsEmpty())
    {
        recordUndo = false;
        UndoRedoNode *n = DbUndoStack.UndoPop();
        if(n->EntryType() == UndoRedoType::Delete)
        {            
            insert(get_iter_at_offset(n->BeginOffset()), n->Text());
        }
        else
        {
            erase(get_iter_at_offset(n->BeginOffset()), get_iter_at_offset(n->EndOffset()));
        }
         undoStackUpdate.emit(DbUndoStack.CanUndo(), DbUndoStack.CanRedo());
         recordUndo = true;
    }
}

void DbTextBuffer::Redo(void)
{
    if(!RedoIsEmpty())
    {
        recordUndo = false;
        UndoRedoNode *n = DbUndoStack.RedoPop();
        if(n->EntryType() == UndoRedoType::Insert)
        {            
            insert(get_iter_at_offset(n->BeginOffset()), n->Text());
        }
        else
        {
            erase(get_iter_at_offset(n->BeginOffset()), get_iter_at_offset(n->EndOffset()));
        }
        undoStackUpdate.emit(DbUndoStack.CanUndo(), DbUndoStack.CanRedo());
        recordUndo = true;
    }
}
        
void DbTextBuffer::on_insert(const Gtk::TextBuffer::iterator& range_start, const Glib::ustring& text, int bytes)
{
    if(recordUndo)
    {
        int begin = range_start.get_offset();        
        Gtk::TextBuffer::on_insert(range_start, text, bytes);
        int end = get_iter_at_mark(get_insert()).get_offset();        
        DbUndoStack.UndoPush(UndoRedoType::Insert, begin, end, text);
        undoStackUpdate.emit(DbUndoStack.CanUndo(), DbUndoStack.CanRedo());
    }
    else
        Gtk::TextBuffer::on_insert(range_start, text, bytes);
}

void DbTextBuffer::on_erase (const TextBuffer::iterator& range_start, const TextBuffer::iterator& range_end)
{
    if(recordUndo)
    {
        int begin = range_start.get_offset();
        int end = range_end.get_offset();
        Glib::ustring text = get_text(range_start, range_end);
        DbUndoStack.UndoPush(UndoRedoType::Delete, begin, end, text);
        undoStackUpdate.emit(DbUndoStack.CanUndo(), DbUndoStack.CanRedo());
    }
    Gtk::TextBuffer::on_erase(range_start, range_end);
}

unsigned int DbTextBuffer::Find(std::string& search, unsigned int start)
{
    unsigned int pos = get_text().find(search, start);
    if(pos != std::string::npos)
    {
        select_range(get_iter_at_offset(pos), get_iter_at_offset(pos + search.length()));
        pos += search.length();
    }
    return pos;
}
