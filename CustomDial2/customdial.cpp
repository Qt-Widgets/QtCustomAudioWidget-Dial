#include "customdial.h"

static const qreal Q_PI   = qreal(M_PI);     // pi

CustomDial::CustomDial()
{

}

CustomDial::CustomDial(QWidget *widget)
{

}

void CustomDial::setValueColor(QColor val)
{
    this->valueColor = val;
}

QColor CustomDial::getValueColor()
{
    return this->valueColor;
}

static int calcBigLineSize(int radius)
{
    int bigLineSize = radius / 6;
    if (bigLineSize < 4)
        bigLineSize = 4;
    if (bigLineSize > radius / 2)
        bigLineSize = radius / 2;
    return bigLineSize;
}

static QPointF calcRadialPos(const CustomDial *dial, QRectF rectF, qreal offset)
{
    const int width = dial->rect().width();
    const int height = dial->rect().height();
    const int r = qMin(rectF.width(), rectF.height()) / 2;
    const int currentSliderPosition = dial->sliderPosition();
    qreal a = 0;
    if (dial->maximum() == dial->minimum())
        a = Q_PI / 2;
    else if (dial->dialWrapping)
        a = Q_PI * 3 / 2 - (currentSliderPosition - dial->minimum()) * 2 * Q_PI
            / (dial->maximum() - dial->minimum());
    else
        a = (Q_PI * 8 - (currentSliderPosition - dial->minimum()) * 10 * Q_PI
            / (dial->maximum() - dial->minimum())) / 6;
    qreal xc = width / 2.0;
    qreal yc = height / 2.0;
    qreal len = r;
    qreal back = offset * len;
    QPointF pos(QPointF(xc + back * qCos(a), yc - back * qSin(a)));
    return pos;
}

QVector<QPolygonF> calcLines(const QStyleOptionSlider *dial)
{
    QVector<QPolygonF> ret;
    QPolygonF poly;
    QPolygonF valPoly;
    int width = dial->rect.width();
    int height = dial->rect.height();
    qreal r = qMin(width, height) / 2;
    int bigLineSize = calcBigLineSize(int(r));
    qreal xc = width / 2 + 0.5;
    qreal yc = height / 2 + 0.5;
    const int ns = dial->tickInterval;
    if (!ns) // Invalid values may be set by Qt Designer.
        return ret;
    int notches = (dial->maximum - dial->minimum) / ns + 1;
    int valNotches = (dial->sliderValue - dial->minimum) / ns + 1;
    if (notches <= 0)
        return ret;
    if (dial->maximum < dial->minimum || dial->maximum - dial->minimum > 1000) {
        int maximum = dial->minimum + 1000;
        notches = (maximum - dial->minimum) / ns + 1;
    }
    poly.resize(2 + 2 * notches);
    int smallLineSize = bigLineSize / 2;
    for (int i = 0; i <= notches; ++i) {
        qreal angle = dial->dialWrapping ? Q_PI * 3 / 2 - i * 2 * Q_PI / notches
                  : (Q_PI * 8 - i * 10 * Q_PI / notches) / 6;
        qreal s = qSin(angle) * 0.88;
        qreal c = qCos(angle) * 0.88;
        if (i == 0 || (((ns * i) % (dial->pageStep ? dial->pageStep/2 : 1))) == 0 ||
            i == notches) {
            poly[2 * i] = QPointF(xc + (r - bigLineSize) * c,
                                  yc - (r - bigLineSize) * s);
            poly[2 * i + 1] = QPointF(xc + r * c, yc - r * s);
            if (valNotches >= i) {
                valPoly.push_back(QPointF(xc + (r - bigLineSize) * c,
                                          yc - (r - bigLineSize) * s));
                valPoly.push_back(QPointF(xc + r * c, yc - r * s));
            }
        }
    }
    ret.push_back(poly);
    ret.push_back(valPoly);
    return ret;
}

void CustomDial::paintEvent(QPaintEvent *pe)
{
    QPainter *painter = new QPainter(this);
    painter->setRenderHint(QPainter::Antialiasing);

    QStyleOptionSlider option;
    initStyleOption(&option);

    const int width = this->rect().width();
    const int height = this->rect().height();
    qreal r = qMin(width, height) / 2;

    const qreal d_ = r / 6;
    const qreal penSize = r/20.0;
    const qreal dx = this->rect().x() + d_ + (width - 2 * r) / 2 + 1;
    const qreal dy = this->rect().y() + d_ + (height - 2 * r) / 2 + 1;
    QRectF br = QRectF(dx + 0.5, dy + 0.5,
                       int(r * 2 - 2 * d_ - 2),
                       int(r * 2 - 2 * d_ - 2));
    painter->setPen(QPen(QColor(64,64,64),penSize*0.6));
    QVector<QPolygonF> linesPoly = calcLines(&option);
    painter->drawLines(linesPoly[0]);
    if (this->value() > 0) {
        painter->setPen(QPen(this->valueColor,penSize*0.6));
        painter->drawLines(linesPoly[1]);
    }

    // solid circle
    qreal circleX = br.center().x();
    qreal circleY = br.center().y();
    qreal circleWidth = br.width()*0.8;
    qreal circleHeight = br.height()*0.8;
    QRectF brCircle = QRectF(circleX - circleWidth/2,
                             circleY - circleHeight/2,
                             circleWidth,
                             circleHeight);

    painter->setPen(QPen(QColor(48,62,81).lighter(120),penSize));
    QRadialGradient solidCircle(brCircle.center().x(),
                                brCircle.center().y(),
                                brCircle.width()/2,
                                brCircle.center().x(),
                                brCircle.center().y());
    solidCircle.setColorAt(0, QColor(48,62,81).lighter(110));
    solidCircle.setColorAt(0.5,QColor(48,62,81));
    solidCircle.setColorAt(1.0,QColor(48,62,81).lighter(115));
    painter->setBrush(solidCircle);
    painter->drawEllipse(brCircle);

    // hollow circle
    qreal hollowCircleWidth = circleWidth*0.96;
    qreal hollowCircleHeight = circleHeight*0.96;
    QRectF brHollowCircle = QRectF(circleX - hollowCircleWidth/2,
                                   circleY - hollowCircleHeight/2,
                                   hollowCircleWidth,
                                   hollowCircleHeight);
    painter->setPen(QPen(QColor(224,224,224).darker(105),penSize*0.6));
    painter->drawEllipse(brHollowCircle);

    // line
    painter->setPen(QPen(this->valueColor,penSize*1.4));
    painter->drawLine(calcRadialPos(this, brHollowCircle, qreal(0.65)),
                      calcRadialPos(this, brHollowCircle, qreal(0.912)));
}
