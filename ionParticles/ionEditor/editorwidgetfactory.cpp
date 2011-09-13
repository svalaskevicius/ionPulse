/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "editorwidgetfactory.h"
#include "ionHeart/shared.h"


namespace IonEditor {

namespace Private {


QMap<QString, QString> EditorWidgetBuilderImpl::fileExtToTypeMap; // file ending -> file type in factories


EditorComponent *DefaultLineNumberAreaFactory::operator ()(Editor *widget)
{
    return new IonEditor::Private::LineNumberArea(widget);
}

QSyntaxHighlighter *DefaultHighlighterFactory::operator ()(Editor *widget)
{
    return new IonEditor::Private::Highlighter(widget->getEditorInstance());
}

QSyntaxHighlighter *EditorWidgetBuilderImpl::createHighlighter(Editor *widget, QString filetype)
{
    QMap<QString, QSharedPointer<HighlighterFactory> >::const_iterator it = typeToHighlighterFactoryMap.find(filetype);
    if (it != typeToHighlighterFactoryMap.end()) {
        return (*(*it))(widget);
    }
    DefaultHighlighterFactory _default;
    return _default(widget);
}
EditorComponent *EditorWidgetBuilderImpl::createLineNumberArea(Editor *widget, QString filetype)
{
    QMap<QString, QSharedPointer<LineNumberAreaFactory> >::const_iterator it = typeToLineNumberAreaFactoryMap.find(filetype);
    if (it != typeToLineNumberAreaFactoryMap.end()) {
        return (*(*it))(widget);
    }
    DefaultLineNumberAreaFactory _default;
    return _default(widget);
}

IonHeart::PanelWidget *EditorWidgetBuilderImpl::createEditor(QString path)
{
    EditorWidget *ret = new EditorWidget(path);
    QString type = getFileType(path);
    ret->setComponents(QList<EditorComponent*>() << createLineNumberArea(ret, type));
    ret->setHighlighter(createHighlighter(ret, type));
    return ret;
}

QString EditorWidgetBuilderImpl::getFileType(QString filePath)
{
    int pos = filePath.lastIndexOf('.');
    if (pos > 0) {
        QMap<QString, QString>::const_iterator it = fileExtToTypeMap.find(filePath.right(filePath.length()-pos-1));
        if (it != fileExtToTypeMap.end()) {
            return it.value();
        }
    }
    return "text";
}

void EditorWidgetBuilderImpl::registerFileType(QString fileExt, QString fileType) {
    fileExtToTypeMap.insert(fileExt, fileType);
}
void EditorWidgetBuilderImpl::registerHighlighterFactory(QString const & filetype, HighlighterFactory *highlighter) {
    typeToHighlighterFactoryMap[filetype] = QSharedPointer<HighlighterFactory>(highlighter);
}
void EditorWidgetBuilderImpl::registerLineNumberAreaFactory(QString const & filetype, LineNumberAreaFactory *lineNumberArea) {
    typeToLineNumberAreaFactoryMap[filetype] = QSharedPointer<LineNumberAreaFactory>(lineNumberArea);
}

}
}
