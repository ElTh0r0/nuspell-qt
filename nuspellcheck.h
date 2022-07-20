/**
 * \file nuspellcheck.h
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

#ifndef NUSPELLCHECK_H
#define NUSPELLCHECK_H

#include <QString>
#include <QStringList>
#include <nuspell/dictionary.hxx>

class NuspellCheck {
 public:
  NuspellCheck(nuspell::Dictionary *p_Dict, const QString &sUserDictionary);
  ~NuspellCheck();

  auto spell(const QString &sWord) -> bool;
  auto suggest(const QString &sWord) -> QStringList;
  void ignoreWord(const QString &sWord);
  void addToUserWordlist(const QString &sWord);

 private:
  void put_word(const QString &sWord);
  nuspell::Dictionary *m_pDict;
  QString m_sUserDictionary;
  QStringList m_UserWordsList;
};

#endif  // NUSPELLCHECK_H
