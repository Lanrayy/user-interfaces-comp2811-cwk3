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
#include <QtWidgets>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QScrollArea>
#include <QFileDialog>
#include <QSlider>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileIconProvider>
#include <QDesktopServices>
#include <QImageReader>
#include <QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include "the_player.h"
#include "the_button.h"
#include "custom_button.h"
using namespace std;

//define global variables
std::vector<QString> vidTitles;

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
            string tempTitle = f.left( f .length() - 4).toStdString();
            vidTitles.push_back(QString::fromStdString(tempTitle.substr(tempTitle.find_last_of('/') + 1)));
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
    videoWidget->setStyleSheet("background-color: black");


    // the layout that will allow for controls and a slider to be added in one main area
    // (and the widget that holds it)
    QWidget *playerWidget = new QWidget();
    QVBoxLayout *mediaLayout = new QVBoxLayout;

    // the QMediaPlayer which controls the playback
    ThePlayer *player = new ThePlayer;

    // the scrub that allows shifting through the video
    QSlider *scrub = new QSlider;
    scrub->setStyleSheet("selection-background-color: #3B8DF1");


    // Creating a layout for all the contols to go in
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    //creating a widget for the layout to go in so it can be added to the main layout
    QWidget *buttonWidgetControls = new QWidget();
    buttonWidgetControls->setLayout(controlsLayout);
    controlsLayout->setSpacing(0);
    controlsLayout->setAlignment(Qt::AlignCenter);


    //volume & pause/play & replay buttons
    QSlider *volume = new QSlider;
    player->setVolumeSlider(volume);
    volume->setOrientation(Qt::Horizontal);
    volume->setRange(0,100);
    volume->setFixedWidth(80);
//    volume->setTickPosition(QSlider::TicksBelow);

    //create the volume Icon
    custom_button *volumeIcon = new custom_button();
    volumeIcon->setFixedSize(25,25);
    volumeIcon->setIcon(QIcon(":volume-icon.png"));
    volumeIcon->setIconSize(QSize(25,25));
    volumeIcon->setStyleSheet("border: none");

    //create the layout for volume
    QHBoxLayout *volumeLayout = new QHBoxLayout();
    volumeLayout->addWidget(volumeIcon);
    volumeLayout->addWidget(volume);
    volumeLayout->addStretch();
    volumeLayout->setSpacing(0);
    volumeLayout->setAlignment(Qt::AlignLeft);

    //volume container
    custom_button *volumeContainer = new custom_button();
    volumeContainer->setLayout(volumeLayout);
    volumeContainer->setFixedWidth(120);
    volumeContainer->setFixedHeight(50);
    volumeContainer->setStyleSheet("border: none");

    custom_button *pausePlayButton = new custom_button();
    pausePlayButton ->setFixedSize(120,50);
    pausePlayButton->setIcon(QIcon(":pause-icon.png"));
    pausePlayButton->setIconSize(QSize(50,50));
    pausePlayButton->setStyleSheet("border: none");


    custom_button *replayButton = new custom_button();
    replayButton ->setFixedSize(120,50);
    replayButton->setIcon(QIcon(":replay-icon.png"));
    replayButton->setIconSize(QSize(25,25));
    replayButton->setStyleSheet("border: none");

    //add the button to the controlsLayout.
    controlsLayout->addWidget(volumeContainer);
    controlsLayout->addWidget(pausePlayButton);
    controlsLayout->addWidget(replayButton);

    player->setScrub(scrub);

    // piece everything together
    player->setVideoOutput(videoWidget);
    player->setVideo(videoWidget);
    player->setScrub(scrub);

    //volume slider connects to player's volume and vice versa
    player->connect(volume, SIGNAL(valueChanged(int)), player, SLOT(setVolume(int)));
    player->setVolume(50);   //initial volume 50
    volume->setValue(50);

    //playback for player set
    player->setPlayPause(pausePlayButton);
    player->setReplay(replayButton);

    //video scrub
    scrub->setOrientation(Qt::Horizontal);
    mediaLayout->addWidget(videoWidget);
    mediaLayout->addWidget(scrub);
    mediaLayout->addWidget(buttonWidgetControls);

    //video player
    playerWidget->setLayout(mediaLayout);
    playerWidget->setStyleSheet("background-color: #25282a");

    // a column of buttons
    QWidget *buttonWidget = new QWidget();  //this is the container for the thumbnails
//    buttonWidget->setStyleSheet("background-color: #666666");
    // a list of the buttons
    std::vector<TheButton*> buttons;
    // the buttons are arranged vertically
    QGridLayout *layout = new QGridLayout();
    buttonWidget->setLayout(layout);

    QLabel *libraryHeader = new QLabel("Your Library");
    libraryHeader->setStyleSheet("QLabel { color: #ACACAC; font-weight: bold ; font-size: 20px}");
    layout->addWidget(libraryHeader, 0, 0, Qt::AlignLeft);
    layout->setRowStretch(0, 1);

    // create the thumbnails
    for (int i = 0; i < videos.size(); i++ ) { //for each video in the videos vector
        TheButton *button = new TheButton(buttonWidget); //this is an instance of the button class
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo* )), player, SLOT (jumpTo(TheButtonInfo*))); // when clicked, tell the player to play.
        buttons.push_back(button);

        //create the vid title to accompany the thumbnail
        QLabel *title = new QLabel(vidTitles.at(i));
        title->setStyleSheet("QLabel { color: #E3E3E3;}");

        //add the video to the button
        button->init(&videos.at(i));

        //create a container and layout for both the button and it's title
        QWidget *thumbnailContainer = new QWidget();
        QVBoxLayout *thumbnailLayout = new QVBoxLayout();
        thumbnailLayout->addWidget(button);
        thumbnailLayout->addWidget(title);

        //marry the layout to the container
        thumbnailContainer->setLayout(thumbnailLayout);

        //add button and label to the layout
        layout->addWidget(thumbnailContainer, (i / 2) + 1, i % 2, Qt::AlignHCenter);
    }
    layout->setAlignment(Qt::AlignHCenter);

    // tell the player what buttons and videos are available
    player->setContent(&buttons, & videos);

    //create scrollArea for the thumbnail container
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(buttonWidget);
    scrollArea->setMaximumWidth(500); //530?
    scrollArea->setMinimumWidth(500); //makes the maximum size for the thumbnail column fit 2 thumbnails with appropriate spacing


    // create the main window and layout
    QWidget window;
    QGridLayout *top = new QGridLayout();
    top->setSizeConstraint(QLayout::SetDefaultConstraint);
    window.setLayout(top);
    window.setWindowTitle("tomeo");
    window.setMinimumSize(930, 680);
//    window.setStyleSheet("background-color: #333333");
    window.setStyleSheet("background-color: #25282a");

    //Browse button
    //creating the layout and its  widget
    QWidget *browseButtonsWidget = new QWidget();
    QHBoxLayout *browseButtonLayout = new QHBoxLayout();
    browseButtonsWidget->setLayout(browseButtonLayout); // set layout

    //File browser Dialog Boxes
    QFileDialog *addVideoDialogBox = new QFileDialog();
    addVideoDialogBox->setFileMode(QFileDialog::ExistingFiles);
    addVideoDialogBox->setStyleSheet("background-color: #333333");
    addVideoDialogBox->setNameFilter(("Videos (*.mov *.mp4 *.wmv)")); //only video files are displayed

    //IMPORT FOLDER DIALOG BOX
    QFileDialog *importFolderDialogBox = new QFileDialog();
    importFolderDialogBox->setFileMode(QFileDialog::DirectoryOnly);
    importFolderDialogBox->setNameFilter(("Videos (*.mov *.mp4 *.wmv)"));

    //create add video button & set its size
    custom_button *addVideoButton = new custom_button();
    addVideoButton->setText("Add video...");
    addVideoButton->setFixedSize(200, 30);
    addVideoButton->setStyleSheet("background-color: #3B8DF1; border-radius: 15px");
    // create success message box
    QMessageBox *addVideoSuccessMessage = new QMessageBox();
    addVideoSuccessMessage->setIcon(QMessageBox::Information);
    addVideoSuccessMessage->setWindowTitle("Add Video...");
    addVideoSuccessMessage->setText("Congratulations, you've added new a new video");
    addVideoSuccessMessage->setStyleSheet("background-color: #333333; color: white");
    QAbstractButton* ok1 = addVideoSuccessMessage->addButton(("Okay!"), QMessageBox::YesRole);
    ok1->setStyleSheet("background-color: #3B8DF1; border-radius: 10px; width: 100px; height: 25px; color: black");
    //failure message box
    QMessageBox *addVideoFailureMessage = new QMessageBox();
    addVideoFailureMessage->setIcon(QMessageBox::Warning);
    addVideoFailureMessage->setWindowTitle("Warning!");
    addVideoFailureMessage->setText("Unable to add video. PLease try again.");
    addVideoFailureMessage->setStyleSheet("background-color: #333333; color: white");
    QAbstractButton* ok2 = addVideoFailureMessage->addButton(("Okay!"), QMessageBox::YesRole);
    ok2->setStyleSheet("background-color: #3B8DF1; border-radius: 10px; width: 100px; height: 25px; color: black");

    // slots and signals for add video
    QObject::connect(addVideoButton, SIGNAL(clicked()), addVideoDialogBox, SLOT(exec()));
    QObject::connect(addVideoDialogBox, SIGNAL(accepted()), addVideoSuccessMessage, SLOT(exec()));
    QObject::connect(addVideoDialogBox, SIGNAL(rejected()), addVideoFailureMessage, SLOT(exec())); // adding folder unsuccessful

    //create import folder button & set its size
    custom_button *importFolderButton = new custom_button();
    importFolderButton->setStyleSheet("background-color: #3B8DF1; border-radius: 15px");
    importFolderButton->setText("Import folder...");
    importFolderButton->setFixedSize(200, 30);

    // IMPORT FOLDER
    QMessageBox *importFolderSuccessMessage = new QMessageBox();
    importFolderSuccessMessage->setIcon(QMessageBox::Information);
    importFolderSuccessMessage->setWindowTitle("Import folder...");
    importFolderSuccessMessage->setText("Congratulations, you've imported a new folder");
    importFolderSuccessMessage->setStyleSheet("background-color: #333333; color: white");
    QAbstractButton* ok3 = importFolderSuccessMessage->addButton(("Okay!"), QMessageBox::YesRole);
    ok3->setStyleSheet("background-color: #3B8DF1; border-radius: 10px; width: 100px; height: 25px; color: black");

    QMessageBox *importFolderFailureMessage = new QMessageBox();
    importFolderFailureMessage->setIcon(QMessageBox::Warning);
    importFolderFailureMessage->setWindowTitle("Warning!");
    importFolderFailureMessage->setText("Unable to import folder. PLease try again.");
    importFolderFailureMessage->setStyleSheet("background-color: #333333; color: white");
    QAbstractButton* ok4 = importFolderFailureMessage->addButton(("Okay!"), QMessageBox::YesRole);
    ok4->setStyleSheet("background-color: #3B8DF1; border-radius: 10px; width: 100px; height: 25px; color: black");



    //slots and signals for import folder
    QObject::connect(importFolderButton, SIGNAL(clicked()), importFolderDialogBox, SLOT(exec()));
    QObject::connect(importFolderDialogBox, SIGNAL(accepted()), importFolderSuccessMessage, SLOT(exec())); // adding folder successful
    QObject::connect(importFolderDialogBox, SIGNAL(rejected()), importFolderFailureMessage, SLOT(exec())); // adding folder unsuccessful


    //tomeo logo
    QPushButton *tomeoLogo = new QPushButton();
    tomeoLogo->setIcon(QIcon(":tomeo-logo-2.png"));
    tomeoLogo->setFixedSize(240,50);
    tomeoLogo->setIconSize(QSize(210,210));
    tomeoLogo->setStyleSheet("border: none");

    //adding widgets to layout
    browseButtonLayout->addWidget(addVideoButton);
    browseButtonLayout->addWidget(importFolderButton);

    // add the video and the buttons to the top level QGridLayout
    //the way items in the GridLayout works is you designate their Row then Column, then how many Rows they span over, then how many Cols they span over
    top->addWidget(tomeoLogo, 0,0);
    top->addWidget(playerWidget, 1, 0);   //video widget is row 1 col 0
    top->setColumnStretch(0, 1);
    top->addWidget(scrollArea, 1, 1); //scollArea is row 0 col 1
    top->addWidget(browseButtonsWidget, 0,1); // browse buttons are on row 0 col 1
    top->setColumnStretch(1, 1);

    // showtime!
    window.show();

    app.installEventFilter(player);

    // wait for the app to terminate
    return app.exec();
}
