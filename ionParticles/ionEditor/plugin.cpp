/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "plugin.h"

#include <QtPlugin>
#include "editorwidget.h"
#include "editorwidgetfactory.h"
#include <QMessageBox>
#include "editorwidgetfactory.h"
#include <ionParticles/ionLayout/layoutapi.h>
#include <QBoxLayout>
#include "filetreewidget.h"

#include <ionCore/shared.h>
#include <QMenuBar>

#include "searchpanel.h"

namespace IonEditor {

namespace Private {

QScriptValue JsSyntaxHighlighterToScriptValue(QScriptEngine *engine, JsSyntaxHighlighter* const &in)
{ return engine->newQObject(in); }

void JsSyntaxHighlighterFromScriptValue(const QScriptValue &object, JsSyntaxHighlighter* &out)
{ out = qobject_cast<JsSyntaxHighlighter*>(object.toQObject()); }


struct JsHighlighterFactory : virtual public IonEditor::HighlighterFactory {
    QScriptEngine *engine;
    QScriptValue function;
    JsHighlighterFactory(QScriptEngine *engine, QScriptValue function) : engine(engine), function(function) {}
    virtual QSyntaxHighlighter *operator()(IonEditor::Editor *editor, QString filetype)
    {
        return new JsSyntaxHighlighter(editor, engine, function);
    }
};



QScriptValue registerJsFileHighlighter(QScriptContext *context, QScriptEngine *engine)
{
    Plugin *plugin = qobject_cast<Plugin *>(engine->globalObject().property("editorPlugin").toQObject());
    plugin->getEditorWidgetBuilder()->registerHighlighterFactory(
        context->argument(0).toString(),
        new JsHighlighterFactory(engine, context->argument(1))
    );
    return engine->nullValue();
}

QScriptValue registerJsFileType(QScriptContext *context, QScriptEngine *engine)
{
    Plugin *plugin = qobject_cast<Plugin *>(engine->globalObject().property("editorPlugin").toQObject());
    plugin->getEditorWidgetBuilder()->registerFileType(
        context->argument(0).toString(),
        context->argument(1).toString()
    );
    return engine->nullValue();
}


QScriptValue editorToScriptValue(QScriptEngine *engine, Editor* const &in)
{
    return engine->newQObject(in);
}

void editorFromScriptValue(const QScriptValue &object, Editor* &out)
{
    out = qobject_cast<Editor*>(object.toQObject());
}

Plugin::Plugin(QObject *parent) :
    QObject(parent), _layoutManager(NULL), _openedFiles(), _focusedEditor(NULL)
{
}

Plugin::~Plugin()
{
    // needs to be here as per
    // http://doc.qt.nokia.com/latest/qscopedpointer.html#forward-declared-pointers
}

void Plugin::addParent(BasicPlugin *parent) {
    CHECK_AND_ADD_PARENT(parent, IonLayout::LayoutPlugin, _layoutManager = target->getLayoutManager());
}


void Plugin::postLoad()
{
    Q_ASSERT(_layoutManager);
    Q_ASSERT(mainWindow);

    IonLayout::ZoneDefinition def;

    def.name = "central";
    def.orientation = Qt::Vertical;
    def.parentPath = "";
    def.after = "left";
    def.before = "right";
    def.hideIfEmpty = false;
    def.sizeWeight = 700;
    def.childrenClosable = true;
    def.subZonesContainerType = IonLayout::ZoneDefinition::Boxed;
    def.widgetsContainerType = IonLayout::ZoneDefinition::Tabbed;
    _layoutManager->addZone(def);

    def.name = "central_footer";
    def.orientation = Qt::Vertical;
    def.parentPath = "central";
    def.after = "central";
    def.before = "";
    def.hideIfEmpty = true;
    def.sizeWeight = 1;
    def.childrenClosable = false;
    def.subZonesContainerType = IonLayout::ZoneDefinition::Tabbed;
    def.widgetsContainerType = IonLayout::ZoneDefinition::Boxed;
    _layoutManager->addZone(def);

    def.name = "left";
    def.orientation = Qt::Vertical;
    def.parentPath = "";
    def.after = "";
    def.before = "central";
    def.hideIfEmpty = false;
    def.sizeWeight = 200;
    def.childrenClosable = false;
    def.subZonesContainerType = IonLayout::ZoneDefinition::Split;
    def.widgetsContainerType = IonLayout::ZoneDefinition::Tabbed;
    _layoutManager->addZone(def);

    def.name = "leftbottom";
    def.orientation = Qt::Vertical;
    def.parentPath = "left";
    def.after = "left";
    def.before = "";
    def.hideIfEmpty = true;
    def.sizeWeight = 150;
    def.childrenClosable = false;
    def.subZonesContainerType = IonLayout::ZoneDefinition::Split;
    def.widgetsContainerType = IonLayout::ZoneDefinition::Tabbed;
    _layoutManager->addZone(def);

    def.name = "right";
    def.orientation = Qt::Vertical;
    def.parentPath = "";
    def.after = "central";
    def.before = "";
    def.hideIfEmpty = true;
    def.sizeWeight = 150;
    def.childrenClosable = false;
    def.subZonesContainerType = IonLayout::ZoneDefinition::Split;
    def.widgetsContainerType = IonLayout::ZoneDefinition::Tabbed;
    _layoutManager->addZone(def);

    Private::FileTreeWidget *fileTree = new Private::FileTreeWidget();
    _layoutManager->add("left", fileTree);
    connect(fileTree, SIGNAL(fileActivated(QString, int)), this, SLOT(openFile(QString, int)));

    getEditorWidgetBuilder()->registerComponentFactory("text", new Private::DefaultLineNumberAreaFactory());

    QMenuBar *menuBar = mainWindow->menuBar();
    QMenu *fileMenu = menuBar->addMenu("&File");
    fileMenu->addAction("&Save", this, SLOT(onFileSave()), QKeySequence(Qt::CTRL + Qt::Key_S));
    fileMenu->addAction("&Close", this, SLOT(onFileClose()), QKeySequence(Qt::CTRL + Qt::Key_W));

    QMenu *editMenu = menuBar->addMenu("&Edit");
    editMenu->addAction("&Find", this, SLOT(onEditSearch()), QKeySequence(Qt::CTRL + Qt::Key_F));
    editMenu->addAction("Find &Next", this, SLOT(onEditSearchNext()), QKeySequence(Qt::CTRL + Qt::Key_G));
    editMenu->addAction("Find &Previous", this, SLOT(onEditSearchPrev()), QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_G));

    _searchPanel = new SearchPanel();
    _searchPanel->setEditorPlugin(this);
    _layoutManager->add("central/central_footer", _searchPanel);

}

Editor *Plugin::getCurrentEditor() {
    return _focusedEditor;
}

QMap<QString, Editor *> Plugin::getOpenedFiles() {
    return _openedFiles;
}

void Plugin::onEditSearch()
{
    _searchPanel->focusSearchInput();
}

void Plugin::onEditSearchNext()
{
    _searchPanel->findNext();
}
void Plugin::onEditSearchPrev()
{
    _searchPanel->findPrevious();
}

void Plugin::onFileSave()
{
    if (_focusedEditor) {
        _focusedEditor->saveFile();
    }
}
void Plugin::onFileClose()
{
    if (_focusedEditor) {
        Editor *widget = _focusedEditor;
        int ret = QMessageBox::Discard;
        if (widget->document()->isModified()) {
            QMessageBox msgBox;
            msgBox.setText("The document '"+widget->windowTitle()+"' has been modified.");
            msgBox.setInformativeText("Do you want to save your changes?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            ret = msgBox.exec();
        }
        switch (ret) {
            case QMessageBox::Cancel:
                break;
            case QMessageBox::Save:
                widget->saveFile();
                // fall through to remove it
            case QMessageBox::Discard:
                _focusedEditor = NULL;
                _layoutManager->remove(widget);
        }
    }
}

void Plugin::openFile(QString path, int line)
{
    Editor *widget = NULL;
    QMap<QString, Editor *>::Iterator it = _openedFiles.find(path);
    if (_openedFiles.end() == it) {
        widget = getEditorWidgetBuilder()->createEditor(path);
        _openedFiles[path] = widget;
        connect(widget, SIGNAL(editorClosing(Editor *)), this, SLOT(closeFileEditor(Editor *)));
        connect(widget, SIGNAL(editorFocusing(Editor *)), this, SLOT(focusFileEditor(Editor *)));
        _layoutManager->add("central", widget);
        emit editorOpened(widget);
    } else {
        widget = it.value();
        _layoutManager->focus(widget);
    }
    if (-1 != line) {
        widget->focusOnLine(line);
    } else {
        widget->focusOnLine(0);
    }
}

void Plugin::focusFileEditor(Editor *editor)
{
    _focusedEditor = editor;
}

void Plugin::closeFileEditor(Editor *editor)
{
    for (QMap<QString, Editor *>::Iterator it = _openedFiles.begin(); it != _openedFiles.end(); it++) {
        if (it.value() == editor) {
            it = _openedFiles.erase(it);
            return;
        }
    }
}


EditorWidgetBuilder *Plugin::getEditorWidgetBuilder()
{
    if (!_editorWidgetFactory) {
        _editorWidgetFactory.reset(new Private::EditorWidgetBuilderImpl());
    }
    return _editorWidgetFactory.data();
}

void Plugin::registerJsApi(QScriptEngine & jsEngine)
{
    qScriptRegisterMetaType(&jsEngine, editorToScriptValue, editorFromScriptValue);
    QScriptValue editorPlugin = jsEngine.newQObject(this);
    jsEngine.globalObject().setProperty("editorPlugin", editorPlugin);
    jsEngine.globalObject().setProperty("registerFileHighlighter", jsEngine.newFunction(IonEditor::Private::registerJsFileHighlighter));
    jsEngine.globalObject().setProperty("registerFileType", jsEngine.newFunction(IonEditor::Private::registerJsFileType));
    qScriptRegisterMetaType(&jsEngine, JsSyntaxHighlighterToScriptValue, JsSyntaxHighlighterFromScriptValue);
}


}
}

Q_EXPORT_PLUGIN2 ( ionEditor, IonEditor::Private::Plugin )
