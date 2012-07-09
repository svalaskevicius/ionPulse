/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

TextHighlighter = function () {};
TextHighlighter.prototype = {
    createCharFormat: function (color, weight, italic, size) {
        if (typeof size === 'undefined') {
            size = 14;
        }
        var format = new QTextCharFormat();
        var brush = new QBrush();
        var font = new QFont("Monaco", size, weight, italic);
        if (!font.exactMatch()) {
            font = new QFont("Courier New", size, weight, italic);
        }
        brush.setColor(color);
        brush.setStyle(Qt.SolidPattern);
        format.setForeground(brush);
        format.setFont(font);
        return format;
    },

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

    charFormatting: {},
    transitions: {},
    highlightRules: {},

    _hightlightState: function (state, from, to) {
        this._cppApi.setFormat(from, to - from, this.charFormatting[state]);
        for (var rule in this.highlightRules[state]) {
            var re = this.highlightRules[state][rule];
            re.lastIndex = from;
            var idx = -1;
            do {
                var match = re.exec(this._text);
                if (match && match.index < to) {
                    idx = match.index;
                    this._cppApi.setFormat(idx, match[0].length, this.charFormatting[state + "/" + rule]);
                } else {
                    idx = -1;
                }
            } while (idx >= 0);
        }
    },

    _processState: function (state) {
        var minPos = false;
        var minState = false;
        for (var newState in this.transitions[state]) {
            var statePos = this.transitions[state][newState]();
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
    },

    initialize: function () {
        this.states = [];
        for (var state in this.transitions) {
            this.states.push(state);
        }
    },
    highlight: function (cppApi, text) {
        this._cppApi = cppApi;
        this._text = text;
        this._stateMatcher = {
            'start': 0,
            'next': 0
        };
        var stateId = cppApi.previousBlockState;
        if (stateId < 0) {
            stateId = 0;
        }
        var state = this.states[stateId];
        while (this._stateMatcher) {
            state = this._processState(state);
        }
        stateId = this.states.indexOf(state);
        if (cppApi.currentBlockState != stateId) {
            cppApi.currentBlockState = stateId;
        }
    }
};
