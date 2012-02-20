#include "searchpanel.h"

SearchPanel::SearchPanel(QWidget *parent) :
    QWidget(parent)
{
    DEBUG_MSG( metaObject()->className() );
    //setStyleSheet("background-color: white;");
}
