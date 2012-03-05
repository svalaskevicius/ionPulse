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

Plugin::Plugin(QObject *parent) :
    QObject(parent), layoutManager(NULL), openedFiles(), focusedEditor(NULL)
{
}

Plugin::~Plugin()
{
    // needs to be here as per
    // http://doc.qt.nokia.com/latest/qscopedpointer.html#forward-declared-pointers
}

void Plugin::addParent(BasicPlugin *parent) {
    CHECK_AND_ADD_PARENT(parent, IonLayout::LayoutPlugin, layoutManager = target->getLayoutManager());
}


void Plugin::postLoad()
{
    Q_ASSERT(layoutManager);
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
    layoutManager->addZone(def);

    def.name = "central_footer";
    def.orientation = Qt::Horizontal;
    def.parentPath = "central";
    def.after = "central";
    def.before = "";
    def.hideIfEmpty = true;
    def.sizeWeight = 1;
    def.childrenClosable = false;
    def.subZonesContainerType = IonLayout::ZoneDefinition::Tabbed;
    def.widgetsContainerType = IonLayout::ZoneDefinition::Boxed;
    layoutManager->addZone(def);

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
    layoutManager->addZone(def);

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
    layoutManager->addZone(def);

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
    layoutManager->addZone(def);

    Private::FileTreeWidget *fileTree = new Private::FileTreeWidget();
    layoutManager->add("left", fileTree);
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

    searchPanel = new SearchPanel();
    searchPanel->setEditorPlugin(this);
    layoutManager->add("central/central_footer", searchPanel);

}

Editor *Plugin::getCurrentEditor() {
    return focusedEditor;
}

void Plugin::onEditSearch()
{
    searchPanel->focusSearchInput();
}

void Plugin::onEditSearchNext()
{
    searchPanel->findNext();
}
void Plugin::onEditSearchPrev()
{
    searchPanel->findPrevious();
}

void Plugin::onFileSave()
{
    if (focusedEditor) {
        focusedEditor->saveFile();
    }
}
void Plugin::onFileClose()
{
    if (focusedEditor) {
        Editor *widget = focusedEditor;
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
                focusedEditor = NULL;
                layoutManager->remove(widget);
        }
    }
}

void Plugin::openFile(QString path, int line)
{
    Editor *widget = NULL;
    QMap<QString, Editor *>::Iterator it = openedFiles.find(path);
    if (openedFiles.end() == it) {
        widget = getEditorWidgetBuilder()->createEditor(path);
        openedFiles[path] = widget;
        connect(widget, SIGNAL(editorClosing(Editor *)), this, SLOT(closeFileEditor(Editor *)));
        connect(widget, SIGNAL(editorFocusing(Editor *)), this, SLOT(focusFileEditor(Editor *)));
        layoutManager->add("central", widget);
    } else {
        widget = it.value();
        layoutManager->focus(widget);
    }
    if (-1 != line) {
        widget->focusOnLine(line);
    } else {
        widget->focusOnLine(0);
    }
}

void Plugin::focusFileEditor(Editor *editor)
{
    focusedEditor = editor;
}

void Plugin::closeFileEditor(Editor *editor)
{
    for (QMap<QString, Editor *>::Iterator it = openedFiles.begin(); it != openedFiles.end(); it++) {
        if (it.value() == editor) {
            it = openedFiles.erase(it);
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

}
}

Q_EXPORT_PLUGIN2 ( ionEditor, IonEditor::Private::Plugin )
