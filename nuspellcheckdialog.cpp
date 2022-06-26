/**
 * \file nuspellcheckdialog.cpp
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

#include "./nuspellcheckdialog.h"
#include "ui_nuspellcheckdialog.h"

#include "./nuspellcheck.h"

NuspellCheckDialog::NuspellCheckDialog(NuspellCheck *pSpellChecker,
                                       QWidget *pParent)
  : QDialog(pParent),
    ui(new Ui::NuspellCheckDialog) {
  ui->setupUi(this);
  m_pSpellChecker = pSpellChecker;

  connect(ui->listWidget, &QListWidget::currentTextChanged,
          ui->ledtReplaceWith, &QLineEdit::setText);

  connect(ui->btnAddToDict, &QPushButton::clicked,
          this, &NuspellCheckDialog::addToDict);
  connect(ui->btnReplaceOnce, &QPushButton::clicked,
          this, &NuspellCheckDialog::replaceOnce);
  connect(ui->btnReplaceAll, &QPushButton::clicked,
          this, &NuspellCheckDialog::replaceAll);
  connect(ui->btnIgnoreOnce, &QPushButton::clicked,
          this, &NuspellCheckDialog::ignoreOnce);
  connect(ui->btnIgnoreAll, &QPushButton::clicked,
          this, &NuspellCheckDialog::ignoreAll);
  connect(ui->btnCancel, &QPushButton::clicked,
          this, &NuspellCheckDialog::reject);
}


NuspellCheckDialog::~NuspellCheckDialog() {
  delete ui;
}


NuspellCheckDialog::SpellCheckAction NuspellCheckDialog::checkWord(
    const QString &sWord) {
  m_sUnkownWord = sWord;
  ui->lblUnknownWord->setText(QStringLiteral("<b>%1</b>").arg(m_sUnkownWord));

  ui->ledtReplaceWith->clear();

  QStringList suggestions = m_pSpellChecker->suggest(sWord);
  ui->listWidget->clear();
  ui->listWidget->addItems(suggestions);

  if (suggestions.count() > 0)
    ui->listWidget->setCurrentRow(0, QItemSelectionModel::Select);

  m_ReturnCode = AbortCheck;
  QDialog::exec();
  return m_ReturnCode;
}


QString NuspellCheckDialog::replacement() const {
  return ui->ledtReplaceWith->text();
}


void NuspellCheckDialog::ignoreOnce() {
  m_ReturnCode = IgnoreOnce;
  accept();
}


void NuspellCheckDialog::ignoreAll() {
  m_pSpellChecker->ignoreWord(m_sUnkownWord);
  m_ReturnCode = IgnoreAll;
  accept();
}


void NuspellCheckDialog::replaceOnce() {
  m_ReturnCode = ReplaceOnce;
  accept();
}


void NuspellCheckDialog::replaceAll() {
  m_ReturnCode = ReplaceAll;
  accept();
}


void NuspellCheckDialog::addToDict() {
  m_pSpellChecker->addToUserWordlist(m_sUnkownWord);
  m_ReturnCode = AddToDict;
  accept();
}
