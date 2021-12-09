//
// Created by twak on 11/11/2019.
//

#include "the_player.h"
#include <iostream>

// all buttons have been setup, store pointers here
void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i) {
    buttons = b;
    infos = i;
    jumpTo(buttons -> at(0) -> info);
    std::cout << "duration of media: " << this->duration() << std::endl;
    scrub->setMaximum(this->duration());
    scrub->setValue(this->position());
}


// ensure that 'scrub' attribute points to a slider element
void ThePlayer::setScrub(QSlider* s) {
    scrub = s;
    connect(scrub, SIGNAL(sliderReleased()), this, SLOT(setPos()));
    connect(scrub, SIGNAL(sliderPressed()), this, SLOT(pause()));
    connect( mTimer, SIGNAL (timeout()),  SLOT (setScrubPos()));
    configureScrub();
}

void ThePlayer::setVideo(QVideoWidget* v) {
    video = v;
}

void ThePlayer::setVolumeSlider(QSlider* v) {
    volumeSlider = v;
}

//Playback controls
void ThePlayer::setPlayPause(QPushButton* p) {
    pausePlayButton = p;
    connect(pausePlayButton, SIGNAL(clicked()), this, SLOT(ChangePlayOrPause()));
}

void ThePlayer::setReplay(QPushButton* p) {
    replayButton = p;
    connect(replayButton, SIGNAL(clicked()), this, SLOT(Replay()));
}

void ThePlayer::Replay() {
    //resets the video to the begining not the slider
    pause();
    setPosition(0);
    play();
    scrub->setValue(0);
    pause();
    pausePlayButton->setIcon(QIcon(":play-icon.png"));
    playing = 0;
}

void ThePlayer::ChangePlayOrPause() {
    if(playing == 1){
        playing = 0;
        pausePlayButton->setIcon(QIcon(":play-icon.png"));
        pausePlayButton->setToolTip("Play");
        mTimer->stop(); // stops scrub from updating while paused
        pause();
    }
    else{
        playing =1;
        mTimer->start();
        pausePlayButton->setIcon(QIcon(":pause-icon.png"));
        pausePlayButton->setToolTip("Pause");
        play();
    }
}

// set position of the media based on the position of the slider
void ThePlayer::setPos() {
    int value;
    value = scrub->value();
    setPosition(value);
    play();
}

// update scrub position every interval
void ThePlayer::setScrubPos() {
    int value;
//    std::cout << "MAXIMUM: " << scrub->maximum() << std::endl;
    value = this->position();
    scrub->setValue(value);
}

// configure scrub when duration of video is determined
void ThePlayer::configureScrub() {
    scrub->setMaximum(this->duration());
    scrub->setValue(this->position());
}

//An event filter that controls what keypress does what on a global scale
bool ThePlayer::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::KeyPress)
    {
        QKeyEvent *event= dynamic_cast<QKeyEvent*>(e);

        //switches for keypresses
        switch(event->key()) {
        case Qt::Key_Up: volumeSlider->setValue(volumeSlider->value() + 10); break;
        case Qt::Key_Down: volumeSlider->setValue(volumeSlider->value() - 10); break;
        case Qt::Key_Left:
            scrub->setValue(scrub->value() - 5000);
            this->setPosition(scrub->value());
            break;
        case Qt::Key_Right:
            scrub->setValue(scrub->value() + 5000);
            this->setPosition(scrub->value());
            break;
        case Qt::Key_Space: {   //need to check if the video is at the end already
            if(this->state() != QMediaPlayer::State::StoppedState)
                ChangePlayOrPause();
            else {
                replayButton->click();
                ChangePlayOrPause();
            }
            break;
        }

        default:
            return false;
        }
        return true;
    }
    return false;
}

void ThePlayer::playStateChanged (QMediaPlayer::State ms) {
    switch (ms) {
        case QMediaPlayer::State::StoppedState:
            //play(); // starting playing again...
            break;
    default:
        break;
    }
}

void ThePlayer::jumpTo (TheButtonInfo* button) {
    setMedia( * button -> url);
    play();
}
