#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "ZSettingWidget.h"
#include "ZAllSettings.h"
#include "ZParamDelegate.h"

// FIXME: брать эти файлы из исходников
#include "zplay-common.h"
#include "dbus_constants.h"

QString fileOpenErrors[9] = {QObject::trUtf8("Неизвестная ошибка!"),
                             "",
                             QObject::trUtf8("Неподдерживаемый формат файла!"),
                             QObject::trUtf8("Ошибка чтения файла!"),
                             QObject::trUtf8("Не является WAV-файлом!"),
                             QObject::trUtf8("Не хватает памяти для открытия файла!"),
                             QObject::trUtf8("Ошибка при инициализации MP3!"),
                             QObject::trUtf8("Ошибка при открытии MP3-файла!"),
                             QObject::trUtf8("Неподдерживаемый формат MP3-файла!")
                            };

QString userMessages[__NUMBER_OF_ASYNC_MESSAGES] = {"",
                                                    QObject::trUtf8("Нет места на SD-карте!"),
                                                    QObject::trUtf8("Параметр будет применен к следующему старту записи/сквозного канала."),
                                                    QObject::trUtf8("Включен сквозной канал - параметр будет применен позже."),
                                                    QObject::trUtf8("SD-карта защищена от записи!"),
                                                    QObject::trUtf8("Изменился источник записи! Запись и сквозной канал остановлены!"),
                                                    "",
                                                    QObject::trUtf8("SD-карта используется."),
                                                    QObject::trUtf8("Ошибка форматирования!"),
                                                    QObject::trUtf8("Форматирование успешно завершено."),
                                                    "",
                                                    "",
                                                    QObject::trUtf8("SD-карта не готова!")
                                                   };

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
    ui->recording->setStyleSheet("background-color: red; color: white;");
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
    ui->filesView->installEventFilter(this);
    files.setSorting(QDir::Name | QDir::DirsFirst | QDir::IgnoreCase);
    files.setResolveSymlinks(false);
    files.setNameFilters(QStringList() << "*.wav");
    files.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Files);

    zdbus->startAliveTimer(10000);
    connect(zdbus, SIGNAL(gainChanged(QString, QString)), SLOT(gainChanged(QString, QString)));
    connect(zdbus, SIGNAL(paramChanged(QString, QString)), SLOT(paramChanged(QString, QString)));
    connect(zdbus, SIGNAL(recDurationChanged(int)), SLOT(recDurationChanged(int)));
    connect(zdbus, SIGNAL(playStateChanged(int)), SLOT(playStateChanged(int)));
    connect(zdbus, SIGNAL(playPositionChanged(int)), SLOT(playPositionChanged(int)));
    connect(zdbus, SIGNAL(messageForUser(unsigned int, int)), SLOT(messageForUser(unsigned int, int)));
    int val;
    zdbus->getParameter("Temp", "Recorder.State", &val);
    zdbus->getParameter("Temp", "Storage.Connected", &val);
    zdbus->getParameter("Temp", "Storage.Formatted", &val);
    zdbus->getParameter("Temp", "Storage.Capacity", &val);
    zdbus->getParameter("Temp", "Storage.Free_space", &val);

    settings.setRootNode(&settingsRoot);
    ui->settingsView->setModel(&settings);
    mixer.setRootNode(&mixerRoot);
    ui->mixerView->setModel(&mixer);
    filters.setRootNode(&filtersRoot);
    ui->filtersView->setModel(&filters);

#if defined(Q_OS_WIN)
    ZAllSettings::loadAllSettings("../res/settings-ru.xml", ui->paramPage, "settings", &settingsRoot);
    ZAllSettings::loadAllSettings("../res/settings-ru.xml", ui->paramPage, "mixer", &mixerRoot);
    ZAllSettings::loadAllSettings("../res/settings-ru.xml", ui->paramPage, "filters", &filtersRoot);
#elif defined(Q_WS_QWS)
    ZAllSettings::loadAllSettings("/etc/zface/settings-ru.xml", ui->paramPage, "settings", &settingsRoot);
    ZAllSettings::loadAllSettings("/etc/zface/settings-ru.xml", ui->paramPage, "mixer", &mixerRoot);
    ZAllSettings::loadAllSettings("/etc/zface/settings-ru.xml", ui->paramPage, "filters", &filtersRoot);
#elif defined(Q_OS_UNIX)
    ZAllSettings::loadAllSettings("settings-ru.xml", ui->paramPage, "settings", &settingsRoot);
    ZAllSettings::loadAllSettings("settings-ru.xml", ui->paramPage, "mixer", &mixerRoot);
    ZAllSettings::loadAllSettings("settings-ru.xml", ui->paramPage, "filters", &filtersRoot);
#endif

    ZParamDelegate * paramDelegate = new ZParamDelegate(this);
    ui->settingsView->installEventFilter(this);
    ui->settingsView->setItemDelegate(paramDelegate);
    ui->mixerView->installEventFilter(this);
    ui->mixerView->setItemDelegate(paramDelegate);
    ui->filtersView->installEventFilter(this);
    ui->filtersView->setItemDelegate(paramDelegate);
    ui->fileOpsList->installEventFilter(this);

    ui->gainRecLeftProgress->setFormat(tr("%v dB"));
    ui->gainRecRightProgress->setFormat(tr("%v dB"));
    ui->gainPlayProgress->setFormat(tr("%v dB"));

    watcher = NULL;
    //SetWatcher(ui->filesView->currentIndex());
    //connect(ui->filesView, SIGNAL(entered(QModelIndex))
    QTimer * timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(updateTime()));
    timer->start(1000);
    updateTime();

    gainTimer = new QTimer(this);
    connect(gainTimer, SIGNAL(timeout()), SLOT(hideGain()));

    message = 0;
    messageTimer.setSingleShot(true);
    messageTimer.setInterval(3000);
    connect(&messageTimer, SIGNAL(timeout()), SLOT(removeMessageBox()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateTime()
{
    ui->clockLabel->setText(QTime::currentTime().toString("hh:mm"));
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
            processSettingsPage(event, ui->filtersView);
            break;
        case 5 :
            processParameterPage(event);
            break;
        case 6 :
            processPlayPage(event);
            break;
    }
    //QMainWindow::keyPressEvent(event);
}

void MainWindow::SetWatcher(QModelIndex root)
{
    if (watcher)
        delete watcher;

    watcher = new QFileSystemWatcher(this);
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
        case Qt::Key_Down :
            {
                ui->filesView->setRootIndex(files.index("/tmp/sound"));
                rootIndex = ui->filesView->rootIndex();
                ui->filesView->setCurrentIndex(files.index(0, 0, rootIndex));
                ui->pages->setCurrentWidget(ui->browserPage);
                ui->filesView->setEditFocus(true);
            }
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
            // Идем наверх
            while (ui->filtersView->rootIndex().isValid())
                ui->filtersView->setRootIndex(ui->filtersView->rootIndex().parent());
            ui->pages->setCurrentWidget(ui->filtersPage);
            ui->filtersView->setEditFocus(true);
            // Чтобы элемент сразу выделился
            ui->filtersView->setCurrentIndex(filters.index(0, 0, ui->filtersView->rootIndex()));
            currentView = ui->filtersView;
            currentPage = ui->filtersPage;
            break;
    }
}

void MainWindow::processBrowserPage(QKeyEvent * event)
{
    switch (event->key())
    {
        case Qt::Key_Select :
            if (files.fileInfo(ui->filesView->currentIndex()).isDir())
            {
                SetWatcher(ui->filesView->currentIndex());
                ui->filesView->setRootIndex(ui->filesView->currentIndex());
                ui->filesView->setCurrentIndex(ui->filesView->rootIndex().child(0, 0));
                files.refresh(ui->filesView->rootIndex());
                ui->filesView->setEditFocus(true);
            }
            else
            {
                if (!zdbus->sendOpenFileRequest(files.fileInfo(ui->filesView->currentIndex()).absoluteFilePath(), &currentFileInfo))
                    showMessage(QMessageBox::Warning, fileOpenErrors[0]);
                else
                {
                    if (currentFileInfo.openStatus != XPLAY_OPEN_OK)
                        showMessage(QMessageBox::Warning, fileOpenErrors[currentFileInfo.openStatus]);
                    else
                    {
                        qDebug() << currentFileInfo.sampleSize << " " << currentFileInfo.sampleRate << " " << currentFileInfo.duration;
                        ui->fileNameLabel->setText(files.fileName(ui->filesView->currentIndex()));
                        QString info = QString("%1").arg(currentFileInfo.sampleSize) + " " + trUtf8("бит") + ", " +
                                       QString("%1").arg(currentFileInfo.sampleRate / 1000) + " " + trUtf8("кГц") + ", ";
                        if (currentFileInfo.channels == 1)
                            info += trUtf8("моно");
                        else if (currentFileInfo.channels == 2)
                            info += trUtf8("стерео");
                        ui->fileInfoLabel->setText(info);
                        ui->playProgress->setValue(0);
                        ui->playProgressLabel->setText(QString("0:00:00 / %1:%2:%3")
                                                        .arg(currentFileInfo.duration / 3600)
                                                        .arg(currentFileInfo.duration % 3600 / 60, 2, 10, QChar('0'))
                                                        .arg(currentFileInfo.duration % 60, 2, 10, QChar('0')));
                        ui->pages->setCurrentWidget(ui->playPage);
                        ui->fileOpsList->setEditFocus(true);
                        ui->fileOpsList->setCurrentRow(0);
                        currentPlayState = PlayStopped;
                    }
                }
            }
            break;
        case Qt::Key_Escape :
            if (files.parent(ui->filesView->rootIndex()).isValid() && ui->filesView->rootIndex() != rootIndex)
            {
                ui->filesView->setCurrentIndex(ui->filesView->rootIndex());
                ui->filesView->setRootIndex(files.parent(ui->filesView->rootIndex()));
                SetWatcher(ui->filesView->rootIndex());
                files.refresh(ui->filesView->rootIndex());
                ui->filesView->setEditFocus(true);
            }
            else
                ui->pages->setCurrentWidget(ui->mainPage);
            break;
    }
}

void MainWindow::processSettingsPage(QKeyEvent * event, QListView * view)
{
    switch (event->key())
    {
//        case Qt::Key_Escape :
//            ui->pages->setCurrentWidget(ui->mainPage);
//            break;
//        case Qt::Key_Right :
        case Qt::Key_Select:
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
        case Qt::Key_Escape :
            QModelIndex ind = view->rootIndex();
            if (ind.isValid())
            {
                view->setRootIndex(ind.parent());
                view->setCurrentIndex(ind);
            }
            else
                ui->pages->setCurrentWidget(ui->mainPage);
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

void MainWindow::processPlayPage(QKeyEvent * event)
{
    switch (event->key())
    {
        case Qt::Key_Escape :
            zdbus->sendPlayEvent("StopPlay");
            ui->pages->setCurrentWidget(ui->browserPage);
            ui->filesView->setEditFocus(true);
            break;
        case Qt::Key_Select :
            processFileOps();
            break;
    }
}

void MainWindow::processFileOps()
{
    switch (ui->fileOpsList->currentRow())
    {
        case 0 :
        {
            switch (currentPlayState)
            {
                case PlayStopped :
                case PlayPaused :
                    zdbus->sendPlayEvent("StartPlay");
                    break;
                case PlayPlaying :
                    zdbus->sendPlayEvent("PausePlay");
                    break;
            }
            break;
        }
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
    else if (_gain.contains("Phone.Gain"))
    {
        ui->gainRecLeftProgress->setValue(_value.toInt());
        ui->gainRecRightProgress->setValue(_value.toInt());
    }
    else if (_gain.contains(".Gain"))
        ui->gainPlayProgress->setValue(_value.toInt());
}

void MainWindow::paramChanged(QString _param, QString _value)
{
    qDebug() << "paramChanged " << _param << ": " << _value;
    int value = _value.toInt();

    if (_param == "Mixer.Input")
    {
        int gain;
        switch (value)
        {
            case 0 :
                ui->gainRecIcon->setStyleSheet("background-image: url(:/all/res/gain_rec_mic.bmp);"
                                               "background-repeat: repeat-n;"
                                               "background-position: center;");
                if (zdbus->getParameter("Main", "Mixer.Input.Mic.Left_gain", &gain))
                    ui->gainRecLeftProgress->setValue(gain);
                if (zdbus->getParameter("Main", "Mixer.Input.Mic.Right_gain", &gain))
                    ui->gainRecRightProgress->setValue(gain);
                break;
            case 1 :
                ui->gainRecIcon->setStyleSheet("background-image: url(:/all/res/gain_rec_line.bmp);"
                                               "background-repeat: repeat-n;"
                                               "background-position: center;");
                if (zdbus->getParameter("Main", "Mixer.Input.Analog.Left_gain", &gain))
                    ui->gainRecLeftProgress->setValue(gain);
                if (zdbus->getParameter("Main", "Mixer.Input.Analog.Right_gain", &gain))
                    ui->gainRecRightProgress->setValue(gain);
                break;
            case 2 :
                ui->gainRecIcon->setStyleSheet("background-image: url(:/all/res/gain_rec_phone.bmp);"
                                               "background-repeat: repeat-n;"
                                               "background-position: center;");
                if (zdbus->getParameter("Main", "Mixer.Input.Phone.Gain", &gain))
                {
                    ui->gainRecLeftProgress->setValue(gain);
                    ui->gainRecRightProgress->setValue(gain);
                }
                break;
            case 3 :
                ui->gainRecIcon->setStyleSheet("background-image: url(:/all/res/gain_rec_dig.bmp);"
                                               "background-repeat: repeat-n;"
                                               "background-position: center;");
                if (zdbus->getParameter("Main", "Mixer.Input.DigitalIn.Left_gain", &gain))
                    ui->gainRecLeftProgress->setValue(gain);
                if (zdbus->getParameter("Main", "Mixer.Input.DigitalIn.Right_gain", &gain))
                    ui->gainRecRightProgress->setValue(gain);
                break;
        }
    }

    else if (_param == "Mixer.Output")
    {
        int gain;
        switch (value)
        {
            case 0 :
                ui->gainPlayIcon->setStyleSheet("background-image: url(:/all/res/gain_play_line.bmp);"
                                                "background-repeat: repeat-n;"
                                                "background-position: center;");
                if (zdbus->getParameter("Main", "Mixer.Output.LineOut.Gain", &gain))
                    ui->gainPlayProgress->setValue(gain);
                break;
            case 1 :
                ui->gainPlayIcon->setStyleSheet("background-image: url(:/all/res/gain_play_spk.bmp);"
                                                "background-repeat: repeat-n;"
                                                "background-position: center;");
                if (zdbus->getParameter("Main", "Mixer.Output.Speaker.Gain", &gain))
                    ui->gainPlayProgress->setValue(gain);
                break;
            case 2 :
                ui->gainPlayIcon->setStyleSheet("background-image: url(:/all/res/gain_play_dig.bmp);"
                                                "background-repeat: repeat-n;"
                                                "background-position: center;");
                if (zdbus->getParameter("Main", "Mixer.Output.DigitalOut.Gain", &gain))
                    ui->gainPlayProgress->setValue(gain);
                break;
        }
    }

    else if (_param == "Mixer.Through_channel")
    {
    }

    else if (_param == "Recorder.State")
    {
        switch (value)
        {
            case RecStopped :
            case RecStoppedForced :
                ui->recording->hide();
                ui->recordingWidget->setStyleSheet("background-image: none");
                break;
            case RecStarted :
            case RecStartedAccu :
                ui->recordingDurationLabel->setText("0:00:00");
                ui->recording->show();
                ui->recordingWidget->setStyleSheet("background-image: url(:/all/res/record_16x16.png);"
                                                   "background-repeat: repeat-n;");
                break;
        }
    }

    else if (_param == "Storage.Connected")
    {
        if (!value)
        {
            currentSDState = NotConnected;
            ui->cardErrorLabel->setText(trUtf8("Вставьте\nSD-карту!"));
            ui->cardInfoWidget->hide();
            ui->cardErrorLabel->show();
        }
        else if (currentSDState != NotFormatted)
        {
            ui->cardErrorLabel->hide();
            ui->cardInfoWidget->show();
            currentSDState = SDOK;
        }
    }

    else if (_param == "Storage.Formatted")
    {
        if (!value && currentSDState != NotConnected)
        {
            currentSDState = NotFormatted;
            ui->cardErrorLabel->setText(trUtf8("SD-карта не\nотформатирована!"));
            ui->cardInfoWidget->hide();
            ui->cardErrorLabel->show();
        }
        else if (value && currentSDState != NotConnected)
        {
            ui->cardErrorLabel->hide();
            ui->cardInfoWidget->show();
            currentSDState = SDOK;
        }
    }

    else if (_param == "Storage.Free_space")
    {
        float val = value;
        if (value >= 1000)
        {
            val /= 1000.;
            ui->measureFree->setText(trUtf8("Гб"));
        }
        else
            ui->measureFree->setText(trUtf8("Мб"));
        ui->valueFree->setText(QString("%1").arg(val, 0, 'f', 1));
    }
    else if (_param == "Storage.Capacity")
    {
        float val = value;
        if (value >= 1000)
        {
            val /= 1000.;
            ui->measureTotal->setText(trUtf8("Гб"));
        }
        else
            ui->measureTotal->setText(trUtf8("Мб"));
        ui->valueTotal->setText(QString("%1").arg(val, 0, 'f', 1));
    }

    else if (_param == "Recorder.Sample_size")
    {
        ui->sampleSizeLabel->setText(settings.valueByName(_param));
    }
    else if (_param == "Recorder.Compression")
    {
        ui->compressionLabel->setText(settings.valueByName(_param));
    }
    else if (_param == "Recorder.Sample_rate")
    {
        ui->sampleRateLabel->setText(settings.valueByName(_param));
    }
    else if (_param == "Recorder.Channels")
    {
        ui->channelsLabel->setText(settings.valueByName(_param));
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
        if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Select)
        {
            QCoreApplication::sendEvent(this, event);
            return true;
        }
    return false;
}

void MainWindow::recDurationChanged(int _duration)
{
    ui->recordingDurationLabel->setText(
            QString("%1:%2:%3").arg(_duration / 3600).arg(_duration % 3600 / 60, 2, 10, QChar('0')).arg(_duration % 60, 2, 10, QChar('0')));
}

void MainWindow::playStateChanged(int _state)
{
    switch (_state)
    {
        case PlayPaused :
        case PlayStopped :
            ui->fileOpsList->item(0)->setText(trUtf8("Воспроизвести"));
            break;
        case PlayPlaying :
            ui->fileOpsList->item(0)->setText(trUtf8("Остановить воспр-е"));
            break;
    }
    currentPlayState = static_cast<PlayState>(_state);
}

void MainWindow::playPositionChanged(int _position)
{
    ui->playProgressLabel->setText(QString("%1:%2:%3 / %4:%5:%6")
                                    .arg(_position / 3600)
                                    .arg(_position % 3600 / 60, 2, 10, QChar('0'))
                                    .arg(_position % 60, 2, 10, QChar('0'))
                                    .arg(currentFileInfo.duration / 3600)
                                    .arg(currentFileInfo.duration % 3600 / 60, 2, 10, QChar('0'))
                                    .arg(currentFileInfo.duration % 60, 2, 10, QChar('0')));
    if (currentFileInfo.duration)
        ui->playProgress->setValue(_position * ui->playProgress->maximum() / currentFileInfo.duration);
}

void MainWindow::messageForUser(unsigned int _code, int _type)
{
    QString messageText = userMessages[_code];
    if (messageText.isEmpty())
        messageText = trUtf8("Неизвестный номер сообщения: ") + QString("%1").arg(_code);
    if (_type == 0)
        showMessage(QMessageBox::Warning, messageText);
    else if (_type == 1)
        showMessage(QMessageBox::Critical, messageText);
}

void MainWindow::showMessage(QMessageBox::Icon _type, const QString & _message)
{
    if (message)
        delete message;
    else
        focusedWidget = QApplication::focusWidget();
    message = new QMessageBox(this);
    message->setText(_message);
    message->setStandardButtons(QMessageBox::Ok);
    message->setIcon(_type);
    messageTimer.start();
    message->exec();
    if (focusedWidget)
        focusedWidget->setEditFocus(true);
}

void MainWindow::removeMessageBox()
{
    if (message)
    {
        delete message;
        message = 0;
        if (focusedWidget)
            focusedWidget->setEditFocus(true);
    }
}
