//
// Created by twak on 11/11/2019.
//

#ifndef CW2_THE_PLAYER_H
#define CW2_THE_PLAYER_H


#include <QApplication>
#include <QMediaPlayer>
#include "the_button.h"
#include <vector>
#include <QTimer>
#include <QSlider>
#include <QPushButton>
#include <QVideoWidget>
#include <QEvent>
#include <QKeyEvent>

class ThePlayer : public QMediaPlayer {

Q_OBJECT

private:
    std::vector<TheButtonInfo>* infos;
    std::vector<TheButton*>* buttons;
    QTimer* mTimer;
    QSlider* scrub;
    long updateCount = 0;

    int playing =1;
    QPushButton* pausePlayButton;
    QPushButton* replayButton;
    QSlider* volumeSlider;
    QVideoWidget* video;

public:
    ThePlayer() : QMediaPlayer(NULL) {
        setVolume(0); // be slightly less annoying
        connect (this, SIGNAL (stateChanged(QMediaPlayer::State)), this, SLOT (playStateChanged(QMediaPlayer::State)));
        connect (this, SIGNAL (durationChanged(qint64)), this, SLOT (configureScrub()));

        mTimer = new QTimer(NULL);
        mTimer->setInterval(1000); // 1000ms is one second between ...
        mTimer->start();
        //connect( mTimer, SIGNAL (timeout()), SLOT ( shuffle() ) ); // ...running shuffle method
    }
    void setScrub(QSlider* scrub);

    void setVideo(QVideoWidget* video);

    void setVolumeSlider(QSlider* volumeSlider);

    void setPlayPause(QPushButton* pausePlayButton);
    void setReplay(QPushButton* pausePlayButton);

    //key board listener
    bool eventFilter(QObject *obj, QEvent *e);


    // all buttons have been setup, store pointers here
    void setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i);

private slots:

    void setPos();

    void setScrubPos();

    void configureScrub();

    void ChangePlayOrPause();

    void Replay();

    void playStateChanged (QMediaPlayer::State ms);

public slots:

    // start playing this ButtonInfo

    void jumpTo (TheButtonInfo* button);
};

#endif //CW2_THE_PLAYER_H
