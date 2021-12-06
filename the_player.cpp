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


//Playback controls
void ThePlayer::setPlayPause(QPushButton* p) {
    pausePlayButton = p;
    connect(pausePlayButton, SIGNAL(clicked()), this, SLOT(ChangePlayOrPause()));
}

void ThePlayer::setReplay(QPushButton* p) {
    replayButton = p;
    connect(replayButton, SIGNAL(clicked()), this, SLOT(Replay()));
}
void ThePlayer::setVolume(QSlider* v) {
    volume = v;
    connect(volume, SIGNAL(valueChanged(int)), this, SLOT(adjustVolume()));
}

void ThePlayer::adjustVolume() {
       setVolume(volume);
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
<<<<<<< HEAD
        pausePlayButton->setIcon(QIcon(":play-icon.png"));
=======
        pausePlayButton->setText(">");
        mTimer->stop(); // stops scrub from updating while paused
>>>>>>> 66a6921f28b3a7041c1cd9d7c68ba754d150f823
        pause();
    }
    else{
        playing =1;
<<<<<<< HEAD
        pausePlayButton->setIcon(QIcon(":pause-icon.png"));
=======
        mTimer->start();
        pausePlayButton->setText("||");
>>>>>>> 66a6921f28b3a7041c1cd9d7c68ba754d150f823
        play();
    }
}

// change the image and video for one button every one second
void ThePlayer::shuffle() {
    TheButtonInfo* i = & infos -> at (rand() % infos->size() );
//        setMedia(*i->url);
    buttons -> at( updateCount++ % buttons->size() ) -> init( i );
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
    std::cout << "MAXIMUM: " << scrub->maximum() << std::endl;
    value = this->position();
    scrub->setValue(value);
}

// configure scrub when duration of video is determined
void ThePlayer::configureScrub() {
    scrub->setMaximum(this->duration());
    scrub->setValue(this->position());
}

void ThePlayer::playStateChanged (QMediaPlayer::State ms) {
    switch (ms) {
        case QMediaPlayer::State::StoppedState:
            play(); // starting playing again...
            break;
    default:
        break;
    }
}

void ThePlayer::jumpTo (TheButtonInfo* button) {
    setMedia( * button -> url);
    play();
}
