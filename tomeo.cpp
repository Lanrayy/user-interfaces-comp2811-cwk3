//    ______
//   /_  __/___  ____ ___  ___  ____
//    / / / __ \/ __ `__ \/ _ \/ __ \
//   / / / /_/ / / / / / /  __/ /_/ /
//  /_/  \____/_/ /_/ /_/\___/\____/
//              video for sports enthusiasts...
//
//  2811 cw3 : twak 11/11/2021
//

#include <iostream>
#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QMediaPlaylist>
#include <string>
#include <vector>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QScrollArea>
#include <QWidget>
#include <QSlider>
#include <QMessageBox>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileIconProvider>
#include <QDesktopServices>
#include <QImageReader>
#include <QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include "the_player.h"
#include "the_button.h"

// read in videos and thumbnails to this directory
std::vector<TheButtonInfo> getInfoIn (std::string loc) {

    std::vector<TheButtonInfo> out =  std::vector<TheButtonInfo>();
    QDir dir(QString::fromStdString(loc) );
    QDirIterator it(dir);

    while (it.hasNext()) { // for all files

        QString f = it.next();

            if (f.contains("."))

#if defined(_WIN32)
            if (f.contains(".wmv"))  { // windows
#else
            if (f.contains(".mp4") || f.contains("MOV"))  { // mac/linux
#endif

            QString thumb = f.left( f .length() - 4) +".png";
            if (QFile(thumb).exists()) { // if a png thumbnail exists
                QImageReader *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // read the thumbnail
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // voodoo to create an icon for the button
                        QUrl* url = new QUrl(QUrl::fromLocalFile( f )); // convert the file location to a generic url
                        out . push_back(TheButtonInfo( url , ico  ) ); // add to the output list
                    }
                    else
                        qDebug() << "warning: skipping video because I couldn't process thumbnail " << thumb << endl;
            }
            else
                qDebug() << "warning: skipping video because I couldn't find thumbnail " << thumb << endl;
        }
    }

    return out;
}


int main(int argc, char *argv[]) {

    // let's just check that Qt is operational first
    qDebug() << "Qt version: " << QT_VERSION_STR << endl;

    // create the Qt Application
    QApplication app(argc, argv);

    // collect all the videos in the folder
    std::vector<TheButtonInfo> videos;

    if (argc == 2)
        videos = getInfoIn( std::string(argv[1]) );

    if (videos.size() == 0) {

        const int result = QMessageBox::question(
                    NULL,
                    QString("Tomeo"),
                    QString("no videos found! download, unzip, and add command line argument to \"quoted\" file location. Download videos from Tom's OneDrive?"),
                    QMessageBox::Yes |
                    QMessageBox::No );

        switch( result )
        {
        case QMessageBox::Yes:
          QDesktopServices::openUrl(QUrl("https://leeds365-my.sharepoint.com/:u:/g/personal/scstke_leeds_ac_uk/EcGntcL-K3JOiaZF4T_uaA4BHn6USbq2E55kF_BTfdpPag?e=n1qfuN"));
          break;
        default:
            break;
        }
        exit(-1);
    }

    // the widget that will show the video
    QVideoWidget *videoWidget = new QVideoWidget;
    videoWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);


    // the layout that will allow for controls and a slider to be added in one main area
    // (and the widget that holds it)
    QWidget *playerWidget = new QWidget();
    QVBoxLayout *mediaLayout = new QVBoxLayout;

    // the QMediaPlayer which controls the playback
    ThePlayer *player = new ThePlayer;

    // the scrub that allows shifting through the video
    QSlider *scrub = new QSlider;

    // piece everything together
    player->setVideoOutput(videoWidget);
    player->setScrub(scrub);
    scrub->setOrientation(Qt::Horizontal);
    mediaLayout->addWidget(videoWidget);
    mediaLayout->addWidget(scrub);
    playerWidget->setLayout(mediaLayout);


    // a column of buttons
    QWidget *buttonWidget = new QWidget();  //this is the container for the thumbnails
    // a list of the buttons
    std::vector<TheButton*> buttons;
    // the buttons are arranged vertically
    QGridLayout *layout = new QGridLayout();
    buttonWidget->setLayout(layout);

    // create the thumbnails
    for (int i = 0; i < videos.size(); i++ ) { //for each video in the videos vector
        TheButton *button = new TheButton(buttonWidget);    //this is an instance of the button class
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo* )), player, SLOT (jumpTo(TheButtonInfo*))); // when clicked, tell the player to play.
        buttons.push_back(button);
        layout->addWidget(button, i / 2, i % 2, Qt::AlignHCenter);
        button->init(&videos.at(i));
    }
    layout->setAlignment(Qt::AlignHCenter);

    // tell the player what buttons and videos are available
    player->setContent(&buttons, & videos);

    //create scrollArea for the thumbnail container
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(buttonWidget);
    scrollArea->setMaximumWidth(465);   //makes the maximum size for the thumbnail column fit 2 thumbnails with appropriate spacing
    scrollArea->setMinimumWidth(465);

    // create the main window and layout
    QWidget window;
    QGridLayout *top = new QGridLayout();
    top->setSizeConstraint(QLayout::SetDefaultConstraint);
    window.setLayout(top);
    window.setWindowTitle("tomeo");
    window.setMinimumSize(800, 680);

    //creating the layout and its  widget
    QWidget *browseButtonsWidget = new QWidget();
    QHBoxLayout *browseButtonLayout = new QHBoxLayout();
    browseButtonsWidget->setLayout(browseButtonLayout); // set layout


    //create add video button & set its size
    QPushButton *addVideo = new QPushButton();
    addVideo->setText("Add video...");
    addVideo->setFixedSize(200, 30);
    // create message box
    QMessageBox *addVideoMessage = new QMessageBox();
    addVideoMessage->setWindowTitle("Browse...");
    addVideoMessage->setText("Congratulations, you've added new a new video");
    // connect add video button to message box
    QObject::connect(addVideo, SIGNAL(clicked()), addVideoMessage, SLOT(exec()));


    //create import folder button & set its size
    QPushButton *importFolder = new QPushButton();
    importFolder->setText("Import folder...");
    importFolder->setFixedSize(200, 30);
    // create message box
    QMessageBox *importFolderMessage = new QMessageBox();
    importFolderMessage->setWindowTitle("Import folder");
    importFolderMessage->setText("Congratulations, you've imported a new folder");
    QObject::connect(importFolder, SIGNAL(clicked()), importFolderMessage, SLOT(exec()));

    //adding widgets to layout
    browseButtonLayout->addWidget(addVideo);
    browseButtonLayout->addWidget(importFolder);


    // add the video and the buttons to the top level QGridLayout
    //the way items in the GridLayout works is you designate their Row then Column, then how many Rows they span over, then how many Cols they span over
    top->addWidget(playerWidget, 0, 0);   //video widget is row 0 col 0
    top->setColumnStretch(0, 1);
    top->addWidget(scrollArea, 0, 1); //scollArea is row 0 col 1
    top->addWidget(browseButtonsWidget, 1,1);
    top->setColumnStretch(1, 1);

    // showtime!
    window.show();

    // wait for the app to terminate
    return app.exec();
}