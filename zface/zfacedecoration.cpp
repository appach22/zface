/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include "zfacedecoration.h"

ZFaceDecoration::ZFaceDecoration()
    : QDecorationDefault()
{
    border = 4;
    titleHeight = 0;
    buttonWidth = 0;
    buttonHeight = 0;
    buttonMargin = 0;
}

QRegion ZFaceDecoration::region(const QWidget *, const QRect &insideRect,
                             int decorationRegion)
{
    QRect outsideRect(insideRect.left() - border,
                      insideRect.top() - titleHeight - border,
                      insideRect.width() + 2 * border,
                      insideRect.height() + titleHeight + 2 * border);

    QRegion region;

    if (decorationRegion == All) {
        region += QRegion(outsideRect) - QRegion(insideRect);
        return region;
    }
    if (decorationRegion & Top) {
        QRect rect = outsideRect.adjusted(border, 0, -border, 0);
        rect.setHeight(border);
        region += rect;
    }
    if (decorationRegion & Left) {
        QRect rect = outsideRect.adjusted(0, border, 0, -border);
        rect.setWidth(border);
        region += rect;
    }
    if (decorationRegion & Right) {
        QRect rect = outsideRect.adjusted(0, border, 0, -border);
        rect.setLeft(rect.right() + 1 - border);
        region += rect;
    }
    if (decorationRegion & Bottom) {
        QRect rect = outsideRect.adjusted(border, 0, -border, 0);
        rect.setTop(rect.bottom() + 1 - border);
        region += rect;
    }
    if (decorationRegion & TopLeft) {
        QRect rect = outsideRect;
        rect.setWidth(border);
        rect.setHeight(border);
        region += rect;
    }
    if (decorationRegion & TopRight) {
        QRect rect = outsideRect;
        rect.setLeft(rect.right() + 1 - border);
        rect.setHeight(border);
        region += rect;
    }
    if (decorationRegion & BottomLeft) {
        QRect rect = outsideRect;
        rect.setWidth(border);
        rect.setTop(rect.bottom() + 1 - border);
        region += rect;
    }
    if (decorationRegion & BottomRight) {
        QRect rect = outsideRect;
        rect.setLeft(rect.right() + 1 - border);
        rect.setTop(rect.bottom() + 1 - border);
        region += rect;
    }
    return region;
}

bool ZFaceDecoration::paint(QPainter *painter, const QWidget *widget,
                         int decorationRegion, DecorationState state)
{
    if (decorationRegion == None)
        return false;

    bool handled = false;

    QPalette palette = QApplication::palette();
    if (decorationRegion & Top) {
        QRect rect = QDecoration::region(widget, Top).boundingRect();
        painter->fillRect(rect, palette.brush(QPalette::Dark));
        handled = true;
    }
    if (decorationRegion & Left) {
        QRect rect = QDecoration::region(widget, Left).boundingRect();
        painter->fillRect(rect, palette.brush(QPalette::Dark));
        handled = true;
    }
    if (decorationRegion & Right) {
        QRect rect = QDecoration::region(widget, Right).boundingRect();
        painter->fillRect(rect, palette.brush(QPalette::Dark));
        handled = true;
    }
    if (decorationRegion & Bottom) {
        QRect rect = QDecoration::region(widget, Bottom).boundingRect();
        painter->fillRect(rect, palette.brush(QPalette::Dark));
        handled = true;
    }
    if (decorationRegion & TopLeft) {
        QRect rect = QDecoration::region(widget, TopLeft).boundingRect();
        painter->fillRect(rect, palette.brush(QPalette::Dark));
        handled = true;
    }
    if (decorationRegion & TopRight) {
        QRect rect = QDecoration::region(widget, TopRight).boundingRect();
        painter->fillRect(rect, palette.brush(QPalette::Dark));
        handled = true;
    }
    if (decorationRegion & BottomLeft) {
        QRect rect = QDecoration::region(widget, BottomLeft).boundingRect();
        painter->fillRect(rect, palette.brush(QPalette::Dark));
        handled = true;
    }
    if (decorationRegion & BottomRight) {
        QRect rect = QDecoration::region(widget, BottomRight).boundingRect();
        painter->fillRect(rect, palette.brush(QPalette::Dark));
        handled = true;
    }

    return handled;
}
