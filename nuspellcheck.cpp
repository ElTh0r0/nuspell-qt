/**
 * \file nuspellcheck.cpp
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

#include "./nuspellcheck.h"

#include <QDebug>
#include <QFile>

NuspellCheck::NuspellCheck(nuspell::Dictionary *p_Dict,
                           const QString &sUserDictionary)
    : m_pDict(p_Dict), m_sUserDictionary(sUserDictionary) {
  QFile userDictonaryFile(m_sUserDictionary);
  if (!m_sUserDictionary.isEmpty() && userDictonaryFile.exists()) {
    if (userDictonaryFile.open(QIODevice::ReadOnly)) {
      QTextStream stream(&userDictonaryFile);
      for (QString word = stream.readLine(); !word.isEmpty();
           word = stream.readLine())
        put_word(word);
      userDictonaryFile.close();
    } else {
      qWarning() << "User dictionary in " << m_sUserDictionary
                 << "could not be opened";
    }
  } else {
    qDebug() << "User dictionary not set.";
  }
}

NuspellCheck::~NuspellCheck() {}

auto NuspellCheck::spell(const QString &sWord) -> bool {
  if (!m_UserWordsList.contains(sWord)) {
    return m_pDict->spell(sWord.toStdString());
  }
  return true;
}

auto NuspellCheck::suggest(const QString &sWord) -> QStringList {
  auto sugs = std::vector<std::string>();
  m_pDict->suggest(sWord.toStdString(), sugs);

  QStringList suggestions;
  std::transform(
      sugs.begin(), sugs.end(), std::back_inserter(suggestions),
      [](const std::string &v) { return QString::fromStdString(v); });

  return suggestions;
}

void NuspellCheck::ignoreWord(const QString &sWord) { put_word(sWord); }

void NuspellCheck::put_word(const QString &sWord) { m_UserWordsList << sWord; }

void NuspellCheck::addToUserWordlist(const QString &sWord) {
  put_word(sWord);
  if (!m_sUserDictionary.isEmpty()) {
    QFile userDictonaryFile(m_sUserDictionary);
    if (userDictonaryFile.open(QIODevice::Append)) {
      QTextStream stream(&userDictonaryFile);
      stream << sWord << "\n";
      userDictonaryFile.close();
    } else {
      qWarning() << "User dictionary in " << m_sUserDictionary
                 << "could not be opened for appending a new word";
    }
  } else {
    qDebug() << "User dictionary not set.";
  }
}
