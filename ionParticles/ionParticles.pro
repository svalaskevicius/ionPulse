
TEMPLATE  = subdirs
CONFIG += ordered
SUBDIRS = ionLayout ionEditor ionProject ionDbXml ionPhp

ionEditor.depends = ionLayout
ionProject.depends = ionEditor
ionPhp.depends = ionEditor ionProject ionDbXml
