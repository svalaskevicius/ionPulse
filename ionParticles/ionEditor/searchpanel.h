#ifndef SEARCHPANEL_H
#define SEARCHPANEL_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include <ionParticles/ionLayout/layoutapi.h>
#include <ionCore/shared.h>

class SearchPanel : public QWidget, public IonLayout::PanelWidget
{
    Q_OBJECT
public:
    explicit SearchPanel(QWidget *parent = 0);
    virtual QWidget *getWidget() {return this;}
    virtual QString getPanelZone() {return "central/central_footer";}

    void paintEvent(QPaintEvent*) {
        QStyleOption styleOption;
        styleOption.init(this);
        QPainter painter(this);
        style()->drawPrimitive(QStyle::PE_Widget, &styleOption, &painter, this);
    }
signals:

public slots:

};

#endif // SEARCHPANEL_H
