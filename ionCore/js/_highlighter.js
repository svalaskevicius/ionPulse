/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

TextHighlighter = function () {};
TextHighlighter.prototype = {
    /**
     * Add state transitions
     * The highlighter is a state machine where transitions define possible links from one state
     * to the others.
     *
     * Example:
     * {
     *     'path/to/state': {
     *         'path/to/new/state': transition object,
     *          ...
     *     },
     * }
     */
    addTransitions : function(transitions) {
        _.extend(this._transitions, transitions);
    },

    /**
     * Add highlighting rules in given states.
     * A highlighting rule is a semi-state, as it is only used to find a matching
     * character formatting in the current state.
     *
     * Example:
     * {
     *     'path/to/state': {
     *         'formatting rule': /regular expression/,
     *          ...
     *     },
     * }
     */
    addHighlightRules : function(rules) {
        _.extend(this._highlightRules, rules);
    },

    /**
     * Add text formatting rules for states and highlight rules in those states.
     * Each formatting element is an array of: color, weight, italic, size, backgroundColor
     *
     * Example:
     * {
     *     'path/to/state':                  [toColor("ccccb0"), QFont.Normal, false, null, null],
     *     'path/to/state/highlighter rule': [toColor("ccccb0"), QFont.Normal, true, null, null],
     * }
     */
    addTextFormatting : function(textFormatting) {
        var self = this;
        var createFormatter = function(el) {
            return self._createCharFormat(el[0], el[1], el[2], el[3], el[4]);
        };
        for (key in textFormatting) {
            this._charFormatting[key] = createFormatter(textFormatting[key]);
        }
    },

    /**
     * Invoke a callback for specified state for custom userdata generation
     */
    setBlockInfoHandler: function(state, callback) {
        this._blockInfoCallbacks[state] = callback;
    },

    /**
     * Specify which highlighter state will start the file for a given fileType
     */
    setDefaultState: function(fileType, state) {
        this._defaultStates[fileType] = state;
    },

    /**
     * Initialise internal highlighter structures
     * No more state transitions can be added after the initialise
     */
    initialize: function () {
        this.states = [];
        for (var state in this._transitions) {
            this.states.push(state);
        }
    },

    /**
     * Highlighter entry point, where C++ API invokes it to highlight a given text
     */
    highlight: function (cppApi, text) {
        this._blockInfo = {};
        this._cppApi = cppApi;
        this._text = text;
        this._stateMatcher = {
            'start': 0,
            'next': 0
        };
        var state = this._getStateById(cppApi.previousBlockState);

        while (this._stateMatcher) {
            state = this._processState(state);
        }
        var stateId = this.states.indexOf(state);
        if (cppApi.currentBlockState !== stateId) {
            cppApi.currentBlockState = stateId;
        }

        cppApi.setCurrentBlockUserData(this._blockInfo);
    },


    /**
     * Create a state transition based on a regular expression matched agains the text
     */
    regexTransition: function (re, atStart) {
        var self = this;
        if (/([^\[]|^)\^/.test(re.source)) {
            return function () {
                var textToMatch;
                if (0 === self._stateMatcher.next) {
                    textToMatch = self._text;
                } else {
                    textToMatch = self._text.substring(self._stateMatcher.next);
                }
                re.lastIndex = 0;
                var m = re.exec(textToMatch);
                if (m) {
                    return {
                        'start': self._stateMatcher.next + m.index + (atStart ? 0 : m[0].length),
                        'next': self._stateMatcher.next + m.index + m[0].length,
                    };
                }
                return false;
            };
        } else {
            return function () {
                re.lastIndex = self._stateMatcher.next;
                var m = re.exec(self._text);
                if (m) {
                    return {
                        'start': m.index + (atStart ? 0 : m[0].length),
                        'next': m.index + m[0].length,
                    };
                }
                return false;
            };
        }
    },

    _charFormatting: {},
    _transitions: {},
    _highlightRules: {},
    _blockInfoCallbacks: {},
    _defaultStates: {},

    _getStateById : function(prevStateId) {
        if (prevStateId < 0) {
            var fileType = this._cppApi.editor.property('fileType');
            if (this._defaultStates[fileType]) {
                return this._defaultStates[fileType];
            }
            return this.states[0];
        } else {
            return this.states[prevStateId];
        }
    },


    /**
     * Create character format to be used for displaying the highlighted text
     *
     * @return QTextCharFormat
     */
    _createCharFormat: function (color, weight, italic, size, backgroundColor) {
        if ((typeof size === 'undefined') || (size === null)) {
            size = 17;
        }
        var format = new QTextCharFormat();

        var brush = new QBrush();
        brush.setColor(color);
        brush.setStyle(Qt.SolidPattern);
        format.setForeground(brush);

        if ((typeof backgroundColor !== 'undefined') && (backgroundColor !== null)) {
            var bgBrush = new QBrush();
            bgBrush.setColor(backgroundColor);
            bgBrush.setStyle(Qt.SolidPattern);
            format.setBackground(bgBrush);
        }

        var font = new QFont("Inconsolata", size, weight, italic);
        if (!font.exactMatch()) {
            font = new QFont("Courier New", size, weight, italic);
        }
        format.setFont(font);

        return format;
    },

    _hightlightState: function (state, from, to) {
        if (this._blockInfoCallbacks[state]) {
            this._blockInfo[state] = this._blockInfoCallbacks[state](this._blockInfo[state], from, to, this._text);
        }
        this._cppApi.setFormat(from, to - from, this._charFormatting[state]);
        for (var rule in this._highlightRules[state]) {
            var re = this._highlightRules[state][rule];
            re.lastIndex = from;
            var idx = -1;
            do {
                var match = re.exec(this._text);
                if (match && match.index < to) {
                    idx = match.index;
                    this._cppApi.setFormat(idx, match[0].length, this._charFormatting[state + "/" + rule]);
                } else {
                    idx = -1;
                }
            } while (idx >= 0);
        }
    },

    _processState: function (state) {
        var minPos = false;
        var minState = false;
        for (var newState in this._transitions[state]) {
            var statePos = this._transitions[state][newState]();
            if (statePos) {
                if (!minPos || (minPos.start > statePos.start)) {
                    minPos = statePos;
                    minState = newState;
                }
            }
        }
        if (minPos) {
            this._hightlightState(state, this._stateMatcher.start, minPos.start);
            this._stateMatcher = minPos;
            return minState;
        } else {
            this._hightlightState(state, this._stateMatcher.start, this._text.length);
            this._stateMatcher = false;
            return state;
        }
    }
};

textHighlighter = new TextHighlighter();
