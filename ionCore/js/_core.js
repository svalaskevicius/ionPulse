/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

Array.prototype.each = function(callback) {
    for (var i=0; i<this.length;i++) {
        callback(this[i]);
    }
};

Function.prototype.inheritsFrom = function(parentClassOrObject) {
    if ( parentClassOrObject.constructor == Function ) {
        //Normal Inheritance
        this.prototype = new parentClassOrObject;
        this.prototype.constructor = this;
        this.prototype.parent = parentClassOrObject.prototype;
    } else {
        //Pure Virtual Inheritance
        this.prototype = parentClassOrObject;
        this.prototype.constructor = this;
        this.prototype.parent = parentClassOrObject;
    }
    return this;
};

var importFailures = new Array();
["qt.core", "qt.gui", "qt.xml", "qt.network",
 "qt.sql", "qt.opengl"
//, "qt.webkit", "qt.xmlpatterns", "qt.svg","qt.uitools"
].each(
    function(ext) {
        try {
            qs.script.importExtension(ext);
        } catch (e) {
            importFailures.push(e);
        }
    }
);



is_string = function(input)
{
    return typeof(input)=='string';
};

to_string = function(input)
{
    if (this.is_string(input)) {
        return input;
    }

    if (undefined === input) {
        return "undefined";
    }
    if (null === input) {
        return "null";
    }
    return input.toString();
};

escape = function(text)
{
    return this.to_string(text).replace(/&/g, '&amp;').replace(/>/g, '&gt;').replace(/</g, '&lt;');
};

trim = function(str) {
    str = this.to_string(str).replace(/^\s\s*/, '')
    var ws = /\s/, i = str.length;
    while ((i>0) && ws.test(str.charAt(--i)));
    return str.slice(0, i + 1);
};

alert = function(text)
{
    QMessageBox.information(this, "ionPulse - alert", "<html><pre>"+this.escape(text));
};

for (var i = 0; i < importFailures.length; i++) {
    alert(importFailures[i]);
}
