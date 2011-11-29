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

QSyntaxHighlighter *DefaultHighlighterFactory::operator ()(Editor *widget)
{
    return new IonEditor::Private::Highlighter(widget->getEditorInstance());
}

QSyntaxHighlighter *EditorWidgetBuilderImpl::createHighlighter(Editor *widget, QString filetype)
{
    QMap<QString, QSharedPointer<HighlighterFactory> >::const_iterator it = typeToHighlighterFactoryMap.find(filetype);
    if (it != typeToHighlighterFactoryMap.end()) {
        qDebug() << typeToHighlighterFactoryMap;
        return (*(*it))(widget);
    }
    DefaultHighlighterFactory _default;
    return _default(widget);
}

QList<EditorComponent*> EditorWidgetBuilderImpl::createComponents(Editor *widget, QString fileType)
{
    QList<EditorComponent*> components;
    QSet<QString> addedComponents;
    while (fileType != "") {
        foreach (QSharedPointer<EditorComponentFactoryBase> factory, typeToComponentFactoryMap.values(fileType)) {
            if (!addedComponents.contains(factory->getIdentifier())) {
                qDebug() << "Loading component: "<< fileType << factory->getIdentifier();
                components.append((*factory)(widget));
                addedComponents.insert(factory->getIdentifier());
            }
        }
        int idx = fileType.lastIndexOf("/");
        if (idx > 0) {
            fileType = fileType.left(idx);
        } else {
            fileType = "";
        }
    }
    return components;
}

Editor *EditorWidgetBuilderImpl::createEditor(QString path)
{
    EditorWidget *ret = new EditorWidget(path);
    QString type = getFileType(path);
    ret->setComponents(createComponents(ret, type));
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
void EditorWidgetBuilderImpl::registerComponentFactory(QString const & filetype, EditorComponentFactoryBase *component) {
    typeToComponentFactoryMap.insert(filetype, QSharedPointer<EditorComponentFactoryBase>(component));
}

}
}
