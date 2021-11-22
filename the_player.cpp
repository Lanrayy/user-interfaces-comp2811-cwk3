//
// Created by twak on 11/11/2019.
//

#include "the_player.h"

// all buttons have been setup, store pointers here
void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i) {
    buttons = b;
    infos = i;
    jumpTo(buttons -> at(0) -> info);
}

// ensure that 'scrub' attribute points to a slider element
void ThePlayer::setScrub(QSlider* s) {
    scrub = s;
    connect(scrub, SIGNAL(valueChanged(int)), this, SLOT(setPos()));
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
    pause();
    value = (scrub->value() * duration())/scrub->maximum();
    setPosition(value);
    play();
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
