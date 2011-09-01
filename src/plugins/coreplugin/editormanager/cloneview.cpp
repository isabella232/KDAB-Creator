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

#include "cloneview.h"
#include "editormanager.h"
#include "ieditor.h"

#include <coreplugin/editortoolbar.h>

#include <QPlainTextEdit>
#include <QEvent>

using namespace Core;
using namespace Core::Internal;

CloneView::CloneView(QWidget *parent) :
    EditorView(parent)
{
    setWindowFlags(Qt::Dialog);
    connect(EditorManager::instance(), SIGNAL(currentEditorChanged(Core::IEditor*)),
            this, SLOT(displayEditor(Core::IEditor*)));
    connect(EditorManager::instance(), SIGNAL(editorOpened(Core::IEditor*)),
            this, SLOT(displayEditor(Core::IEditor*)));
    connect(EditorManager::instance(), SIGNAL(editorsClosed(QList<Core::IEditor*>)),
            this, SLOT(closeEditors(QList<Core::IEditor*>)));

    findChild<EditorToolBar*>()->hide();
    displayEditor(EditorManager::instance()->currentEditor());
}

void CloneView::closeEditors(QList<Core::IEditor *> editors)
{
    foreach(Core::IEditor *editor, editors) {
        if (m_editors.contains(editor)) {
            Core::IEditor *duplicate = m_editors.value(editor);
            removeEditor(duplicate);
            m_editors.remove(editor);
            delete duplicate;
        }
    }
}

void CloneView::displayEditor(Core::IEditor *editor)
{
    if (!editor)
        return;

    Core::IEditor *duplicate = 0;

    if (m_editors.contains(editor))
        duplicate = m_editors.value(editor);

    if (!duplicate && editor && editor->duplicateSupported()) {
        duplicate = editor->duplicate(0);
        m_editors[editor] = duplicate;
        addEditor(duplicate);
    }

    if (duplicate) {
        setCurrentEditor(duplicate);
        setWindowTitle(duplicate->displayName());
        if (!m_connectedEditors.contains(editor)) {
            QPlainTextEdit *textEdit = qobject_cast<QPlainTextEdit*>(editor->widget());
            if (textEdit)
                m_connectedEditors.insert(editor);
            connect(textEdit, SIGNAL(cursorPositionChanged()),
                    this, SLOT(updateCursorPosition()));
        }
    }

    editor->widget()->setFocus();
}

void CloneView::updateCursorPosition()
{
    QPlainTextEdit *textEdit = qobject_cast<QPlainTextEdit*>(
                EditorManager::instance()->currentEditor()->widget());
    QPlainTextEdit *duplicateEdit = qobject_cast<QPlainTextEdit*>(currentEditor()->widget());
    QTextCursor cursor = duplicateEdit->textCursor();
    cursor.setPosition(textEdit->textCursor().position());
    duplicateEdit->setTextCursor(cursor);
}
