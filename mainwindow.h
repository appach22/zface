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

#include "ZSettingsModel.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent * event);

private:
    Ui::MainWindow *ui;
    QImage img;
    QWidget * current;
    QDirModel files;
    ZSettingsModel settings;
    QFileSystemWatcher * watcher;
    QTimer * gainTimer;
    int playGain;

    void SetWatcher(QModelIndex root);

    void processMainPage(QKeyEvent * event);
    void processBrowserPage(QKeyEvent * event);
    void processSettingsPage(QKeyEvent * event);
    void processParameterPage(QKeyEvent * event);
    void processEncoders(QKeyEvent * event);

private slots:
    void updateTime();
    void hideGain();
    void refreshPath();

};

#endif // MAINWINDOW_H
