#include "DbUndoRedoList.h"

#define STACKMAXDEPTH 1000
#define STACKREDUCEDDEPTH 900

UndoRedoNode::UndoRedoNode(UndoRedoType t, int begin, int end, Glib::ustring s, UndoRedoNode *p)
{
    type = t;
    beginOffset = begin;
    endOffset = end;
    text = s;
    prev = p;
}

UndoRedoNode::~UndoRedoNode(void)
{
    
}

DbUndoRedoList::DbUndoRedoList(void)
{
    
}

DbUndoRedoList::~DbUndoRedoList(void)
{
    ClearUndo();
    ClearRedo();
}

void DbUndoRedoList::UndoPush(UndoRedoType type, int begin, int end, Glib::ustring text)
{
    if(CanRedo())
        ClearRedo();
    UndoRedoNode *temp = Undotos;
    Undotos = new UndoRedoNode(type, begin, end, text, temp);
    if(++StackDepth > STACKMAXDEPTH)
    {
        //find cut off node
        UndoRedoNode *temp = Undotos;
        for(int i = 0 ; i < STACKREDUCEDDEPTH ; i++)
        {
            temp = temp->PreviousNode();
        }
        //remove the last 100 entries
        UndoRedoNode *t = temp;
        temp = temp->PreviousNode();
        t->SetPreviousNode(nullptr);
        while(temp != nullptr)
        {
            t = temp;
            temp = t->PreviousNode();
            delete t;
            StackDepth--;
        }
    }
}

UndoRedoNode *DbUndoRedoList::UndoPop(void)
{
    UndoRedoNode *temp = Undotos;    
    Undotos = temp->PreviousNode();
    temp->SetPreviousNode(Redotos);
    Redotos = temp;
    StackDepth--;
    return Redotos;
}

UndoRedoNode *DbUndoRedoList::RedoPop(void)
{
    UndoRedoNode *temp = Redotos;
    Redotos = temp->PreviousNode();    
    temp->SetPreviousNode(Undotos);
    Undotos = temp;
    StackDepth++;
    return Undotos;
}

void DbUndoRedoList::ClearUndo(void)
{
    while(Undotos != nullptr)
    {
        UndoRedoNode *temp = Undotos;
        Undotos = temp->PreviousNode();
        delete temp;
        StackDepth = 0;
    }
}

void DbUndoRedoList::ClearRedo(void)
{
    while(Redotos != nullptr)
    {
        UndoRedoNode *temp = Redotos;
        Redotos = temp->PreviousNode();
        delete temp;
    }
}
