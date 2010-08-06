#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "ZSettingWidget.h"
#include "ZAllSettings.h"
#include "ZParamDelegate.h"

// FIXME: брать эти файлы из исходников
#include "zplay-common.h"
#include "dbus_constants.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/input.h>


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

static int keyboardAutolockTimes[4] = {0, 5000, 60000, 120000};
static int backlightTimes[4] = {0, 15000, 30000, 60000};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent, Qt::FramelessWindowHint), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pages->setCurrentWidget(ui->mainPage);
//    ui->mainPage->setStyleSheet("background-image: url(:/all/res/xface_mainmenu.bmp);"
//                                     "background-repeat: repeat-n;"
//                                     "background-position: center;");
    //ui->statusPages->setStyleSheet("background-color: white;");
    ui->sd->setStyleSheet("background-image: url(:/all/res/storage.png);"
                          "background-repeat: repeat-n;"
                          "background-position: center;");
    ui->gainPlayIcon->setStyleSheet("background-image: url(:/all/res/gain_play_spk.bmp);"
                                    "background-repeat: repeat-n;"
                                    "background-position: center;");
    ui->recording->hide();
//    ui->recording->setStyleSheet("background-color: qlineargradient(spread:pad, x1:1, y1:0.573864, x2:1, y2:0, stop:0 rgba(255, 80, 80, 255), stop:1 rgba(255, 255, 255, 255)); color: white;");

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

    keyboardLockTimer.setSingleShot(true);
    connect(&keyboardLockTimer, SIGNAL(timeout()), SLOT(lockKeyboard()));
    keyboardLocked = false;
    beforeLockWidget = 0;

    backlightTimer.setSingleShot(true);
    connect(&backlightTimer, SIGNAL(timeout()), SLOT(backlightTurnOff()));
    backlightIsOff = false;
    fdFrameBuffer = open("/dev/fb0", O_RDWR);

    ui->digit1->installEventFilter(this);
    ui->digit2->installEventFilter(this);
    ui->digit3->installEventFilter(this);
    ui->digit4->installEventFilter(this);

    ui->filesView->setModel(&files);
    watcher = NULL;
    ui->filesView->installEventFilter(this);
    files.setSorting(QDir::Name | QDir::DirsFirst | QDir::IgnoreCase | QDir::Reversed);
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
    zdbus->getParameter("Main", "Display.Backlight.Auto_turn_off", &val);
    zdbus->getParameter("Main", "Security.Keyboard_lock.Auto_lock", &val);
    zdbus->getParameter("Main", "Security.Protection.Enabled", &val);
    zdbus->getParameter("Temp", "Security.Keyboard_lock.Active", &val);

    // Начальное положение джека
    uint8_t sw_b[SW_MAX / 8 + 1];
    int sw = 0;
    memset(sw_b, 0, sizeof(sw_b));
    int fd = open("/dev/input/hp-detect", O_RDONLY, 0);
    if (-1 == fd)
        qDebug() << "Unable to open /dev/input/hp-detect!";
    ioctl(fd, EVIOCGSW(sizeof(sw_b)), sw_b);
    for(int i = 0; i < SW_MAX; i++)
    {
        if((sw_b[i / 32] >> i) & 1)
            sw = i;
    }
    if (2 == sw)
        zdbus->setParameter("Temp", "Mixer.Headset.Connected", 1);
    else
        zdbus->setParameter("Temp", "Mixer.Headset.Connected", 0);


    settingsRoot = mixerRoot = filtersRoot = 0;
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
    //ui->settingsView->setAlternatingRowColors(true);
    ui->mixerView->installEventFilter(this);
    ui->mixerView->setItemDelegate(paramDelegate);
    //ui->mixerView->setAlternatingRowColors(true);
    ui->filtersView->installEventFilter(this);
    ui->filtersView->setItemDelegate(paramDelegate);
    //ui->filtersView->setAlternatingRowColors(true);
    ui->fileOpsList->installEventFilter(this);
    ui->fileOpsList->setItemDelegate(paramDelegate);
    //ui->fileOpsList->setAlternatingRowColors(true);
    ui->filesView->setItemDelegate(paramDelegate);
    //ui->filesView->setAlternatingRowColors(true);
    ui->utilitiesList->installEventFilter(this);
    ui->utilitiesList->setItemDelegate(paramDelegate);
    //ui->utilitiesList->setAlternatingRowColors(true);
    ui->filtersAndPresetsList->installEventFilter(this);
    ui->filtersAndPresetsList->setItemDelegate(paramDelegate);
    //ui->filtersAndPresetsList->setAlternatingRowColors(true);
    ui->presetsList->installEventFilter(this);
    ui->presetsList->setItemDelegate(paramDelegate);
    //ui->presetsList->setAlternatingRowColors(true);
    ui->presetOpsList->installEventFilter(this);
    ui->presetOpsList->setItemDelegate(paramDelegate);
    //ui->presetOpsList->setAlternatingRowColors(true);

    ui->fileOpsList->addItem(trUtf8("Воспроизвести"));
    ui->fileOpsList->addItem(trUtf8("Очистить и сохранить"));
    ui->fileOpsList->addItem(trUtf8("Удалить в корзину"));
    // Это хак! Без него элементы рисуются неправильно
    ui->utilitiesList->item(0)->setSizeHint(QSize(121, 15));
    ui->filtersAndPresetsList->item(0)->setSizeHint(QSize(121, 15));

    ui->gainRecLeftProgress->setFormat(tr("%v dB"));
    ui->gainRecRightProgress->setFormat(tr("%v dB"));
    ui->gainPlayProgress->setFormat(tr("%v dB"));

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

    confirmMessage = new QMessageBox(this);
    confirmMessage->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmMessage->setDefaultButton(QMessageBox::No);
    }

MainWindow::~MainWindow()
{
    ::close(fdFrameBuffer);
    delete ui;
}

void MainWindow::updateTime()
{
    if (++clockCounter % 2)
        ui->clockLabel->setText(QTime::currentTime().toString("hh:mm"));
    else
        ui->clockLabel->setText(QTime::currentTime().toString("hh mm"));
}

void MainWindow::hideGain()
{
    ui->statusPages->setCurrentWidget(ui->mainStatus);
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
    if (processEncoders(event))
        return;

    if (event->key() == Qt::Key_F5)
    {
        zdbus->sendRecordKey();
        return;
    }
    if (event->key() == Qt::Key_F11)
    {
        zdbus->setParameter("Temp", "Mixer.Headset.Connected", 0);
        return;
    }
    if (event->key() == Qt::Key_F12)
    {
        zdbus->setParameter("Temp", "Mixer.Headset.Connected", 1);
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
        case 7 :
            processUtilitiesPage(event);
            break;
        case 8 :
            processLogsBrowserPage(event);
            break;
        case 9 :
            processLockedPages(event);
            break;
        case 10 :
            processLockedPages(event);
            break;
        case 11 :
            processPinCodePage(event);
            break;
        case 12 :
            processFiltersAndPresetsPage(event);
            break;
        case 13 :
            processPresetsPage(event);
            break;
        case 14 :
            processPresetOpsPage(event);
            break;
        case 15 :
            processPresetNamePage(event);
            break;
        default:
            QMainWindow::keyPressEvent(event);
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
    rootIndex = files.index("/tmp/sound");
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
        case Qt::Key_Up :
            {
                ui->pages->setCurrentWidget(ui->utilitiesPage);
                ui->utilitiesList->setCurrentRow(0);
                ui->utilitiesList->setEditFocus(true);
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
            ui->pages->setCurrentWidget(ui->filtersAndPresetsPage);
            ui->filtersAndPresetsList->setCurrentRow(0);
            ui->filtersAndPresetsList->setEditFocus(true);
            break;
    }
}

void MainWindow::processBrowserPage(QKeyEvent * event)
{
    switch (event->key())
    {
        case Qt::Key_Select :
            if (!ui->filesView->currentIndex().isValid())
                break;
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
            if (ui->filesView->rootIndex().parent().isValid() && ui->filesView->rootIndex() != rootIndex)
            {
                ui->filesView->setCurrentIndex(ui->filesView->rootIndex());
                ui->filesView->setRootIndex(ui->filesView->rootIndex().parent());
                SetWatcher(ui->filesView->rootIndex());
                files.refresh(ui->filesView->rootIndex());
                ui->filesView->setEditFocus(true);
            }
            else
                ui->pages->setCurrentWidget(ui->mainPage);
            break;
        case Qt::Key_Left :
            ui->filesView->setCurrentIndex(ui->filesView->currentIndex().sibling(0, 0));
            break;
        case Qt::Key_Right :
            {
                int count = ui->filesView->currentIndex().model()->rowCount(ui->filesView->currentIndex().parent());
                ui->filesView->setCurrentIndex(ui->filesView->currentIndex().sibling(count - 1, 0));
            }
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
            {
                QModelIndex ind = view->rootIndex();
                if (ind.isValid())
                {
                    view->setRootIndex(ind.parent());
                    view->setCurrentIndex(ind);
                }
                else
                {
                    if (view == ui->filtersView)
                    {
                        ui->pages->setCurrentWidget(ui->filtersAndPresetsPage);
                        ui->filtersAndPresetsList->setEditFocus(true);
                    }
                    else
                        ui->pages->setCurrentWidget(ui->mainPage);
                }
            }
            break;
        case Qt::Key_Left :
            view->setCurrentIndex(view->currentIndex().sibling(0, 0));
            break;
        case Qt::Key_Right :
            {
                int count = view->currentIndex().model()->rowCount(view->currentIndex().parent());
                view->setCurrentIndex(view->currentIndex().sibling(count - 1, 0));
            }
            break;
    }
}

void MainWindow::processUtilitiesPage(QKeyEvent * event)
{
    int res = 0;
    switch (event->key())
    {
        case Qt::Key_Escape :
            ui->pages->setCurrentWidget(ui->mainPage);
            break;
        case Qt::Key_Select :
            if (ui->utilitiesList->currentRow() == 0)
            {
                res = QMessageBox::question(this, "", trUtf8("Заблокировать кнопки от случайных нажатий?"),
                                            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                if (QMessageBox::Yes == res)
                    zdbus->setParameter("Temp", "Security.Keyboard_lock.Active", 1);
            }
            else if (ui->utilitiesList->currentRow() == 1)
            {
                res = QMessageBox::question(this, "", trUtf8("Очистить корзину?"),
                                            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
                if (QMessageBox::Yes == res)
                    externalCommand.start("/usr/bin/emptyrecycle.sh");
            }
            else if (ui->utilitiesList->currentRow() == 2)
            {
                res = QMessageBox::warning(this, "", trUtf8("После форматирования все записи будут потеряны! Форматировать?"),
                                           QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
                if (QMessageBox::Yes == res)
                    externalCommand.start("/usr/bin/mmcformat.sh");
            }
            else if (ui->utilitiesList->currentRow() == 3)
            {
                QFile log("/etc/firmware/logs/problems.log");
                if (log.open(QIODevice::ReadOnly))
                {
                    ui->pages->setCurrentWidget(ui->logsPage);
                    //ui->logsBrowser->setSource(QUrl());
                    ui->logsBrowser->setPlainText(QString(log.readAll()));
                    ui->logsBrowser->setEditFocus(true);
                    log.close();
                    break;
                }
                else
                    showMessage(QMessageBox::Critical, trUtf8("Невозможно открыть файл журнала!"));
            }
            if (res)
                ui->utilitiesList->setEditFocus(true);
            break;
    }
}


void MainWindow::processFiltersAndPresetsPage(QKeyEvent * event)
{
    switch (event->key())
    {
        case Qt::Key_Escape :
            ui->pages->setCurrentWidget(ui->mainPage);
            break;
        case Qt::Key_Select :
            if (ui->filtersAndPresetsList->currentRow() == 0)
            {
                // Идем наверх
                while (ui->filtersView->rootIndex().isValid())
                    ui->filtersView->setRootIndex(ui->filtersView->rootIndex().parent());
                ui->pages->setCurrentWidget(ui->filtersPage);
                ui->filtersView->setEditFocus(true);
                // Чтобы элемент сразу выделился
                ui->filtersView->setCurrentIndex(filters.index(0, 0, ui->filtersView->rootIndex()));
                currentView = ui->filtersView;
                currentPage = ui->filtersPage;
            }
            else if (ui->filtersAndPresetsList->currentRow() == 1)
            {
                QStringList presets;
                int res = zdbus->getPresetsListing(presets);
                if (res)
                {
                    showMessage(QMessageBox::Critical, trUtf8("Не удалось получить список пресетов! Код ошибки ") + QString("%1.").arg(res));
                    break;
                }
                ui->presetsList->clear();
                for (int i = 0; i < presets.count(); ++i)
                {
                    presets[i].replace(QChar('-'), QChar('/'));
                    ui->presetsList->addItem(QString::fromUtf8(QByteArray::fromBase64(presets[i].toAscii()).data()));
                }
                ui->presetsList->sortItems();
                ui->pages->setCurrentWidget(ui->presetsPage);
                ui->presetsList->setCurrentRow(0);
                ui->presetsList->scrollToTop();
                ui->presetsList->setEditFocus(true);
            }
            else if (ui->filtersAndPresetsList->currentRow() == 2)
            {
                doRenamePreset = false;
                ui->presetNewNameLabel->setText("");
                ui->pages->setCurrentWidget(ui->presetNamePage);
                ui->pushButton_36->setFocus();
            }
            break;
    }
}

void MainWindow::processPresetsPage(QKeyEvent * event)
{
    switch (event->key())
    {
        case Qt::Key_Escape :
            ui->pages->setCurrentWidget(ui->filtersAndPresetsPage);
            ui->filtersAndPresetsList->setEditFocus(true);
            break;
        case Qt::Key_Select :
            {
                QString presetName = ui->presetsList->currentItem()->text();
                ui->presetNameLabel->setText(presetName);
                ui->presetOpsList->clear();
                ui->presetOpsList->addItem(trUtf8("Применить"));
                if (presetName[0] != '!')
                {
                    ui->presetOpsList->addItem(trUtf8("Переименовать"));
                    ui->presetOpsList->addItem(trUtf8("Удалить"));
                }
                ui->pages->setCurrentWidget(ui->presetOpsPage);
                ui->presetOpsList->setCurrentRow(0);
                ui->presetOpsList->setEditFocus(true);
            }
            break;
    }
}

void MainWindow::processPresetOpsPage(QKeyEvent * event)
{
    switch (event->key())
    {
        case Qt::Key_Escape :
            ui->pages->setCurrentWidget(ui->presetsPage);
            ui->presetsList->setEditFocus(true);
            break;
        case Qt::Key_Select :
            if (ui->presetOpsList->currentRow() == 0)
            {
                QString name = ui->presetNameLabel->text().toUtf8().toBase64();
                name.replace(QChar('/'), QChar('-'));
                int res = zdbus->applyPreset(name);
                if (res)
                {
                    showMessage(QMessageBox::Critical, trUtf8("Не удалось применить пресет! Код ошибки ") + QString("%1.").arg(res));
                    break;
                }
                ui->pages->setCurrentWidget(ui->presetsPage);
                ui->presetsList->setEditFocus(true);
            }
            if (ui->presetOpsList->currentRow() == 1)
            {
                doRenamePreset = true;
                ui->presetNewNameLabel->setText(ui->presetNameLabel->text());
                ui->pages->setCurrentWidget(ui->presetNamePage);
                ui->pushButton_36->setFocus();
                break;
            }
            if (ui->presetOpsList->currentRow() == 2)
            {
                QString msgText(trUtf8("Удалить пресет ") + ui->presetNameLabel->text() + "?");
                confirmMessage->setText(msgText);
                confirmMessage->setDefaultButton(QMessageBox::No);
                int res = confirmMessage->exec();
                if (QMessageBox::No == res)
                    ui->presetOpsList->setEditFocus(true);
                else if (QMessageBox::Yes == res)
                {
                    QString name = ui->presetNameLabel->text().toUtf8().toBase64();
                    name.replace(QChar('/'), QChar('-'));
                    int res = zdbus->deletePreset(name);
                    if (res)
                    {
                        showMessage(QMessageBox::Critical, trUtf8("Не удалось удалить пресет! Код ошибки ") + QString("%1.").arg(res));
                        break;
                    }

                    int row = ui->presetsList->currentRow();
                    if (ui->presetsList->count() > 1)
                    {
                        if (ui->presetsList->currentRow() == ui->presetsList->count() - 1)
                            ui->presetsList->setCurrentRow(ui->presetsList->currentRow() - 1);
                        else
                            ui->presetsList->setCurrentRow(ui->presetsList->currentRow() + 1);
                    }
                    delete ui->presetsList->takeItem(row);
                    ui->pages->setCurrentWidget(ui->presetsPage);
                    ui->presetsList->setEditFocus(true);
                }
            }
            break;
    }
}

void MainWindow::processPresetNamePage(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Escape)
    {
        if (doRenamePreset)
        {
            ui->pages->setCurrentWidget(ui->presetOpsPage);
            ui->presetOpsList->setEditFocus(true);
        }
        else
        {
            ui->pages->setCurrentWidget(ui->filtersAndPresetsPage);
            ui->filtersAndPresetsList->setEditFocus(true);
        }
    }
}

void MainWindow::processLogsBrowserPage(QKeyEvent * event)
{
    switch (event->key())
    {
        case Qt::Key_Escape :
        case Qt::Key_Select :
            ui->pages->setCurrentWidget(ui->utilitiesPage);
            ui->utilitiesList->setEditFocus(true);
            break;
//        case Qt::Key_Up :
//            ui->logsBrowser
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
        case 2 :
        {
            QString msgText(trUtf8("Переместить файл %1 в корзину?").arg(files.fileName(ui->filesView->currentIndex())));
            confirmMessage->setText(msgText);
            confirmMessage->setDefaultButton(QMessageBox::No);
            int res = confirmMessage->exec();
            if (QMessageBox::Yes == res)
            {
                QModelIndex index = ui->filesView->currentIndex();
                if (files.rowCount(index.parent()) > 1)
                {
                    if (index.row() == files.rowCount(index.parent()) - 1)
                        ui->filesView->setCurrentIndex(index.sibling(index.row() - 1, 0));
                    else
                        ui->filesView->setCurrentIndex(index.sibling(index.row() + 1, 0));
                }
                QString fName = files.fileName(index);
                files.fileInfo(index).absoluteDir().mkdir(".Trash");
                files.fileInfo(index).absoluteDir().rename(fName, QString(".Trash/") + fName);
                ui->pages->setCurrentWidget(ui->browserPage);
                ui->filesView->setEditFocus(true);
            }
            else if (QMessageBox::No == res)
                ui->fileOpsList->setEditFocus(true);
        }
    }

}

void MainWindow::processLockedPages(QKeyEvent * event)
{
    if (ui->pages->currentWidget() == ui->lockedPage1 && event->key() == Qt::Key_Left)
    {
        ui->pages->setCurrentWidget(ui->lockedPage2);
        QTimer::singleShot(2000, this, SLOT(setLocked1()));
    }
    else if (ui->pages->currentWidget() == ui->lockedPage2 && event->key() == Qt::Key_Right)
        zdbus->setParameter("Temp", "Security.Keyboard_lock.Active", 0);
}

void MainWindow::setLocked1()
{
    if (ui->pages->currentWidget() == ui->lockedPage2)
        ui->pages->setCurrentWidget(ui->lockedPage1);
}

void MainWindow::processPinCodePage(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Select)
    {
        int currentPin;
        if (!zdbus->getParameter("Main", "Security.Protection.PIN", &currentPin))
            showMessage(QMessageBox::Critical, trUtf8("Не удалось получить текущий ПИН-код!"));
        else
        {
            int newPin = ui->digit1->value() * 1000 + ui->digit2->value() * 100 + ui->digit3->value() * 10 + ui->digit4->value();
            if (newPin != currentPin)
                showMessage(QMessageBox::Critical, trUtf8("ПИН-код введен неверно!"));
            else
                zdbus->setParameter("Temp", "Security.Keyboard_lock.Active", 0);
        }
    }
    else
        QMainWindow::keyPressEvent(event);
}

bool MainWindow::processEncoders(QKeyEvent * event)
{
    switch (event->key())
    {
        case Qt::Key_E :
            if (keyboardLocked) break;
            if (ui->statusPages->currentIndex() != 1)
                ui->statusPages->setCurrentWidget(ui->gainPlayStatus);
            zdbus->sendRotaryEvent("PlayEncoder", "+");
            break;
        case Qt::Key_C :
            if (keyboardLocked) break;
            if (ui->statusPages->currentIndex() != 1)
                ui->statusPages->setCurrentWidget(ui->gainPlayStatus);
            zdbus->sendRotaryEvent("PlayEncoder", "-");
            break;
        case Qt::Key_Q :
            if (keyboardLocked) break;
            if (ui->statusPages->currentIndex() != 2)
                ui->statusPages->setCurrentWidget(ui->gainRecStatus);
            zdbus->sendRotaryEvent("LeftRecEncoder", "+");
            break;
        case Qt::Key_Z :
            if (keyboardLocked) break;
            if (ui->statusPages->currentIndex() != 2)
                ui->statusPages->setCurrentWidget(ui->gainRecStatus);
            zdbus->sendRotaryEvent("LeftRecEncoder", "-");
            break;
        case Qt::Key_W :
            if (keyboardLocked) break;
            if (ui->statusPages->currentIndex() != 2)
                ui->statusPages->setCurrentWidget(ui->gainRecStatus);
            zdbus->sendRotaryEvent("RightRecEncoder", "+");
            break;
        case Qt::Key_X :
            if (keyboardLocked) break;
            if (ui->statusPages->currentIndex() != 2)
                ui->statusPages->setCurrentWidget(ui->gainRecStatus);
            zdbus->sendRotaryEvent("RightRecEncoder", "-");
            break;
        default:
            return false;
    }

    if (!keyboardLocked)
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
    settings.setValueByName(_param, value);
    mixer.setValueByName(_param, value);
    filters.setValueByName(_param, value);

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
            ui->cardStatusPages->setCurrentWidget(ui->cardErrorPage);
        }
        else if (currentSDState != NotFormatted)
        {
            ui->cardStatusPages->setCurrentWidget(ui->cardInfoPage);
            currentSDState = SDOK;
        }
    }

    else if (_param == "Storage.Formatted")
    {
        if (value == 0 && currentSDState != NotConnected)
        {
            currentSDState = NotFormatted;
            ui->cardErrorLabel->setText(trUtf8("SD-карта не\nотформат-на!"));
            ui->cardStatusPages->setCurrentWidget(ui->cardErrorPage);
        }
        else if (value == 1 && currentSDState != NotConnected)
        {
            ui->cardStatusPages->setCurrentWidget(ui->cardInfoPage);
            rootIndex = files.index("/tmp/sound");
            ui->filesView->setRootIndex(rootIndex);
            SetWatcher(rootIndex);
            currentSDState = SDOK;
        }
        else if (value == 2 && currentSDState != NotConnected)
        {
            ui->cardErrorLabel->setText(trUtf8("Идет\nформатир-е..."));
            ui->cardStatusPages->setCurrentWidget(ui->cardErrorPage);
            rootIndex = files.index("/tmp/sound");
            ui->filesView->setRootIndex(rootIndex);
            SetWatcher(rootIndex);
            currentSDState = SDOK;
        }
    }

    else if (_param == "Storage.Free_space")
    {
        float val = value;
        if (value >= 1000)
        {
            val /= 1024.;
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
            val /= 1024.;
            ui->measureTotal->setText(trUtf8("Гб"));
        }
        else
            ui->measureTotal->setText(trUtf8("Мб"));
        ui->valueTotal->setText(QString("%1").arg(val, 0, 'f', 1));
    }

    else if (_param == "Recorder.Sample_size")
    {
        ui->sampleSizeLabel->setText(settings.getValueByName(_param));
    }
    else if (_param == "Recorder.Compression")
    {
        ui->compressionLabel->setText(settings.getValueByName(_param));
    }
    else if (_param == "Recorder.Sample_rate")
    {
        ui->sampleRateLabel->setText(settings.getValueByName(_param));
        ui->sampleRateLabel->update();
    }
    else if (_param == "Recorder.Channels")
    {
        ui->channelsLabel->setText(settings.getValueByName(_param));
    }

    else if (_param == "Security.Keyboard_lock.Auto_lock")
    {
        keyboardLockTimeout = keyboardAutolockTimes[value];
        keyboardLockTimer.stop();
        if (keyboardLockTimeout)
        {
            keyboardLockTimer.setInterval(keyboardLockTimeout);
            keyboardLockTimer.start();
        }
    }

    else if (_param == "Security.Keyboard_lock.Active")
    {
        keyboardLocked = value;
        if (value == 1)
        {
            activeWindow = QApplication::activeWindow();
            if (activeWindow && activeWindow != this)
            {
                activeWindow->hide();
                focusedWidgets.push(this->focusWidget());
            }
            else if (QApplication::focusWidget())
                focusedWidgets.push(QApplication::focusWidget());
            beforeLockWidget = ui->pages->currentWidget();
            if (pinCodeProtection)
            {
                ui->digit1->setValue(0);
                ui->digit2->setValue(0);
                ui->digit3->setValue(0);
                ui->digit4->setValue(0);
                ui->pages->setCurrentWidget(ui->pinCodePage);
                ui->digit1->setEditFocus(true);
            }
            else
                ui->pages->setCurrentWidget(ui->lockedPage1);

            if (backlightTimeout)
            {
                backlightTimer.setInterval(5000);
                backlightTimer.start();
            }
        }
        else
        {
            if (beforeLockWidget)
                ui->pages->setCurrentWidget(beforeLockWidget);
            if (!focusedWidgets.isEmpty())
            {
                qDebug() << focusedWidgets;
                focusedWidgets.pop()->setEditFocus(true);
            }

            if (backlightTimeout)
            {
                backlightTimer.setInterval(backlightTimeout);
                backlightTimer.start();
            }
        }
    }

    else if (_param == "Display.Backlight.Auto_turn_off")
    {
        backlightTimer.stop();
        backlightTimeout = backlightTimes[value];
        if (backlightTimeout)
        {
            backlightTimer.setInterval(backlightTimeout);
            backlightTimer.start();
        }
    }

    else if (_param == "Mixer.Headset.Connected")
    {
        if (value == 1)
            ui->jackWidget->setStyleSheet("background-image: url(:/all/res/headphones.png);"
                                          "background-repeat: repeat-n;"
                                          "background-position: center;");
        else
            ui->jackWidget->setStyleSheet("background-image: none");
    }
    else if (_param == "Recorder.Timer.Enabled")
    {
        if (value == 1)
            ui->timerWidget->setStyleSheet("background-image: url(:/all/res/clock.png);"
                                           "background-repeat: repeat-n;"
                                           "background-position: center;");
        else
            ui->timerWidget->setStyleSheet("background-image: none");
    }
    else if (_param == "Security.Protection.Enabled")
    {
        pinCodeProtection = value;
        if (value == 1)
            ui->PINwidget->setStyleSheet("background-image: url(:/all/res/key.png);"
                                         "background-repeat: repeat-n;"
                                         "background-position: center;");
        else
            ui->PINwidget->setStyleSheet("background-image: none");
    }
}

void MainWindow::restartInactivityTimers()
{
    if (keyboardLockTimeout)
        keyboardLockTimer.start();
    if (backlightIsOff)
    {
        ioctl(fdFrameBuffer, FBIOBLANK, FB_BLANK_UNBLANK);
        backlightIsOff = false;
        backlightTimer.start();
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (obj == ui->digit1 || obj == ui->digit2 || obj == ui->digit3 || obj == ui->digit4)
        {
            if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Left ||
                static_cast<QKeyEvent *>(event)->key() == Qt::Key_Right)
            {
                QCoreApplication::sendEvent(this, event);
                QApplication::focusWidget()->setEditFocus(true);
                return true;
            }
        }
        if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Select)
        {
            QCoreApplication::sendEvent(this, event);
            return true;
        }
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
        showMessage(QMessageBox::Information, messageText);
    else if (_type == 1)
        showMessage(QMessageBox::Critical, messageText);
}

void MainWindow::showMessage(QMessageBox::Icon _type, const QString & _message)
{
    if (!message)
        focusedWidgets.push(QApplication::focusWidget());

    message = new QMessageBox(this);
    message->setText(_message);
    message->setStandardButtons(QMessageBox::Ok);
    message->setIcon(_type);
    messageTimer.start();
    message->exec();
    messageTimer.stop();
    removeMessageBox();
}

void MainWindow::removeMessageBox()
{
    if (message)
    {
        delete message;
        message = 0;
        if ((focused = focusedWidgets.pop()))
            focused->setEditFocus(true);
    }
}

void MainWindow::lockKeyboard()
{
    if (!keyboardLocked)
        zdbus->setParameter("Temp", "Security.Keyboard_lock.Active", 1);
}

void MainWindow::backlightTurnOff()
{
    if (!backlightIsOff)
    {
        ioctl(fdFrameBuffer, FBIOBLANK, FB_BLANK_POWERDOWN);
        backlightIsOff = true;
    }
}

void MainWindow::virtualKeyboardPressed()
{
    QObject * button = sender();
    QString name = ui->presetNewNameLabel->text();

    if (button == ui->vkbdButton_Backspace)
    {
        if (!name.isEmpty())
            name.remove(name.length() - 1, 1);
    }
    else if (button == ui->vkbdButton_Space && name.length() < 20)
        name += " ";
    else if (button == ui->vkbdButton_Caps)
    {
        QPushButton * current;
        QList<QPushButton * > buttons = ui->virtualKeyboardWidget->findChildren<QPushButton * >(QRegExp("pushButton"));
        if (ui->pushButton_17->text() == "Q")
            foreach (current, buttons)
                current->setText(current->text().toLower());
        else
            foreach (current, buttons)
                current->setText(current->text().toUpper());
    }
    else if (button == ui->presetSaveButton && doRenamePreset)
    {
        int res = zdbus->renamePreset(ui->presetNameLabel->text().toUtf8().toBase64().replace('/', '-'), name.toUtf8().toBase64().replace('/', '-'));
        if (res)
            showMessage(QMessageBox::Critical, trUtf8("Не удалось переименовать пресет! Код ошибки ") + QString("%1.").arg(res));
        else
        {
            ui->presetNameLabel->setText(name);
            ui->presetsList->currentItem()->setText(name);
            ui->pages->setCurrentWidget(ui->presetOpsPage);
            ui->presetOpsList->setEditFocus(true);
        }
    }
    else if (button == ui->presetSaveButton)
    {
        int res = zdbus->savePreset(name.toUtf8().toBase64().replace('/', '-'));
        if (res)
            showMessage(QMessageBox::Critical, trUtf8("Не удалось сохранить пресет! Код ошибки ") + QString("%1.").arg(res));
        else
        {
            ui->pages->setCurrentWidget(ui->filtersAndPresetsPage);
            ui->filtersAndPresetsList->setEditFocus(true);
        }
    }
    else
        if (name.length() < 20)
            name += dynamic_cast<QPushButton *>(button)->text();

    ui->presetNewNameLabel->setText(name);
}
