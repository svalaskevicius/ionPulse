
TEMPLATE  = subdirs
CONFIG += ordered
SUBDIRS = ionCore ionParticles ionTests

ionTests.depends = ionCore ionParticles
