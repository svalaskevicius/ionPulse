/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef IONEDITOR_H
#define IONEDITOR_H

#include <QObject>
#include <QScopedPointer>

#include <ionParticles/ionLayout/layoutapi.h>

#include "editorapi.h"
#include "searchpanel.h"

namespace IonEditor {

namespace Private {
class EditorWidgetBuilderImpl;

class Plugin : public QObject, public EditorPlugin
{
    Q_OBJECT
    Q_INTERFACES(IonCore::BasicPlugin)
    Q_PROPERTY( IonEditor::Editor * focusedEditor READ getCurrentEditor )
public:
    explicit Plugin(QObject *parent = 0);
    ~Plugin();
    void postLoad();
    QString getName() {
        return EditorPlugin::name();
    }
    virtual QList<QString> getDependencies() {
        QList<QString> ret;
        ret.append(IonLayout::LayoutPlugin::name());
        return ret;
    }
    virtual void addParent(BasicPlugin *parent);
    EditorWidgetBuilder *getEditorWidgetBuilder();
private:
    QScopedPointer<Private::EditorWidgetBuilderImpl> _editorWidgetFactory;
    IonLayout::LayoutManager *_layoutManager;
    QMap<QString, Editor *> _openedFiles;
    Editor *_focusedEditor;
    SearchPanel *_searchPanel;
public:
    virtual void registerJsApi(QScriptEngine & jsEngine);
    Editor *getCurrentEditor();
    QMap<QString, Editor *> getOpenedFiles();
signals:

public slots:
    void openFile(QString path, int line);
    void closeFileEditor(Editor *editor);
    void focusFileEditor(Editor *editor);
    void onFileSave();
    void onFileClose();
    void onEditSearch();
    void onEditSearchNext();
    void onEditSearchPrev();
private:
    Q_DISABLE_COPY(Plugin)
};

}
}

Q_DECLARE_METATYPE(IonEditor::Editor*)

#endif // IONEDITOR_H




