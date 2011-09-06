#include "editorwidgetfactory.h"
#include "ionHeart/shared.h"


namespace IonEditor {

namespace Private {


QMap<QString, QString> EditorWidgetFactoryImpl::fileTypes; // file ending -> file type in factories


EditorComponent *EditorWidgetFactoryImpl::DefaultLineNumberArea::operator ()(Editor *widget)
{
    return new IonEditor::Private::LineNumberArea(widget);
}

QSyntaxHighlighter *EditorWidgetFactoryImpl::DefaultHighlighter::operator ()(Editor *widget)
{
    return new IonEditor::Private::Highlighter(widget->getEditorInstance());
}

QSyntaxHighlighter *EditorWidgetFactoryImpl::createHighlighter(Editor *widget, QString filetype)
{
    QMap<QString, QSharedPointer<Highlighter> >::const_iterator it = m_createHighlighterMap.find(filetype);
    if (it != m_createHighlighterMap.end()) {
        return (*(*it))(widget);
    }
    DefaultHighlighter _default;
    return _default(widget);
}
EditorComponent *EditorWidgetFactoryImpl::createLineNumberArea(Editor *widget, QString filetype)
{
    QMap<QString, QSharedPointer<LineNumberArea> >::const_iterator it = m_createLineNumberAreaMap.find(filetype);
    if (it != m_createLineNumberAreaMap.end()) {
        return (*(*it))(widget);
    }
    DefaultLineNumberArea _default;
    return _default(widget);
}

IonHeart::PanelWidget *EditorWidgetFactoryImpl::createEditor(QString path)
{
    EditorWidget *ret = new EditorWidget(path);
    QString type = getFileType(path);
    ret->setComponents(QList<EditorComponent*>() << createLineNumberArea(ret, type));
    ret->setHighlighter(createHighlighter(ret, type));
    return ret;
}

QString EditorWidgetFactoryImpl::getFileType(QString filePath)
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

void EditorWidgetFactoryImpl::registerFileType(QString fileExt, QString fileType) {
    fileTypes.insert(fileExt, fileType);
}
void EditorWidgetFactoryImpl::registerHighlighter(QString const & filetype, EditorWidgetFactoryImpl::Highlighter *highlighter) {
    m_createHighlighterMap[filetype] = QSharedPointer<Highlighter>(highlighter);
}
void EditorWidgetFactoryImpl::registerLineNumberArea(QString const & filetype, EditorWidgetFactoryImpl::LineNumberArea *lineNumberArea) {
    m_createLineNumberAreaMap[filetype] = QSharedPointer<LineNumberArea>(lineNumberArea);
}

}
}
