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
#include <QTextBlockUserData>

#include <ionParticles/ionLayout/layoutapi.h>

#include "editorapi.h"
#include "searchpanel.h"

namespace IonEditor {

namespace Private {
class EditorWidgetBuilderImpl;

class Plugin : public QObject, public EditorPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.ionPulse.ionEditor")
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
    virtual QObject *asQObject();

private:
    QScopedPointer<Private::EditorWidgetBuilderImpl> _editorWidgetFactory;
    IonLayout::LayoutManager *_layoutManager;
    QMap<QString, Editor *> _openedFiles;
    Editor *_focusedEditor;
    SearchPanel *_searchPanel;
public:
    virtual void registerJsApi(QJSEngine & jsEngine);
    Editor *getCurrentEditor();
    QMap<QString, Editor *> getOpenedFiles();

signals:
    void editorOpened(IonEditor::Editor *editor);
    void editorFocused(IonEditor::Editor *editor);
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


class JsSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

    Q_PROPERTY(int previousBlockState READ previousBlockState)
    Q_PROPERTY(int currentBlockState READ currentBlockState WRITE setCurrentBlockState)
    Q_PROPERTY(QTextBlock currentBlock READ currentBlock)
    Q_PROPERTY(IonEditor::Editor *editor READ getEditor)
private:
    QJSEngine *engine;
    QJSValue function;
    IonEditor::Editor *editor;

public:
    JsSyntaxHighlighter(IonEditor::Editor *editor, QJSEngine *engine, QJSValue function)
        : QSyntaxHighlighter(editor->document()), engine(engine), function(function), editor(editor)
    {
    }

    virtual void highlightBlock(const QString & text)
    {
        function.call(
            QJSValueList() << engine->toScriptValue(this) << engine->toScriptValue(text)
        );
    }

    Q_INVOKABLE void setFormat(int start, int count, QTextCharFormat format)
    {
        QSyntaxHighlighter::setFormat(start, count, format);
    }

    Q_INVOKABLE void setCurrentBlockUserData ( QTextBlockUserData * data )
    {
        QSyntaxHighlighter::setCurrentBlockUserData(data);
    }

    IonEditor::Editor *getEditor()
    {
        return editor;
    }
};


class JsTextBlockUserData : public QTextBlockUserData
{
public:
    QJSValue value;
};

}
}

Q_DECLARE_METATYPE(IonEditor::Editor*)
Q_DECLARE_METATYPE(IonEditor::Private::JsSyntaxHighlighter*)
Q_DECLARE_METATYPE(QTextBlockUserData*)

#endif // IONEDITOR_H




