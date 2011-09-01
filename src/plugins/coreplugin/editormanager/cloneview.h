/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** No Commercial Usage
**
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
**
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#ifndef CORE_INTERNAL_CLONEVIEW_H
#define CORE_INTERNAL_CLONEVIEW_H

#include "editorview.h"

#include <QMap>
#include <QSet>

namespace Core {
namespace Internal {

class CloneView : public EditorView
{
    Q_OBJECT

public:
    explicit CloneView(QWidget *parent = 0);

private slots:
    void displayEditor(Core::IEditor *editor);
    void closeEditors(QList<Core::IEditor *> editors);
    void updateCursorPosition();

private:
    QSet<Core::IEditor*> m_connectedEditors;
    QMap<Core::IEditor*, Core::IEditor*> m_editors; // Editor in Qt Creator/Editor in the clone view
};

} // namespace Internal
} // namespace Core

#endif // CORE_INTERNAL_CLONEVIEW_H
