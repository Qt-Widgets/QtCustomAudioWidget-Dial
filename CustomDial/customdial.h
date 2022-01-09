#ifndef CUSTOMDIAL_H
#define CUSTOMDIAL_H

#include <QWidget>
#include <QDial>
#include <QPainter>
#include <qmath.h>

class CustomDial : public QDial
{
public:
    CustomDial();
    CustomDial(QWidget *widget);

    bool dialWrapping = false;
    void setValueColor(QColor val);
    QColor getValueColor();

protected:
    void paintEvent(QPaintEvent *pe) override;

private:
    QColor valueColor = QColor(0,153,0,255);

};

#endif // CUSTOMDIAL_H
