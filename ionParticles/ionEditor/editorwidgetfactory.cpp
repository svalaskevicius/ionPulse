#include "editorwidgetfactory.h"
#include "ionHeart/shared.h"


namespace IonEditor {
QMap<QString, QString> EditorWidgetFactory::fileTypes; // file ending -> file type in factories


LineNumberArea *EditorWidgetFactory::LineNumberArea::operator ()(IEditor *widget)
{
    return new IonEditor::LineNumberArea(widget);
}

QSyntaxHighlighter *EditorWidgetFactory::Highlighter::operator ()(IEditor *widget)
{
    return new IonEditor::Highlighter(widget->getEditorInstance());
}

QSyntaxHighlighter *EditorWidgetFactory::createHighlighter(IEditor *widget, QString filetype)
{
    QMap<QString, QSharedPointer<IHighlighter> >::const_iterator it = m_createHighlighterMap.find(filetype);
    if (it != m_createHighlighterMap.end()) {
        return (*(*it))(widget);
    }
    Highlighter _default;
    return _default(widget);
}
IEditorComponent *EditorWidgetFactory::createLineNumberArea(IEditor *widget, QString filetype)
{
    QMap<QString, QSharedPointer<ILineNumberArea> >::const_iterator it = m_createLineNumberAreaMap.find(filetype);
    if (it != m_createLineNumberAreaMap.end()) {
        return (*(*it))(widget);
    }
    LineNumberArea _default;
    return _default(widget);
}

IonHeart::IPanelWidget *EditorWidgetFactory::createEditor(QString path)
{
    IonEditor::EditorWidget *ret = new IonEditor::EditorWidget(path);
    QString type = getFileType(path);
    ret->setComponents(QList<IEditorComponent*>() << createLineNumberArea(ret, type));
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

void EditorWidgetFactory::registerFileType(QString fileExt, QString fileType) {
    fileTypes.insert(fileExt, fileType);
}
void EditorWidgetFactory::registerHighlighter(QString filetype, IEditorWidgetFactory::IHighlighter *highlighter) {
    m_createHighlighterMap[filetype] = QSharedPointer<IHighlighter>(highlighter);
}
void EditorWidgetFactory::registerLineNumberArea(QString filetype, IEditorWidgetFactory::ILineNumberArea *lineNumberArea) {
    m_createLineNumberAreaMap[filetype] = QSharedPointer<ILineNumberArea>(lineNumberArea);
}


}
