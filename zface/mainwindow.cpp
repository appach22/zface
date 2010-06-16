#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "ZSettingWidget.h"
#include "ZAllSettings.h"

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
    QFile file("res/style.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    this->setStyleSheet(styleSheet);

    ui->filesView->setModel(&files);
    files.setSorting(QDir::Name | QDir::DirsFirst | QDir::IgnoreCase);

    ZSettingsNode * settingsRoot;
    ZAllSettings::loadAllSettings("settings-en.xml", ui->paramPage, &settingsRoot);
    settings.setRootNode(settingsRoot);
    ui->settingsView->setModel(&settings);

    watcher = NULL;
    playGain = 20;
    ui->gainPlayProgress->setValue(playGain);
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

void MainWindow::paintEvent(QPaintEvent *event)
{
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
    qDebug() << "Key!!!";
    if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Minus)
    {
        processEncoders(event);
        return;
    }

    switch(ui->pages->currentIndex())
    {
        case 0 :
            processMainPage(event);
            break;
        case 1 :
            processBrowserPage(event);
            break;
        case 2 :
            processSettingsPage(event);
            break;
        case 3 :
            processParameterPage(event);
            break;
    }
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
            ui->filesView->setFocus();
            break;
        case Qt::Key_Return :
            break;
        case Qt::Key_Right :
            // Идем наверх
            while (ui->settingsView->rootIndex().isValid())
                ui->settingsView->setRootIndex(ui->settingsView->rootIndex().parent());
            ui->pages->setCurrentWidget(ui->settingsPage);
            ui->settingsView->setFocus();
            // Чтобы элемент сразу выделился
            ui->settingsView->setCurrentIndex(settings.index(0, 0, ui->settingsView->rootIndex()));
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
                ui->filesView->setFocus();
            }
            break;
        case Qt::Key_Left :
            if (files.parent(ui->filesView->rootIndex()).isValid())
            {
                ui->filesView->setRootIndex(files.parent(ui->filesView->rootIndex()));
                SetWatcher(ui->filesView->rootIndex());
                files.refresh(ui->filesView->rootIndex());
                ui->filesView->setFocus();
            }
            break;
    }
}

void MainWindow::processSettingsPage(QKeyEvent * event)
{
    switch (event->key())
    {
        case Qt::Key_Escape :
            ui->pages->setCurrentWidget(ui->mainPage);
            break;
        case Qt::Key_Right :
            if (ui->settingsView->currentIndex().isValid())
            {
                ZSettingsNode * node = static_cast<ZSettingsNode *>(ui->settingsView->currentIndex().internalPointer());
                if (node->type == ZSettingsNode::Node)
                {
                    QModelIndex ind = ui->settingsView->currentIndex().child(0,0);
                    if (ind.isValid())
                    {
                        ui->settingsView->setRootIndex(ui->settingsView->currentIndex());
                        ui->settingsView->setCurrentIndex(ind);
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
            QModelIndex ind = ui->settingsView->rootIndex();
            if (ind.isValid())
            {
                ui->settingsView->setRootIndex(ind.parent());
                ui->settingsView->setCurrentIndex(ind);
            }
            break;
    }
}

void MainWindow::processParameterPage(QKeyEvent * event)
{
    switch (event->key())
    {
        case Qt::Key_Escape :
        case Qt::Key_Return :
            ui->pages->setCurrentWidget(ui->settingsPage);
            break;
    }
}

void MainWindow::processEncoders(QKeyEvent * event)
{
    switch (event->key())
    {
        case Qt::Key_Plus :
            if (ui->statusPages->currentIndex() != 1)
                ui->statusPages->setCurrentWidget(ui->gainPlayStatus);
            if (playGain < 100)
            {
                ui->gainPlayProgress->setValue(++playGain);
                ui->gainPlayValue->setText(QString::number(playGain));
            }
            gainTimer->start(1000);
            break;
        case Qt::Key_Minus :
            if (ui->statusPages->currentIndex() != 1)
                ui->statusPages->setCurrentWidget(ui->gainPlayStatus);
            if (playGain > 0)
            {
                ui->gainPlayProgress->setValue(--playGain);
                ui->gainPlayValue->setText(QString::number(playGain));
            }
            gainTimer->start(1000);
            break;
    }
}
