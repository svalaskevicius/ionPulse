
TEMPLATE  = subdirs
CONFIG += ordered
SUBDIRS = ionCore ionParticles

ionTests.depends = ionCore ionParticles
