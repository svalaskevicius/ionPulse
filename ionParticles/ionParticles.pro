
TEMPLATE  = subdirs
CONFIG += ordered
SUBDIRS = ionLayout ionEditor

ionEditor.depends = ionLayout
