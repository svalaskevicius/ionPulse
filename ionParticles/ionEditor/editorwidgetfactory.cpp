#include "editorwidgetfactory.h"
#include "ionHeart/shared.h"


namespace IonEditor {
QMap<QString, QString> EditorWidgetFactory::fileTypes; // file ending -> file type in factories


LineNumberArea *EditorWidgetFactory::LineNumberArea::operator ()(EditorWidget *widget)
{
    return new IonEditor::LineNumberArea(widget);
}

Highlighter *EditorWidgetFactory::Highlighter::operator ()(EditorWidget *widget)
{
    return new IonEditor::Highlighter(widget);
}

Highlighter *EditorWidgetFactory::createHighlighter(EditorWidget *widget, QString filetype)
{
    QMap<QString, QSharedPointer<Highlighter> >::const_iterator it = m_createHighlighterMap.find(filetype);
    if (it != m_createHighlighterMap.end()) {
        return (*(*it))(widget);
    }
    Highlighter _default;
    return _default(widget);
}
LineNumberArea *EditorWidgetFactory::createLineNumberArea(EditorWidget *widget, QString filetype)
{
    QMap<QString, QSharedPointer<LineNumberArea> >::const_iterator it = m_createLineNumberAreaMap.find(filetype);
    if (it != m_createLineNumberAreaMap.end()) {
        return (*(*it))(widget);
    }
    LineNumberArea _default;
    return _default(widget);
}

IonEditor::EditorWidget *EditorWidgetFactory::createEditor(QString path)
{
    IonEditor::EditorWidget *ret = new IonEditor::EditorWidget(path);
    QString type = getFileType(path);
    ret->setComponents(QList<IonEditor::EditorWidget::Component *>() << createLineNumberArea(ret, type));
    ret->setHighlighter(createHighlighter(ret, type));
    return ret;
}

QString EditorWidgetFactory::getFileType(QString filePath)
{
    int pos = filePath.lastIndexOf('.');
    if (pos > 0) {
        QMap<QString, QString>::const_iterator it = fileTypes.find(filePath.right(filePath.length()-pos-1));
        if (it != fileTypes.end()) {
            return it.value();
        }
    }
    return "text";
}

}
