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
    QSlider* volume;

public:
    ThePlayer() : QMediaPlayer(NULL) {
        setVolume(0); // be slightly less annoying
        connect (this, SIGNAL (stateChanged(QMediaPlayer::State)), this, SLOT (playStateChanged(QMediaPlayer::State)) );

        mTimer = new QTimer(NULL);
        mTimer->setInterval(1000); // 1000ms is one second between ...
        mTimer->start();
        connect( mTimer, SIGNAL (timeout()), SLOT ( shuffle() ) ); // ...running shuffle method
    }
    void setScrub(QSlider* scrub);

    void setPlayPause(QPushButton* pausePlayButton);
    void setReplay(QPushButton* pausePlayButton);
    void setVolume(QSlider* volume);

    // all buttons have been setup, store pointers here
    void setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i);

private slots:

    // change the image and video for one button every one second
    void shuffle();

    void setPos();

    void setScrubPos();

    void ChangePlayOrPause();

    void Replay();

    void adjustVolume();

    void playStateChanged (QMediaPlayer::State ms);

public slots:

    // start playing this ButtonInfo

    void jumpTo (TheButtonInfo* button);
};

#endif //CW2_THE_PLAYER_H
