
TEMPLATE  = subdirs
CONFIG += ordered
SUBDIRS = ionLayout ionEditor ionProject ionPhp

ionPhp.depends = ionEditor
