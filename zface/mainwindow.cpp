#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "ZSettingWidget.h"
#include "ZAllSettings.h"
#include "ZDbus.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent, Qt::FramelessWindowHint), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    img = QImage(":/new/prefix1/res/xface_state_2.bmp");
    ui->pages->setCurrentWidget(ui->mainPage);
    //this->setStyleSheet("background-color: white;");
    ui->recordingWidget->setStyleSheet("background-image: url(:/all/res/record_16x16.png);"
                                       "background-repeat: repeat-n;");
    ui->mainPage->setStyleSheet("background-image: url(:/all/res/xface_mainmenu.bmp);"
                                     "background-repeat: repeat-n;"
                                     "background-position: center;");
    //ui->statusPages->setStyleSheet("background-color: white;");
    ui->sd->setStyleSheet("background-image: url(:/all/res/xface_storage.bmp);"
                          "background-repeat: repeat-n;"
                          "background-position: center;");
    ui->gainPlayIcon->setStyleSheet("background-image: url(:/all/res/gain_play_spk.bmp);"
                                    "background-repeat: repeat-n;"
                                    "background-position: center;");
#if defined(Q_OS_WIN)
    QFile file("../res/style.qss");
#elif defined(Q_WS_QWS)
    QFile file("/etc/zface/style.qss");
#elif defined(Q_OS_UNIX)
    QFile file("res/style.qss");
#endif
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    this->setStyleSheet(styleSheet);

    ui->filesView->setModel(&files);
    files.setSorting(QDir::Name | QDir::DirsFirst | QDir::IgnoreCase);

    zdbus->startAliveTimer(10000);
    connect(zdbus, SIGNAL(gainChanged(QString, QString)), SLOT(gainChanged(QString, QString)));
    connect(zdbus, SIGNAL(paramChanged(QString, QString)), SLOT(paramChanged(QString, QString)));

    ZSettingsNode * settingsRoot;
    ZSettingsNode * mixerRoot;
#if defined(Q_OS_WIN)
    ZAllSettings::loadAllSettings("../res/settings-en.xml", ui->paramPage, "settings", &settingsRoot);
    ZAllSettings::loadAllSettings("../res/settings-en.xml", ui->paramPage, "mixer", &mixerRoot);
#elif defined(Q_WS_QWS)
    ZAllSettings::loadAllSettings("/etc/zface/settings-en.xml", ui->paramPage, "settings", &settingsRoot);
    ZAllSettings::loadAllSettings("/etc/zface/settings-en.xml", ui->paramPage, "mixer", &mixerRoot);
#elif defined(Q_OS_UNIX)
    ZAllSettings::loadAllSettings("settings-en.xml", ui->paramPage, "settings", &settingsRoot);
    ZAllSettings::loadAllSettings("settings-en.xml", ui->paramPage, "mixer", &mixerRoot);
#endif
    settings.setRootNode(settingsRoot);
    ui->settingsView->setModel(&settings);
    mixer.setRootNode(mixerRoot);
    ui->mixerView->setModel(&mixer);

    ui->gainRecLeftProgress->setFormat(tr("%v dB"));
    ui->gainRecRightProgress->setFormat(tr("%v dB"));
    ui->gainPlayProgress->setFormat(tr("%v dB"));

    watcher = NULL;
    //SetWatcher(ui->filesView->currentIndex());
    //connect(ui->filesView, SIGNAL(entered(QModelIndex))
    QTimer * timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(updateTime()));
    timer->start(500);
    updateTime();

    gainTimer = new QTimer(this);
    connect(gainTimer, SIGNAL(timeout()), SLOT(hideGain()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateTime()
{
    ui->clockLabel->setText(QTime::currentTime().toString("mm:ss"));
}

void MainWindow::hideGain()
{
    ui->statusPages->setCurrentWidget(ui->mainStatus);
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
    //qDebug() << "Key!!!";
    if (processEncoders(event))
        return;

    switch(ui->pages->currentIndex())
    {
        case 0 :
            processMainPage(event);
            break;
        case 1 :
            processBrowserPage(event);
            break;
        case 2 :
            processSettingsPage(event, ui->settingsView);
            break;
        case 3 :
            processSettingsPage(event, ui->mixerView);
            break;
        case 4 :
            processParameterPage(event);
            break;
    }
    //QMainWindow::keyPressEvent(event);
}

void MainWindow::SetWatcher(QModelIndex root)
{
    if (watcher)
        delete watcher;

    watcher = new QFileSystemWatcher(this);
    qDebug() << "Watching " << files.fileInfo(root).absoluteFilePath();
    watcher->addPath(files.fileInfo(root).absoluteFilePath());
    connect(watcher, SIGNAL(directoryChanged(const QString &)), SLOT(refreshPath()));
}

void MainWindow::refreshPath()
{
    qDebug() << "Changed!";
    files.refresh(files.parent(ui->filesView->rootIndex()));
}


//-------------------------------------------------------------------------------------------

void MainWindow::processMainPage(QKeyEvent * event)
{
    switch (event->key())
    {
        case Qt::Key_Escape :
            ui->pages->setCurrentWidget(ui->mainPage);
            break;
        case Qt::Key_Down   :
            ui->pages->setCurrentWidget(ui->browserPage);
            ui->filesView->setEditFocus(true);
            break;
        case Qt::Key_Select :
            // Идем наверх
            while (ui->mixerView->rootIndex().isValid())
                ui->mixerView->setRootIndex(ui->mixerView->rootIndex().parent());
            ui->pages->setCurrentWidget(ui->mixerPage);
            ui->mixerView->setEditFocus(true);
            // Чтобы элемент сразу выделился
            ui->mixerView->setCurrentIndex(mixer.index(0, 0, ui->mixerView->rootIndex()));
            currentView = ui->mixerView;
            currentPage = ui->mixerPage;
            break;
        case Qt::Key_Right :
            // Идем наверх
            while (ui->settingsView->rootIndex().isValid())
                ui->settingsView->setRootIndex(ui->settingsView->rootIndex().parent());
            ui->pages->setCurrentWidget(ui->settingsPage);
            ui->settingsView->setEditFocus(true);
            // Чтобы элемент сразу выделился
            ui->settingsView->setCurrentIndex(settings.index(0, 0, ui->settingsView->rootIndex()));
            currentView = ui->settingsView;
            currentPage = ui->settingsPage;
            break;
        case Qt::Key_Left :
            break;
    }
}

void MainWindow::processBrowserPage(QKeyEvent * event)
{
    switch (event->key())
    {
        case Qt::Key_Escape :
            ui->pages->setCurrentWidget(ui->mainPage);
            break;
        case Qt::Key_Right :
            if (files.fileInfo(ui->filesView->currentIndex()).isDir())
            {
                SetWatcher(ui->filesView->currentIndex());
                ui->filesView->setRootIndex(ui->filesView->currentIndex());
                files.refresh(files.parent(ui->filesView->currentIndex()));
                ui->filesView->setEditFocus(true);
            }
            break;
        case Qt::Key_Left :
            if (files.parent(ui->filesView->rootIndex()).isValid())
            {
                ui->filesView->setRootIndex(files.parent(ui->filesView->rootIndex()));
                SetWatcher(ui->filesView->rootIndex());
                files.refresh(ui->filesView->rootIndex());
                ui->filesView->setEditFocus(true);
            }
            break;
    }
}

void MainWindow::processSettingsPage(QKeyEvent * event, QListView * view)
{
    switch (event->key())
    {
        case Qt::Key_Escape :
            ui->pages->setCurrentWidget(ui->mainPage);
            break;
        case Qt::Key_Right :
            if (view->currentIndex().isValid())
            {
                ZSettingsNode * node = static_cast<ZSettingsNode *>(view->currentIndex().internalPointer());
                if (node->type == ZSettingsNode::Node)
                {
                    QModelIndex ind = view->currentIndex().child(0,0);
                    if (ind.isValid())
                    {
                        view->setRootIndex(view->currentIndex());
                        view->setCurrentIndex(ind);
                    }
                }
                else
                {
                    if (ui->paramLayout->count())
                    {
                        ui->paramLayout->itemAt(0)->widget()->hide();
                        ui->paramLayout->removeItem(ui->paramLayout->itemAt(0));
                    }
                    ui->paramLayout->addWidget(node->widget, 0, 0, 1, 1);
                    ui->pages->setCurrentWidget(ui->paramPage);
                    node->widget->show();
                }
            }
            break;
        case Qt::Key_Left :
            QModelIndex ind = view->rootIndex();
            if (ind.isValid())
            {
                view->setRootIndex(ind.parent());
                view->setCurrentIndex(ind);
            }
            break;
    }
}

void MainWindow::processParameterPage(QKeyEvent * event)
{
    switch (event->key())
    {
        case Qt::Key_Escape :
        case Qt::Key_Select :
            ui->pages->setCurrentWidget(currentPage);
            currentView->setEditFocus(true);
            break;
    }
}

bool MainWindow::processEncoders(QKeyEvent * event)
{
    switch (event->key())
    {
        case Qt::Key_E :
            if (ui->statusPages->currentIndex() != 1)
                ui->statusPages->setCurrentWidget(ui->gainPlayStatus);
            zdbus->sendRotaryEvent("PlayEncoder", "+");
            break;
        case Qt::Key_C :
            if (ui->statusPages->currentIndex() != 1)
                ui->statusPages->setCurrentWidget(ui->gainPlayStatus);
            zdbus->sendRotaryEvent("PlayEncoder", "-");
            break;
        case Qt::Key_Q :
            if (ui->statusPages->currentIndex() != 2)
                ui->statusPages->setCurrentWidget(ui->gainRecStatus);
            zdbus->sendRotaryEvent("LeftRecEncoder", "+");
            break;
        case Qt::Key_Z :
            if (ui->statusPages->currentIndex() != 2)
                ui->statusPages->setCurrentWidget(ui->gainRecStatus);
            zdbus->sendRotaryEvent("LeftRecEncoder", "-");
            break;
        case Qt::Key_W :
            if (ui->statusPages->currentIndex() != 2)
                ui->statusPages->setCurrentWidget(ui->gainRecStatus);
            zdbus->sendRotaryEvent("RightRecEncoder", "+");
            break;
        case Qt::Key_X :
            if (ui->statusPages->currentIndex() != 2)
                ui->statusPages->setCurrentWidget(ui->gainRecStatus);
            zdbus->sendRotaryEvent("RightRecEncoder", "-");
            break;
        default:
            return false;
    }

    gainTimer->start(1000);
    return true;
}

void MainWindow::gainChanged(QString _gain, QString _value)
{
    if (_gain.contains("Left_gain"))
        ui->gainRecLeftProgress->setValue(_value.toInt());
    else if (_gain.contains("Right_gain"))
        ui->gainRecRightProgress->setValue(_value.toInt());
    else if (_gain.contains(".Gain"))
        ui->gainPlayProgress->setValue(_value.toInt());
}

void MainWindow::paramChanged(QString _param, QString _value)
{
    qDebug() << "paramChanged " << _param << ": " << _value;
    //if (_param == "")
}
