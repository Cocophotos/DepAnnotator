#ifndef COMMENTPLAINEDIT_H
#define COMMENTPLAINEDIT_H

#include <QPlainTextEdit>

class CommentPlainEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit CommentPlainEdit(QWidget *parent = 0);
    
signals:
    void loseFocus(QString oldc, QString newc);
    
protected:
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);

private:
    QString old;
};

#endif // COMMENTPLAINEDIT_H
