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
    void editorOpened(QPlainTextEdit *editor);
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
private:
    QScriptEngine *engine;
    QScriptValue function;

public:
    JsSyntaxHighlighter(IonEditor::Editor *editor, QScriptEngine *engine, QScriptValue function) : QSyntaxHighlighter(editor->document()), engine(engine), function(function) {}
//    QTextBlock currentBlock() const {return QSyntaxHighlighter::currentBlock();}
//    int currentBlockState() const {return QSyntaxHighlighter::currentBlockState();}
//    QTextBlockUserData * currentBlockUserData() const {return QSyntaxHighlighter::currentBlockUserData();}
//    QTextCharFormat format(int position) const {return QSyntaxHighlighter::currentBlock();}
    virtual void highlightBlock(const QString & text) {function.call(function, QScriptValueList()<<engine->toScriptValue(this)<<engine->toScriptValue(text));}
//    int previousBlockState() const {return QSyntaxHighlighter::currentBlock();}
//    void setCurrentBlockState(int newState) {return QSyntaxHighlighter::currentBlock();}
//    void setCurrentBlockUserData(QTextBlockUserData * data) {return QSyntaxHighlighter::currentBlock();}
    Q_INVOKABLE void dbg(){DEBUG_MSG("ASasdasdasd");}

//    int previousBlockState() const;
//    int currentBlockState() const;
//    void setCurrentBlockState(int newState);

//    void setCurrentBlockUserData(QTextBlockUserData *data);
//    QTextBlockUserData *currentBlockUserData() const;

//    QTextBlock currentBlock() const;

    Q_INVOKABLE void setFormat(int start, int count, QTextCharFormat format) {
       // DEBUG_MSG("set fmt for " << start << count << format.foreground());
        QSyntaxHighlighter::setFormat(start, count, format);
    }
//    void setFormat(int start, int count, const QColor & color) {return QSyntaxHighlighter::currentBlock();}
//    void setFormat(int start, int count, const QFont & font) {return QSyntaxHighlighter::currentBlock();}
};


}
}

Q_DECLARE_METATYPE(IonEditor::Editor*)

Q_DECLARE_METATYPE(IonEditor::Private::JsSyntaxHighlighter*);

#endif // IONEDITOR_H




