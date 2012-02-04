ionPulse IDE
============

Todo
----

### 0.1 CODE REVIEW, documentation

#### search & replace
* mark occurences in editor on clicking on varname

#### editor handler
* handles key presses, such as tab, (shift) enter etc
* autoindent on new line
* home button should go to beginning of the text, after the whitespace
* tabulator should not indent if not on the start of the line

#### new file

#### UTF8 editor

#### project tree
* limit max length of each item (+joined ones) - use dots in the middle with full info as a tooltip
* color coding

#### show trailing whitespaces

#### bug: caret is gone on refocus editor until a char is typed

### 0.2 CODE REVIEW, documentation

#### reparse files on save

#### search & replace in files
* should be possible to delete some results after reviewing

#### trim trailing whitespaces

#### parse php in background thread

#### layout manager
* vertical/horizontal split source code tabs e.g. for tests/code

### 0.3 CODE REVIEW, documentation

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

