

TEMPLATE  = subdirs
CONFIG += ordered
SUBDIRS = ionHeart ionTests ionParticles

ionTests.depends = ionHeart
