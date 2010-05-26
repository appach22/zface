#include "KeyProcessor.h"
#include "mainwindow.h"

namespace zface
 {

KeyProcessor::KeyProcessor(MainWindow * _wnd)
{
    m_wnd = _wnd;
    m_ui = m_wnd->ui;
}

void KeyProcessor::ProcessKey(QKeyEvent * event)
{
    switch (event->key())
    {
        case Qt::Key_Escape :
            m_ui->pages->setCurrentWidget(m_ui->mainPage);
            break;
        case Qt::Key_Down   :
            m_ui->pages->setCurrentWidget(m_ui->browserPage);
            m_ui->filesView->setFocus();
            break;
        case Qt::Key_Return :
        case Qt::Key_Right :
            if (m_ui->pages->currentIndex() == 1)
            {
                if (m_wnd->files.fileInfo(m_ui->filesView->currentIndex()).isDir())
                {
                    m_wnd->SetWatcher(m_ui->filesView->currentIndex());
                    m_ui->filesView->setRootIndex(m_ui->filesView->currentIndex());
                    files.refresh(files.parent(m_ui->filesView->currentIndex()));
                    m_ui->filesView->setFocus();
                }
            }
            else if (m_ui->pages->currentIndex() == 0)
                m_ui->pages->setCurrentWidget(m_ui->settingsPage);
            break;
        case Qt::Key_Left :
            if (m_ui->pages->currentIndex() == 1)
            {
                m_ui->filesView->setRootIndex(files.parent(m_ui->filesView->rootIndex()));
                SetWatcher(m_ui->filesView->rootIndex());
                files.refresh(m_ui->filesView->rootIndex());
                m_ui->filesView->setFocus();
            }
            break;
        case Qt::Key_Plus :
            if (m_ui->statusPages->currentIndex() != 1)
                m_ui->statusPages->setCurrentWidget(m_ui->gainPlayStatus);
            if (m_wnd->playGain < 100)
            {
                m_ui->gainPlayProgress->setValue(++playGain);
                m_ui->gainPlayValue->setText(QString::number(playGain));
            }
            gainTimer->start(1000);
            break;
        case Qt::Key_Minus :
            if (m_ui->statusPages->currentIndex() != 1)
                m_ui->statusPages->setCurrentWidget(m_ui->gainPlayStatus);
            if (m_wnd->playGain > 0)
            {
                m_ui->gainPlayProgress->setValue(--playGain);
                m_ui->gainPlayValue->setText(QString::number(playGain));
            }
            m_wnd->gainTimer->start(1000);
            break;

    }
}

} // namespace zface
