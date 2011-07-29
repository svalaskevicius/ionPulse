
TEMPLATE  = subdirs
CONFIG += ordered
SUBDIRS = ionEditor ionPhp

ionPhp.depends = ionEditor
