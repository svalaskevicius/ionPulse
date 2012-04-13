/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef EDITORAPI_H
#define EDITORAPI_H

#include <QTextBlock>
#include <QEvent>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <ionParticles/ionLayout/layoutapi.h>
#include <ionCore/plugin.h>

/**
 * \brief Provides basic (but extendable) editor functionality.
 *
 * The IonEditor plugin defines an extendable plain text editor which
 * is adaptable for different filetypes.
 *
 * To instantiate an editor, EditorWidgetBuilder is invoked with the
 * filepath to be loaded as a parameter. EditorWidgetBuilder then checks
 * for any registered EditorComponentFactoryBase factories for the given
 * file type and uses them to customise the editor.
 *
 * EditorComponent can be registered using an extended file type notation
 * describing the file type family to be used.
 * e.g. "text/php", which tells that the component is suitable for text/php
 * files family and will be applied if a file type would resolve in either
 * "text/php" or "text/php/any/subtype".
 *
 * A file extension registration to a file type is achieved by using
 * EditorWidgetBuilder::registerFileType().
 * If a file extension is not yet registered, a default "text" file type family
 * is assumed.
 */
namespace IonEditor {


/**
 * \brief Provides basic information about the editor widget to components, which is not visible otherwise.
 *
 * This interface opens several protected Editor widget methods to external components,
 * so that some functionality could be deferred and component-ised from the editor widget class.
 *
 * \see Editor::getEditorInfo()
 */
class EditorComponentInfo {
public:
    virtual ~EditorComponentInfo() {}

    /**
     * \brief Returns the first visible block
     */
    virtual QTextBlock firstVisibleBlock() const = 0;

    /**
     * \brief Returns the bounding rectangle of the text block in content coordinates.
     *
     * Translate the rectangle with the contentOffset() to get visual coordinates on the viewport.
     */
    virtual QRectF blockBoundingGeometry(const QTextBlock &block) const = 0;

    /**
     * \brief Returns the content's origin in viewport coordinates.
     *
     * The origin of the content of a plain text edit is always the top left corner
     * of the first visible text block. The content offset is different from (0,0)
     * when the text has been scrolled horizontally, or when the first visible block
     * has been scrolled partially off the screen, i.e. the visible text does not start
     * with the first line of the first visible block, or when the first visible block
     * is the very first block and the editor displays a margin.
     */
    virtual QPointF contentOffset() const = 0;

    /**
     * \brief Returns the bounding rectangle of the text block in the block's own coordinates.
     */
    virtual QRectF blockBoundingRect(const QTextBlock &block) const = 0;
};


/**
 * \brief Define an interface to create Editor components
 *
 * Editor components provide a way to defer some specific editor functionality
 * to be applied dynamically depending on the file type.
 */
class EditorComponent {
public:
    virtual ~EditorComponent() {}

    /**
     * \brief Handles editor events.
     *
     * The component's editorEvent method is invoked before falling back to default
     * event handler. If any of invoked components return true as a result of this
     * method, default event functionality will not be invoked.
     */
    virtual bool editorEvent(QEvent * ) = 0;

    /**
     * \brief Return width of a component.
     *
     * Currently, component implementation can only take some visual rectangle at the left
     * hand side of the Editor widget. This method will be used to determine the next
     * component, or Editor offset.
     */
    virtual int getWidth() = 0;
};


/**
 * \brief Defines an interface to access and manipulate the Editor widget.
 */
class Editor : public QPlainTextEdit {
    Q_OBJECT
    Q_PROPERTY(int textOptionFlags READ textOptionFlags WRITE setTextOptionFlags)
public:
    Editor() : QPlainTextEdit(NULL) {}
    virtual ~Editor() {}

    /**
     * \brief Defer and open some usefull protected Editor methods.
     */
    virtual const EditorComponentInfo &getEditorInfo() const = 0;

    virtual int textOptionFlags() = 0;
    virtual void setTextOptionFlags(int) = 0;

public slots:
    /**
     * \brief Register an event listener.
     *
     * The event listener registered will only be invoked for events specified by type parameter.
     */
    virtual void addEventListener(QEvent::Type type, EditorComponent *component) = 0;

    /**
     * \brief Notify parent scroll area about changes in viewport margins.
     *
     * This method should be called from components, after component changes its width.
     */
    virtual void updateViewportMargins() = 0;

    /**
     * \brief Set active editor components.
     */
    virtual void setComponents(QList<EditorComponent* > components) = 0;

    /**
     * \brief Set editor focus on the requested line.
     */
    virtual void focusOnLine(int line) = 0;

    /**
     * \brief Save file buffer to file.
     */
    virtual void saveFile() = 0;


};

/**
 * \brief A base interface for EditorComponent factories.
 */
struct EditorComponentFactoryBase {

    virtual ~EditorComponentFactoryBase() {}
    /**
     * \brief Construct EditorComponent for a given Editor object.
     */
    virtual IonEditor::EditorComponent *operator()(Editor *) = 0;

    /**
     * \brief Identify this component factory.
     *
     * This identifier is used to limit created components for one Editor to be one
     * for each identifier. If the same identifier is available for several file types
     * in the same hierarchy, only the most specific EditorComponent factory will be used.
     */
    virtual QString getIdentifier() = 0;
};


/**
 * \brief Convenience  templace to create EditorComponent factories.
 *
 * Requires that the component would define a static method identity().
 */
template <typename component>
struct EditorComponentFactory : public EditorComponentFactoryBase {
    IonEditor::EditorComponent *operator()(Editor *e) {return new component(e);}
    QString getIdentifier() {return component::identity();}
};

/**
 * \brief Convenience  templace to create EditorComponent factories.
 *
 * Requires that the component would define a static method identity().
 */
template <typename component, typename targ1>
struct EditorComponentFactory1 : public EditorComponentFactoryBase {
    targ1 arg1;
    EditorComponentFactory1(targ1 arg1) : arg1(arg1) {}
    IonEditor::EditorComponent *operator()(Editor *e) {return new component(e, arg1);}
    QString getIdentifier() {return component::identity();}
};

/**
 * \brief Editor highlighter factory.
 *
 * Syntax highlighters are not created using components functionality. Instead,
 * QSyntaxHighlighter subclasses are used.
 */
struct HighlighterFactory {
    virtual ~HighlighterFactory() {}
    /**
     * \brief Invoke the factory to create a syntax highlighter for the given Editor and filetype.
     */
    virtual QSyntaxHighlighter *operator()(Editor *, QString) = 0;
};

/**
 * \brief Editor builder class to create an Editor for a given file.
 *
 * This builder class uses registered file types and components for them to
 * dynamically construct a required Editor instance.
 */
class EditorWidgetBuilder
{
public:
    virtual ~EditorWidgetBuilder(){}

    /**
     * \brief Build Editor instance for a file specified by the path parameter.
     */
    virtual Editor *createEditor(QString path) = 0;

    /**
     * \brief Register a filetype for given file extension.
     */
    virtual void registerFileType(QString fileExt, QString fileType) = 0;

    /**
     * \brief Register a syntax highlighter for given file type.
     */
    virtual void registerHighlighterFactory(QString const & filetype, HighlighterFactory *highlighter) = 0;

    /**
     * \brief Register a EditorComponentFactoryBase for given file type.
     */
    virtual void registerComponentFactory(QString const & filetype, EditorComponentFactoryBase *component) = 0;
};

/**
 * \brief The interface of the IonEditor plugin.
 *
 * The EditorPlugin interface provides an unified application scope
 * access to the EditorWidgetBuilder.
 *
 * To use it, client plugins should list EditorPlugin::EditorPlugin::name()
 * in their dependencies. This way the client plugins will receive an
 * instance of EditorPlugin in their IonCore::BasicPlugin::addParent() method.
 */
class EditorPlugin : public IonCore::BasicPlugin
{
public:
    /**
     * \brief Plugin name to be used in IonCore::BasicPlugin::getDependencies()
     *        method of the subscribed plugins.
     */
    static QString name() {return "ionEditor";}

    /**
     * \brief Retrieve an instance of EditorWidgetBuilder.
     */
    virtual EditorWidgetBuilder *getEditorWidgetBuilder() = 0;

    /**
     * \brief Open a requested file path and focus on the specified line.
     */
    virtual void openFile(QString path, int line) = 0;

    /**
     * \brief Retrieve currently active Editor instance, or NULL, if none is active.
     */
    virtual Editor *getCurrentEditor() = 0;
};


}

#endif // EDITORAPI_H
