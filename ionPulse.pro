
TEMPLATE  = subdirs

SUBDIRS = ionHeart ionTests ionParticles

ionTests.depends = ionHeart


deploy.depends = ionHeart ionParticles
deploy.commands = mkdir -p "$${OUT_PWD}/bin" && \
                  cp "$${OUT_PWD}/ionHeart/ionHeart" "$${OUT_PWD}/bin/ionHeart" && \
                  mkdir -p  "$${OUT_PWD}/bin/plugins" && \
                  cp "$${OUT_PWD}/ionParticles/ionEditor/libionEditor.so" "$${OUT_PWD}/bin/plugins/libionEditor.so"
QMAKE_EXTRA_TARGETS += deploy
