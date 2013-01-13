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
* Custom user init.js to add the missing bits.


Start Hacking!
--------------

Download from [SourceForge](http://sourceforge.net/projects/ionpulse/files/?) (due to limited resources, only Mac OS version is available ATM).

Or build it for your platform:
preparing your environment can be a somewhat involved task, so this option is currently suggested only to experienced users.
[Project documentation](http://svalaskevicius.github.com/ionPulse/api/index.html) defines the requirements and instructions for building the project.

Once you get it working, check the files in the main app directory:

*  ionPulse.css - this is where the css is defined (pulling from the user home directory is planned);
*  js/ionPulse.js - this is the main entry for loading the javascript files;
*  ~/.ionPulse/init.js - the user file for customisations.

Create! :)

Example
-------

Lets create a custom syntax highlighter. We'll start this by defining a new file type. The following command will register
a file type "text/slides" for all the files which end as ".slides":

    registerFileType("slides", "text/slides");


The syntax highligter implemented in ionPulse is a state machine, so our next step is to define some highlighting states
and transitions from one state to another:

    textHighlighter.addTransitions({
        "slides/body": {
            "slides/header": textHighlighter.regexTransition(/^\./g, true),
            "slides/space": textHighlighter.regexTransition(/^___$/g, true),
        },
        "slides/header": {
            "slides/body": textHighlighter.regexTransition(/$/g, false),
        },
        "slides/space": {
            "slides/body": textHighlighter.regexTransition(/$/g, false),
        }
    });


Each of the states should already have a default formatting, but in addition
to that, we would like to add some semi-states, where the highlighting execution
stays in the same state, however the text is formatted differently.

    textHighlighter.addHighlightRules({
        "slides/body" : {
            "black": /\*[^*]+\*/g,     // some *important* text
            "emphasis": /\/[^\/]+\//g, // some /emphasised/ text
        }
    });

The important bit in syntax highlighting is the text formatting. Lets define it
for our states and highlighting rules:

    textHighlighter.addTextFormatting({
        //                       color,             weight,       italic, size, backgroundColor
        "slides/body":          [toColor('706050'), QFont.Normal, false,  64,   null],
        "slides/header":        [toColor('805040'), QFont.Normal, false,  96,   null],
        "slides/body/black":    [toColor('000000'), QFont.Black,  false,  64,   null],
        "slides/body/emphasis": [toColor('706050'), QFont.Normal, true,   64,   null],
        "slides/space":         [toColor('e0a0a0'), QFont.Normal, false,  500,  null]
    });


The last bit is to set the starting state when highlighting "text/slides" files:

    textHighlighter.setDefaultState('text/slides', 'slides/body');


While the syntax highlighter above works ok, we might also want to change the background colour
of the file editor. Changing it using backgroundColor parameter in the text formatting description
might not be what we want - as it would only change the background colour of the edited text, 
leaving the editor window colour as it is set by the default stylesheet. The snippet below will
change the style of any editor, opened to edit the "text/slides" files:

    editorPlugin.editorOpened.connect(
        this,
        function (editor) {
            if (editor.property("fileType") === 'text/slides') {
                editor.setStyleSheet("background-color: #fab976");
            }
        }
    );

That's it! Put all the given code into your ~/.ionPulse/init.js file and you'll have a custom syntax highlighter :)


Building Dependencies
---------------------
The following commands are helpful for MacOS users and are clang specific (with c++11).

- BDB XML

```
cd ~
wget https://gist.github.com/raw/4312897/bdbxml-clang.patch

cd <your dbxml-2.5.16 dir>

patch -p2 < ~/bdbxml-clang.patch

./buildall.sh -m 'make -j6' \
--with-configure-env="CC=clang CXX=clang++ CXXFLAGS='-std=c++11 -stdlib=libc++ -lc++ -mmacosx-version-min=10.7 -Wno-c++11-narrowing' LDFLAGS='-stdlib=libc++ -lc++ -mmacosx-version-min=10.7'" \
--with-xerces-conf="--disable-network --without-curl" \
--with-xqilla-conf="--without-tidy"
```

- googlemock

```
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_FLAGS="-std=c++11 -stdlib=libc++ -mmacosx-version-min=10.7 -DGTEST_USE_OWN_TR1_TUPLE=1" ..
make
cp libgmock.a /usr/local/lib/
cp gtest/libgtest.a /usr/local/lib/
rm -Rf /usr/local/include/gmock/
cp -R ../include/gmock /usr/local/include/gmock
```


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
* indentation

#### syntax formatter

#### tools menu with commands to invoke for dir/file
* checkstyle
* phpunit
* phpspec

#### html, css, js highlight

#### ? tdd/bdd helper -> switch to/from test by configurable rules

#### ? textMate bundle support


