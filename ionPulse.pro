

TEMPLATE  = subdirs
CONFIG += ordered
SUBDIRS = ionHeart ionParticles ionTests

ionTests.depends = ionHeart ionParticles
