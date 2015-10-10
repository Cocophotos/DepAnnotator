#include "commentplainedit.h"

CommentPlainEdit::CommentPlainEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    old = "";
}

void CommentPlainEdit::focusInEvent(QFocusEvent *e)
{
    old = this->toPlainText();
    QPlainTextEdit::focusInEvent(e);
}

void CommentPlainEdit::focusOutEvent(QFocusEvent *e)
{
    if(QString::compare(old, this->toPlainText()) != 0)
    {
        emit loseFocus(old, this->toPlainText());
        old = this->toPlainText();
    }

    QPlainTextEdit::focusOutEvent(e);
}
