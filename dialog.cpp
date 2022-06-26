/**
 * \file dialog.cpp
 *
 * \section LICENSE
 *
 * Original code Copyright (c) 2011, Volker Götz
 * Modified code Copyright (c) 2020-2022, Thorsten Roth
 * All rights reserved.
 *
 * This file is part of nuspell-qt.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "./dialog.h"

#include <QDebug>
#include <QMessageBox>

#include <nuspell/dictionary.hxx>
#include <nuspell/finder.hxx>

#include "ui_dialog.h"
#include "./nuspellcheck.h"
#include "./nuspellcheckdialog.h"

Dialog::Dialog(QWidget *pParent)
  : QDialog(pParent),
    ui(new Ui::Dialog) {
  ui->setupUi(this);
  connect(ui->buttonCheckSpelling, &QPushButton::clicked,
          this, &Dialog::checkSpelling);
}


Dialog::~Dialog() {
  delete ui;
}


void Dialog::checkSpelling() {
  QString language = QStringLiteral("en_US");

  auto dirs = std::vector<std::filesystem::path>();
  nuspell::append_default_dir_paths(dirs);
  auto dict_list = nuspell::search_default_dirs_for_dicts();

  // Add application folder to search folder for dictionaries
  std::filesystem::path app_path(
        QString(qApp->applicationDirPath() + "/dicts/").toStdU16String());
  auto dirs2 = std::vector<std::filesystem::path>();
  dirs2.push_back(app_path);
  nuspell::search_dirs_for_dicts(dirs2, dict_list);

  qDebug() << "Found dictionaries:";
  for (auto s : dict_list) {
    qDebug() << QString::fromUtf16(s.u16string().c_str());
  }

  auto dict_path = nuspell::search_dirs_for_one_dict(dirs,
                                                     language.toStdString());
  if (std::empty(dict_path)) {
    qWarning() << "Can not find the requested dictionary.";
    QMessageBox::warning(this, tr("Warning"),
                         tr("Can not find the requested dictionary!"));
    return;
  }

  auto dict = nuspell::Dictionary();
  try {
    dict.load_aff_dic(dict_path);
  }
  catch (const nuspell::Dictionary_Loading_Error& e) {
    qWarning() << e.what() << '\n';
    QMessageBox::warning(this, tr("Warning"), QString::fromLatin1(e.what()));
    return;
  }

  // Save user dictionary in same folder as application
  QString userDict = "userDict_" + language + ".txt";

  NuspellCheck *spellChecker = new NuspellCheck(&dict, userDict);
  NuspellCheckDialog *checkDialog = new NuspellCheckDialog(spellChecker, this);

  QTextCharFormat highlightFormat;
  highlightFormat.setBackground(QBrush(QColor(0xff, 0x60, 0x60)));
  highlightFormat.setForeground(QBrush(QColor(0, 0, 0)));
  // alternative format
  // highlightFormat.setUnderlineColor(QColor("red"));
  // highlightFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

  // save the position of the current cursor
  QTextCursor oldCursor = ui->textEdit->textCursor();

  // create a new cursor to walk through the text
  QTextCursor cursor(ui->textEdit->document());

  QList<QTextEdit::ExtraSelection> esList;

  // Don't call cursor.beginEditBlock(), as this prevents the redraw after
  // changes to the content cursor.beginEditBlock();
  while (!cursor.atEnd()) {
    QCoreApplication::processEvents();
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
    QString word = cursor.selectedText();

    // Workaround for better recognition of words
    // punctuation etc. does not belong to words
    while (!word.isEmpty() &&
           !word.at(0).isLetter() &&
           cursor.anchor() < cursor.position()) {
      int cursorPos = cursor.position();
      cursor.setPosition(cursor.anchor() + 1, QTextCursor::MoveAnchor);
      cursor.setPosition(cursorPos, QTextCursor::KeepAnchor);
      word = cursor.selectedText();
    }

    if (!word.isEmpty() && !spellChecker->spell(word)) {
      QTextCursor tmpCursor(cursor);
      tmpCursor.setPosition(cursor.anchor());
      ui->textEdit->setTextCursor(tmpCursor);
      ui->textEdit->ensureCursorVisible();

      // highlight the unknown word
      QTextEdit::ExtraSelection es;
      es.cursor = cursor;
      es.format = highlightFormat;

      esList << es;
      ui->textEdit->setExtraSelections(esList);
      QCoreApplication::processEvents();

      // ask the user what to do
      NuspellCheckDialog::SpellCheckAction spellResult =
          checkDialog->checkWord(word);

      // reset the word highlight
      esList.clear();
      ui->textEdit->setExtraSelections(esList);
      QCoreApplication::processEvents();

      if (spellResult == NuspellCheckDialog::AbortCheck)
        break;

      switch (spellResult) {
      case NuspellCheckDialog::ReplaceOnce:
        cursor.insertText(checkDialog->replacement());
        break;
      case NuspellCheckDialog::ReplaceAll:
        replaceAll(cursor.position(), word, checkDialog->replacement());
        break;

      default:
        break;
      }
      QCoreApplication::processEvents();
    }
    cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
  }
  // cursor.endEditBlock();

  delete checkDialog;
  checkDialog = nullptr;
  ui->textEdit->setTextCursor(oldCursor);

  QMessageBox::information(this, tr("Finished"),
                           tr("The spell check has finished."));
}


void Dialog::replaceAll(int nPos, const QString &sOld, const QString &sNew) {
  QTextCursor cursor(ui->textEdit->document());
  cursor.setPosition(nPos-sOld.length(), QTextCursor::MoveAnchor);

  while (!cursor.atEnd()) {
    QCoreApplication::processEvents();
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
    QString word = cursor.selectedText();

    // Workaround for better recognition of words
    // punctuation etc. does not belong to words
    while (!word.isEmpty() &&
           !word.at(0).isLetter() &&
           cursor.anchor() < cursor.position()) {
      int cursorPos = cursor.position();
      cursor.setPosition(cursor.anchor() + 1, QTextCursor::MoveAnchor);
      cursor.setPosition(cursorPos, QTextCursor::KeepAnchor);
      word = cursor.selectedText();
    }

    if (word == sOld) {
      cursor.insertText(sNew);
      QCoreApplication::processEvents();
    }
    cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
  }
}
