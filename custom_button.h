#ifndef CUSTOM_BUTTON_H
#define CUSTOM_BUTTON_H

#include <QPushButton>
#include <QUrl>
#include <QDebug>

class custom_button : public QPushButton
{
public:
    custom_button();

private slots:
    void enterEvent(QEvent * event) {
        QWidget::setCursor(Qt::PointingHandCursor);
    }
};

#endif // CUSTOM_BUTTON_H
