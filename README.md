ionPulse IDE
============

The goal of the project is to create a fast, fully functional IDE, extendable and customisable by developers using it.

To achieve this goal, the Qt framework is chosen as it provides a native implementation across multiple platforms. The Look & Feel of Qt projects is both suitable for the platform it is executed on and is also customisable using the widely known CSS standard. The flexibility is achieved by providing Javascript bindings to both the IDE plugins (C++) and the whole Qt framework.


Current state
-------------

ionPulse IDE currently supports:
* parsing PHP code into AST database (Berkeley DB XML), which is accessible both from C++ and Javascript code;
* providing a /filterable/ project tree, consisting of directories, files, php classes and methods, allowing quick and easy code navigation;
* file editor functionality, which is mostly extended using Javascript. E.g. syntax highlighting for PHP is fully implemented in Javascript;
* Javascript console (which itself is implemented using Javascript), allowing to execute javascript operations on the fly, allowing to define and use macros, keybindings, with functional capabilities of the underlying framework.


Start Hacking!
--------------

Download:
https://github.com/svalaskevicius/ionPulse/downloads (due to limited resources, only Mac OS version is available ATM).

Build it for your platform:
preparing your environment can be a somewhat involved task, so this option is currently suggested only to experienced users.
http://svalaskevicius.github.com/ionPulse/api/index.html defines the requirements and instructions for building the project.

Once you get it working, check the files in the main app directory:
* ionPulse.css - this is where the css is defined (pulling from the user home directory is planned);
* js/ionPulse.js - this is the main entry for loading the javascript files.

Create! :)


Todo
----

#### search & replace
* mark occurences in editor on clicking on varname

#### editor handler
* handles key presses, such as tab, (shift) enter etc
* autoindent on new line
* home button should go to beginning of the text, after the whitespace, unless it is in that position already - subsequent home key should focus the start of line
* tabulator should not indent if not on the start of the line

#### new file

#### UTF8 editor

#### project tree
* limit max length of each item (+joined ones) - use dots in the middle with full info as a tooltip

#### bug: caret is gone on refocus editor until a char is typed

#### reparse files on save

#### search & replace in files
* should be possible to delete some results after reviewing

#### parse php in background thread

#### layout manager
* vertical/horizontal split source code tabs e.g. for tests/code
* support center, north, west, south, east directions to add sub-zones/panels

#### autocomplete
* editor reparse on the fly
* editor position -> php scope
* typehinting: parse phpDoc

#### improve Treemodel to provide icons and fonts for dir view

#### project: move TreeModel to overridable factory
* make php project override it and add decorators for tree model getData to provide icons, font sizes etc

#### config
* colors
* fonts
* indentation

#### preference dialog to manage config

#### syntax formatter

#### tools menu with commands to invoke for dir/file
* checkstyle
* phpunit
* phpspec

#### html, css, js highlight

#### ? tdd/bdd helper -> switch to/from test by configurable rules

#### ? textMate bundle support


