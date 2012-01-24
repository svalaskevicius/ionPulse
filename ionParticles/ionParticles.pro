
TEMPLATE  = subdirs
CONFIG += ordered
SUBDIRS = ionLayout ionEditor ionProject ionPhp

ionEditor.depends = ionLayout
ionProject.depends = ionEditor
ionPhp.depends = ionEditor ionProject
