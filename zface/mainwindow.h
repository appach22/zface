#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QPainter>
#include <QtGui/QMessageBox>
#include <QtGui/QKeyEvent>
#include <QDirModel>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QFileSystemWatcher>
#include <QtGui/QListView>

#include "ZSettingsModel.h"
#include "ZDbus.h"


namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum PlayState
    {
        Stopped,
        Playing,
        Paused
    };

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent * event);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::MainWindow *ui;
    QImage img;
    QWidget * current;
    QDirModel files;
    ZSettingsModel settings;
    ZSettingsModel mixer;
    QFileSystemWatcher * watcher;
    QTimer * gainTimer;
    QListView * currentView;
    QWidget * currentPage;
    QModelIndex rootIndex;
    SoundFileInfo currentFileInfo;
    PlayState currentPlayState;

    void SetWatcher(QModelIndex root);

    void processMainPage(QKeyEvent * event);
    void processBrowserPage(QKeyEvent * event);
    void processSettingsPage(QKeyEvent * event, QListView * view);
    void processParameterPage(QKeyEvent * event);
    void processPlayPage(QKeyEvent * event);
    bool processEncoders(QKeyEvent * event);
    void processFileOps();

private slots:
    void updateTime();
    void hideGain();
    void refreshPath();
    void gainChanged(QString _gain, QString _value);
    void paramChanged(QString _param, QString _value);
};

#endif // MAINWINDOW_H
