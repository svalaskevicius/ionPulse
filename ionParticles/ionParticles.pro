
TEMPLATE  = subdirs
CONFIG += ordered
SUBDIRS = ionEditor ionProject ionPhp

ionPhp.depends = ionEditor
