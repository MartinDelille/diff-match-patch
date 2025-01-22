/*
 * Diff Match and Patch -- Test Harness
 * Copyright 2018 The diff-match-patch Authors.
 * https://github.com/google/diff-match-patch
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Modified by gk017, 2014-08-07
// Original file licensed under the Apache License, Version 2.0
// See the LICENSE file for the original license text.

// Code known to compile and run with Qt 4.3 through Qt 4.7.
#include <QtCore>
#include <QtTest/QtTest>

#include "diff_match_patch.h"
#include "diff_match_patch_test.h"

int main(int argc, char **argv) {
  diff_match_patch_test dmp_test;
  qDebug("Starting diff_match_patch unit tests.");
  int result = QTest::qExec(&dmp_test, argc, argv);
  qDebug("Done.");
  return result;
  Q_UNUSED(argc)
  Q_UNUSED(argv)
}

//  DIFF TEST FUNCTIONS

void diff_match_patch_test::testDiffCommonPrefix() {
  // Null case
  QCOMPARE(dmp.diff_commonPrefix("abc", "xyz"), 0);

  // Non null case
  QCOMPARE(dmp.diff_commonPrefix("1234abcdef", "1234xyz"), 4);

  // Whole case
  QCOMPARE(dmp.diff_commonPrefix("1234", "1234xyz"), 4);
}

void diff_match_patch_test::testDiffCommonSuffix() {
  // Null case
  QCOMPARE(dmp.diff_commonSuffix("abc", "xyz"), 0);

  // Non null case
  QCOMPARE(dmp.diff_commonSuffix("abcdef1234", "xyz1234"), 4);

  // Whole case
  QCOMPARE(dmp.diff_commonSuffix("1234", "xyz1234"), 4);
}

void diff_match_patch_test::testDiffCommonOverlap() {
  // Null case
  QCOMPARE(dmp.diff_commonOverlap("", "abcd"), 0);

  // Whole case
  QCOMPARE(dmp.diff_commonOverlap("abc", "abcd"), 3);

  // No overlap
  QCOMPARE(dmp.diff_commonOverlap("123456", "abcd"), 0);

  // Overlap
  QCOMPARE(dmp.diff_commonOverlap("123456xxx", "xxxabcd"), 3);

  // Some overly clever languages (C#) may treat ligatures as equal to their
  // component letters.  E.g. U+FB01 == 'fi'
  QCOMPARE(dmp.diff_commonOverlap(
               "fi", QString::fromWCharArray((const wchar_t *)L"\ufb01i", 2)),
           0);
}

void diff_match_patch_test::testDiffHalfmatch() {
  // Detect a halfmatch.
  dmp.Diff_Timeout = 1;
  // No match #1
  QVERIFY(dmp.diff_halfMatch("1234567890", "abcdef").isEmpty());

  // No match #2
  QVERIFY(dmp.diff_halfMatch("12345", "23").isEmpty());

  // Single Match #1
  QCOMPARE(dmp.diff_halfMatch("1234567890", "a345678z"),
           QString("12,90,a,z,345678").split(","));

  // Single Match #2
  QCOMPARE(dmp.diff_halfMatch("a345678z", "1234567890"),
           QString("a,z,12,90,345678").split(","));

  // Single Match #3
  QCOMPARE(dmp.diff_halfMatch("abc56789z", "1234567890"),
           QString("abc,z,1234,0,56789").split(","));

  // Single Match #4
  QCOMPARE(dmp.diff_halfMatch("a23456xyz", "1234567890"),
           QString("a,xyz,1,7890,23456").split(","));

  // Multiple Matches #1
  QCOMPARE(dmp.diff_halfMatch("121231234123451234123121", "a1234123451234z"),
           QString("12123,123121,a,z,1234123451234").split(","));

  // Multiple Matches #2
  QCOMPARE(dmp.diff_halfMatch("x-=-=-=-=-=-=-=-=-=-=-=-=", "xx-=-=-=-=-=-=-="),
           QString(",-=-=-=-=-=,x,,x-=-=-=-=-=-=-=").split(","));

  // Multiple Matches #3
  QCOMPARE(dmp.diff_halfMatch("-=-=-=-=-=-=-=-=-=-=-=-=y", "-=-=-=-=-=-=-=yy"),
           QString("-=-=-=-=-=,,,y,-=-=-=-=-=-=-=y").split(","));

  // Optimal diff would be -q+x=H-i+e=lloHe+Hu=llo-Hew+y not
  // -qHillo+x=HelloHe-w+Hulloy
  QCOMPARE(dmp.diff_halfMatch("qHilloHelloHew", "xHelloHeHulloy"),
           QString("qHillo,w,x,Hulloy,HelloHe").split(","));

  dmp.Diff_Timeout = 0;
  // Optimal no halfmatch
  QVERIFY(dmp.diff_halfMatch("qHilloHelloHew", "xHelloHeHulloy").isEmpty());
}

void diff_match_patch_test::testDiffLinesToChars() {
  // Convert lines down to characters.
  QStringList tmpVector;
  QList<QVariant> tmpVarList;
  tmpVector.append("");
  tmpVector.append("alpha\n");
  tmpVector.append("beta\n");
  tmpVarList << QVariant::fromValue(
      QString() + QChar((ushort)1) + QChar((ushort)2) +
      QChar((ushort)1));  //(("\u0001\u0002\u0001"));
  tmpVarList << QVariant::fromValue(
      QString() + QChar((ushort)2) + QChar((ushort)1) +
      QChar((ushort)2));  // (("\u0002\u0001\u0002"));
  tmpVarList << QVariant::fromValue(tmpVector);
  QCOMPARE(dmp.diff_linesToChars("alpha\nbeta\nalpha\n", "beta\nalpha\nbeta\n"),
           tmpVarList);

  tmpVector.clear();
  tmpVarList.clear();
  tmpVector.append("");
  tmpVector.append("alpha\r\n");
  tmpVector.append("beta\r\n");
  tmpVector.append("\r\n");
  tmpVarList << QVariant::fromValue(QString(""));
  tmpVarList << QVariant::fromValue(
      QString() + QChar((ushort)1) + QChar((ushort)2) + QChar((ushort)3) +
      QChar((ushort)3));  // (("\u0001\u0002\u0003\u0003"));
  tmpVarList << QVariant::fromValue(tmpVector);
  QCOMPARE(dmp.diff_linesToChars("", "alpha\r\nbeta\r\n\r\n\r\n"), tmpVarList);

  tmpVector.clear();
  tmpVarList.clear();
  tmpVector.append("");
  tmpVector.append("a");
  tmpVector.append("b");
  tmpVarList << QVariant::fromValue(QString() +
                                    QChar((ushort)1));  // (("\u0001"));
  tmpVarList << QVariant::fromValue(QString() +
                                    QChar((ushort)2));  // (("\u0002"));
  tmpVarList << QVariant::fromValue(tmpVector);
  QCOMPARE(dmp.diff_linesToChars("a", "b"), tmpVarList);

  // More than 256 to reveal any 8-bit limitations.
  int n = 300;
  tmpVector.clear();
  tmpVarList.clear();
  QString lines;
  QString chars;
  for (int x = 1; x < n + 1; x++) {
    tmpVector.append(QString::number(x) + "\n");
    lines += QString::number(x) + "\n";
    chars += QChar(static_cast<ushort>(x));
  }
  // More than 256 (setup)
  QCOMPARE(tmpVector.size(), n);
  QCOMPARE(chars.length(), n);
  tmpVector.prepend("");
  tmpVarList << QVariant::fromValue(chars);
  tmpVarList << QVariant::fromValue(QString(""));
  tmpVarList << QVariant::fromValue(tmpVector);
  // More than 256
  QCOMPARE(dmp.diff_linesToChars(lines, ""), tmpVarList);
}

void diff_match_patch_test::testDiffCharsToLines() {
  // First check that Diff equality works.
  QVERIFY(Diff(Equal, "a") == Diff(Equal, "a"));
  QCOMPARE(Diff(Equal, "a"), Diff(Equal, "a"));

  // Convert chars up to lines.
  QList<Diff> diffs;
  diffs << Diff(Equal, QString() + QChar((ushort)1) + QChar((ushort)2) +
                           QChar((ushort)1));  // ("\u0001\u0002\u0001");
  diffs << Diff(Insert, QString() + QChar((ushort)2) + QChar((ushort)1) +
                            QChar((ushort)2));  // ("\u0002\u0001\u0002");
  QStringList tmpVector;
  tmpVector.append("");
  tmpVector.append("alpha\n");
  tmpVector.append("beta\n");
  dmp.diff_charsToLines(diffs, tmpVector);
  QCOMPARE(diffs, QList<Diff>() << Diff(Equal, "alpha\nbeta\nalpha\n")
                                << Diff(Insert, "beta\nalpha\nbeta\n"));

  // More than 256 to reveal any 8-bit limitations.
  int n = 300;
  tmpVector.clear();
  QString lines;
  QString chars;
  for (int x = 1; x < n + 1; x++) {
    tmpVector.append(QString::number(x) + "\n");
    lines += QString::number(x) + "\n";
    chars += QChar(static_cast<ushort>(x));
  }
  // More than 256 (setup)
  QCOMPARE(tmpVector.size(), n);
  QCOMPARE(chars.length(), n);
  tmpVector.prepend("");
  diffs = {Diff(Delete, chars)};
  dmp.diff_charsToLines(diffs, tmpVector);
  // More than 256
  QCOMPARE(diffs, QList<Diff>() << Diff(Delete, lines));
}

void diff_match_patch_test::testDiffCleanupMerge() {
  // Cleanup a messy diff.
  QList<Diff> diffs;
  dmp.diff_cleanupMerge(diffs);
  // Null case
  QCOMPARE(diffs, QList<Diff>());

  diffs = QList<Diff>() << Diff(Equal, "a") << Diff(Delete, "b")
                        << Diff(Insert, "c");
  dmp.diff_cleanupMerge(diffs);
  // No change case
  QCOMPARE(diffs, QList<Diff>() << Diff(Equal, "a") << Diff(Delete, "b")
                                << Diff(Insert, "c"));

  diffs = QList<Diff>() << Diff(Equal, "a") << Diff(Equal, "b")
                        << Diff(Equal, "c");
  dmp.diff_cleanupMerge(diffs);
  // Merge equalities
  QCOMPARE(diffs, QList<Diff>() << Diff(Equal, "abc"));

  diffs = QList<Diff>() << Diff(Delete, "a") << Diff(Delete, "b")
                        << Diff(Delete, "c");
  dmp.diff_cleanupMerge(diffs);
  // Merge deletions
  QCOMPARE(diffs, QList<Diff>() << Diff(Delete, "abc"));

  diffs = QList<Diff>() << Diff(Insert, "a") << Diff(Insert, "b")
                        << Diff(Insert, "c");
  dmp.diff_cleanupMerge(diffs);
  // Merge insertions
  QCOMPARE(diffs, QList<Diff>() << Diff(Insert, "abc"));

  diffs = QList<Diff>() << Diff(Delete, "a") << Diff(Insert, "b")
                        << Diff(Delete, "c") << Diff(Insert, "d")
                        << Diff(Equal, "e") << Diff(Equal, "f");
  dmp.diff_cleanupMerge(diffs);
  // Merge interweave
  QCOMPARE(diffs, QList<Diff>() << Diff(Delete, "ac") << Diff(Insert, "bd")
                                << Diff(Equal, "ef"));

  diffs = QList<Diff>() << Diff(Delete, "a") << Diff(Insert, "abc")
                        << Diff(Delete, "dc");
  dmp.diff_cleanupMerge(diffs);
  // Prefix and suffix detection.
  QCOMPARE(diffs, QList<Diff>() << Diff(Equal, "a") << Diff(Delete, "d")
                                << Diff(Insert, "b") << Diff(Equal, "c"));

  diffs = QList<Diff>() << Diff(Equal, "x") << Diff(Delete, "a")
                        << Diff(Insert, "abc") << Diff(Delete, "dc")
                        << Diff(Equal, "y");
  dmp.diff_cleanupMerge(diffs);
  // Prefix and suffix detection with equalities.
  QCOMPARE(diffs, QList<Diff>() << Diff(Equal, "xa") << Diff(Delete, "d")
                                << Diff(Insert, "b") << Diff(Equal, "cy"));

  diffs = QList<Diff>() << Diff(Equal, "a") << Diff(Insert, "ba")
                        << Diff(Equal, "c");
  dmp.diff_cleanupMerge(diffs);
  // Slide edit left
  QCOMPARE(diffs, QList<Diff>() << Diff(Insert, "ab") << Diff(Equal, "ac"));

  diffs = QList<Diff>() << Diff(Equal, "c") << Diff(Insert, "ab")
                        << Diff(Equal, "a");
  dmp.diff_cleanupMerge(diffs);
  // Slide edit right
  QCOMPARE(diffs, QList<Diff>() << Diff(Equal, "ca") << Diff(Insert, "ba"));

  diffs = QList<Diff>() << Diff(Equal, "a") << Diff(Delete, "b")
                        << Diff(Equal, "c") << Diff(Delete, "ac")
                        << Diff(Equal, "x");
  dmp.diff_cleanupMerge(diffs);
  // Slide edit left recursive
  QCOMPARE(diffs, QList<Diff>() << Diff(Delete, "abc") << Diff(Equal, "acx"));

  diffs = QList<Diff>() << Diff(Equal, "x") << Diff(Delete, "ca")
                        << Diff(Equal, "c") << Diff(Delete, "b")
                        << Diff(Equal, "a");
  dmp.diff_cleanupMerge(diffs);
  // Slide edit right recursive
  QCOMPARE(diffs, QList<Diff>() << Diff(Equal, "xca") << Diff(Delete, "cba"));
}

void diff_match_patch_test::testDiffCleanupSemanticLossless() {
  // Slide diffs to match logical boundaries.
  QList<Diff> diffs = QList<Diff>();
  dmp.diff_cleanupSemanticLossless(diffs);
  // Null case
  QCOMPARE(diffs, QList<Diff>());

  diffs = QList<Diff>() << Diff(Equal, "AAA\r\n\r\nBBB")
                        << Diff(Insert, "\r\nDDD\r\n\r\nBBB")
                        << Diff(Equal, "\r\nEEE");
  dmp.diff_cleanupSemanticLossless(diffs);
  // Blank lines
  QCOMPARE(diffs, QList<Diff>() << Diff(Equal, "AAA\r\n\r\n")
                                << Diff(Insert, "BBB\r\nDDD\r\n\r\n")
                                << Diff(Equal, "BBB\r\nEEE"));

  diffs = QList<Diff>() << Diff(Equal, "AAA\r\nBBB")
                        << Diff(Insert, " DDD\r\nBBB") << Diff(Equal, " EEE");
  dmp.diff_cleanupSemanticLossless(diffs);
  // Line boundaries
  QCOMPARE(diffs, QList<Diff>()
                      << Diff(Equal, "AAA\r\n") << Diff(Insert, "BBB DDD\r\n")
                      << Diff(Equal, "BBB EEE"));

  diffs = QList<Diff>() << Diff(Equal, "The c") << Diff(Insert, "ow and the c")
                        << Diff(Equal, "at.");
  dmp.diff_cleanupSemanticLossless(diffs);
  // Word boundaries
  QCOMPARE(diffs, QList<Diff>()
                      << Diff(Equal, "The ") << Diff(Insert, "cow and the ")
                      << Diff(Equal, "cat."));

  diffs = QList<Diff>() << Diff(Equal, "The-c") << Diff(Insert, "ow-and-the-c")
                        << Diff(Equal, "at.");
  dmp.diff_cleanupSemanticLossless(diffs);
  // Alphanumeric boundaries
  QCOMPARE(diffs, QList<Diff>()
                      << Diff(Equal, "The-") << Diff(Insert, "cow-and-the-")
                      << Diff(Equal, "cat."));

  diffs = QList<Diff>() << Diff(Equal, "a") << Diff(Delete, "a")
                        << Diff(Equal, "ax");
  dmp.diff_cleanupSemanticLossless(diffs);
  // Hitting the start.
  QCOMPARE(diffs, QList<Diff>() << Diff(Delete, "a") << Diff(Equal, "aax"));

  diffs = QList<Diff>() << Diff(Equal, "xa") << Diff(Delete, "a")
                        << Diff(Equal, "a");
  dmp.diff_cleanupSemanticLossless(diffs);
  // Hitting the end.
  QCOMPARE(diffs, QList<Diff>() << Diff(Equal, "xaa") << Diff(Delete, "a"));

  diffs = QList<Diff>() << Diff(Equal, "The xxx. The ")
                        << Diff(Insert, "zzz. The ") << Diff(Equal, "yyy.");
  dmp.diff_cleanupSemanticLossless(diffs);
  // Sentence boundaries.
  QCOMPARE(diffs, QList<Diff>()
                      << Diff(Equal, "The xxx.") << Diff(Insert, " The zzz.")
                      << Diff(Equal, " The yyy."));
}

void diff_match_patch_test::testDiffCleanupSemantic() {
  // Cleanup semantically trivial equalities.
  QList<Diff> diffs = QList<Diff>();
  dmp.diff_cleanupSemantic(diffs);
  // Null case
  QCOMPARE(diffs, QList<Diff>());

  diffs = QList<Diff>() << Diff(Delete, "ab") << Diff(Insert, "cd")
                        << Diff(Equal, "12") << Diff(Delete, "e");
  dmp.diff_cleanupSemantic(diffs);
  // No elimination #1
  QCOMPARE(diffs, QList<Diff>() << Diff(Delete, "ab") << Diff(Insert, "cd")
                                << Diff(Equal, "12") << Diff(Delete, "e"));

  diffs = QList<Diff>() << Diff(Delete, "abc") << Diff(Insert, "ABC")
                        << Diff(Equal, "1234") << Diff(Delete, "wxyz");
  dmp.diff_cleanupSemantic(diffs);
  // No elimination #2
  QCOMPARE(diffs, QList<Diff>() << Diff(Delete, "abc") << Diff(Insert, "ABC")
                                << Diff(Equal, "1234") << Diff(Delete, "wxyz"));

  diffs = QList<Diff>() << Diff(Delete, "a") << Diff(Equal, "b")
                        << Diff(Delete, "c");
  dmp.diff_cleanupSemantic(diffs);
  // Simple elimination
  QCOMPARE(diffs, QList<Diff>() << Diff(Delete, "abc") << Diff(Insert, "b"));

  diffs = QList<Diff>() << Diff(Delete, "ab") << Diff(Equal, "cd")
                        << Diff(Delete, "e") << Diff(Equal, "f")
                        << Diff(Insert, "g");
  dmp.diff_cleanupSemantic(diffs);
  // Backpass elimination
  QCOMPARE(diffs, QList<Diff>()
                      << Diff(Delete, "abcdef") << Diff(Insert, "cdfg"));

  diffs = QList<Diff>() << Diff(Insert, "1") << Diff(Equal, "A")
                        << Diff(Delete, "B") << Diff(Insert, "2")
                        << Diff(Equal, "_") << Diff(Insert, "1")
                        << Diff(Equal, "A") << Diff(Delete, "B")
                        << Diff(Insert, "2");
  dmp.diff_cleanupSemantic(diffs);
  // Multiple eliminations
  QCOMPARE(diffs, QList<Diff>()
                      << Diff(Delete, "AB_AB") << Diff(Insert, "1A2_1A2"));

  diffs = QList<Diff>() << Diff(Equal, "The c") << Diff(Delete, "ow and the c")
                        << Diff(Equal, "at.");
  dmp.diff_cleanupSemantic(diffs);
  // Word boundaries
  QCOMPARE(diffs, QList<Diff>()
                      << Diff(Equal, "The ") << Diff(Delete, "cow and the ")
                      << Diff(Equal, "cat."));

  diffs = QList<Diff>() << Diff(Delete, "abcxx") << Diff(Insert, "xxdef");
  dmp.diff_cleanupSemantic(diffs);
  // No overlap elimination
  QCOMPARE(diffs, QList<Diff>()
                      << Diff(Delete, "abcxx") << Diff(Insert, "xxdef"));

  diffs = QList<Diff>() << Diff(Delete, "abcxxx") << Diff(Insert, "xxxdef");
  dmp.diff_cleanupSemantic(diffs);
  // Overlap elimination
  QCOMPARE(diffs, QList<Diff>() << Diff(Delete, "abc") << Diff(Equal, "xxx")
                                << Diff(Insert, "def"));

  diffs = QList<Diff>() << Diff(Delete, "xxxabc") << Diff(Insert, "defxxx");
  dmp.diff_cleanupSemantic(diffs);
  // Reverse overlap elimination
  QCOMPARE(diffs, QList<Diff>() << Diff(Insert, "def") << Diff(Equal, "xxx")
                                << Diff(Delete, "abc"));

  diffs = QList<Diff>() << Diff(Delete, "abcd1212") << Diff(Insert, "1212efghi")
                        << Diff(Equal, "----") << Diff(Delete, "A3")
                        << Diff(Insert, "3BC");
  dmp.diff_cleanupSemantic(diffs);
  // Two overlap eliminations
  QCOMPARE(diffs, QList<Diff>() << Diff(Delete, "abcd") << Diff(Equal, "1212")
                                << Diff(Insert, "efghi") << Diff(Equal, "----")
                                << Diff(Delete, "A") << Diff(Equal, "3")
                                << Diff(Insert, "BC"));
}

void diff_match_patch_test::testDiffCleanupEfficiency() {
  // Cleanup operationally trivial equalities.
  dmp.Diff_EditCost = 4;
  QList<Diff> diffs = QList<Diff>();
  dmp.diff_cleanupEfficiency(diffs);
  // Null case
  QCOMPARE(diffs, QList<Diff>());

  diffs = QList<Diff>() << Diff(Delete, "ab") << Diff(Insert, "12")
                        << Diff(Equal, "wxyz") << Diff(Delete, "cd")
                        << Diff(Insert, "34");
  dmp.diff_cleanupEfficiency(diffs);
  // No elimination
  QCOMPARE(diffs, QList<Diff>() << Diff(Delete, "ab") << Diff(Insert, "12")
                                << Diff(Equal, "wxyz") << Diff(Delete, "cd")
                                << Diff(Insert, "34"));

  diffs = QList<Diff>() << Diff(Delete, "ab") << Diff(Insert, "12")
                        << Diff(Equal, "xyz") << Diff(Delete, "cd")
                        << Diff(Insert, "34");
  dmp.diff_cleanupEfficiency(diffs);
  // Four-edit elimination
  QCOMPARE(diffs, QList<Diff>()
                      << Diff(Delete, "abxyzcd") << Diff(Insert, "12xyz34"));

  diffs = QList<Diff>() << Diff(Insert, "12") << Diff(Equal, "x")
                        << Diff(Delete, "cd") << Diff(Insert, "34");
  dmp.diff_cleanupEfficiency(diffs);
  // Three-edit elimination
  QCOMPARE(diffs, QList<Diff>()
                      << Diff(Delete, "xcd") << Diff(Insert, "12x34"));

  diffs = QList<Diff>() << Diff(Delete, "ab") << Diff(Insert, "12")
                        << Diff(Equal, "xy") << Diff(Insert, "34")
                        << Diff(Equal, "z") << Diff(Delete, "cd")
                        << Diff(Insert, "56");
  dmp.diff_cleanupEfficiency(diffs);
  // Backpass elimination
  QCOMPARE(diffs, QList<Diff>()
                      << Diff(Delete, "abxyzcd") << Diff(Insert, "12xy34z56"));

  dmp.Diff_EditCost = 5;
  diffs = QList<Diff>() << Diff(Delete, "ab") << Diff(Insert, "12")
                        << Diff(Equal, "wxyz") << Diff(Delete, "cd")
                        << Diff(Insert, "34");
  dmp.diff_cleanupEfficiency(diffs);
  // High cost elimination
  QCOMPARE(diffs, QList<Diff>()
                      << Diff(Delete, "abwxyzcd") << Diff(Insert, "12wxyz34"));
  dmp.Diff_EditCost = 4;
}

void diff_match_patch_test::testDiffPrettyHtml() {
  // Pretty print.
  QList<Diff> diffs = QList<Diff>()
                      << Diff(Equal, "a\n") << Diff(Delete, "<B>b</B>")
                      << Diff(Insert, "c&d");
  QString expectedHtml =
      "<span>a&para;<br></span><del "
      "style=\"background:#ffe6e6;\">&lt;B&gt;b&lt;/B&gt;</del><ins "
      "style=\"background:#e6ffe6;\">c&amp;d</ins>";
  QCOMPARE(dmp.diff_prettyHtml(diffs), expectedHtml);
}

void diff_match_patch_test::testDiffText() {
  // Compute the source and destination texts.
  QList<Diff> diffs = QList<Diff>()
                      << Diff(Equal, "jump") << Diff(Delete, "s")
                      << Diff(Insert, "ed") << Diff(Equal, " over ")
                      << Diff(Delete, "the") << Diff(Insert, "a")
                      << Diff(Equal, " lazy");

  QCOMPARE(dmp.diff_text1(diffs), QString("jumps over the lazy"));
  QCOMPARE(dmp.diff_text2(diffs), QString("jumped over a lazy"));
}

void diff_match_patch_test::testDiffDelta() {
  // Convert a diff into delta string.
  QList<Diff> diffs = QList<Diff>()
                      << Diff(Equal, "jump") << Diff(Delete, "s")
                      << Diff(Insert, "ed") << Diff(Equal, " over ")
                      << Diff(Delete, "the") << Diff(Insert, "a")
                      << Diff(Equal, " lazy") << Diff(Insert, "old dog");
  QString text1 = dmp.diff_text1(diffs);
  // Base text.
  QCOMPARE(text1, QString("jumps over the lazy"));

  QString delta = dmp.diff_toDelta(diffs);
  QCOMPARE(delta, QString("=4\t-1\t+ed\t=6\t-3\t+a\t=5\t+old dog"));

  // Convert delta string into a diff.
  QCOMPARE(dmp.diff_fromDelta(text1, delta), diffs);

  // Generates error (19 < 20).
  try {
    dmp.diff_fromDelta(text1 + "x", delta);
    QFAIL("diff_fromDelta: Too long.");
  } catch (QString ex) {
    // Exception expected.
  }

  // Generates error (19 > 18).
  try {
    dmp.diff_fromDelta(text1.mid(1), delta);
    QFAIL("diff_fromDelta: Too short.");
  } catch (QString ex) {
    // Exception expected.
  }

  // Test deltas with special characters.
  diffs =
      QList<Diff>()
      << Diff(Equal,
              QString::fromWCharArray((const wchar_t *)L"\u0680 \000 \t %", 7))
      << Diff(Delete,
              QString::fromWCharArray((const wchar_t *)L"\u0681 \001 \n ^", 7))
      << Diff(Insert,
              QString::fromWCharArray((const wchar_t *)L"\u0682 \002 \\ |", 7));
  text1 = dmp.diff_text1(diffs);
  // Unicode text.
  QCOMPARE(text1,
           QString::fromWCharArray(
               (const wchar_t *)L"\u0680 \000 \t %\u0681 \001 \n ^", 14));

  delta = dmp.diff_toDelta(diffs);
  // Unicode delta.
  QCOMPARE(delta, QString("=7\t-7\t+%DA%82 %02 %5C %7C"));

  // Unicode.
  QCOMPARE(dmp.diff_fromDelta(text1, delta), diffs);

  // Verify pool of unchanged characters.
  diffs = QList<Diff>() << Diff(
              Insert, "A-Z a-z 0-9 - _ . ! ~ * ' ( ) ; / ? : @ & = + $ , # ");
  QString text2 = dmp.diff_text2(diffs);
  // Unchanged characters.
  QCOMPARE(text2,
           QString("A-Z a-z 0-9 - _ . ! ~ * ' ( ) ; / ? : @ & = + $ , # "));

  delta = dmp.diff_toDelta(diffs);
  // Unchanged characters
  QCOMPARE(delta,
           QString("+A-Z a-z 0-9 - _ . ! ~ * ' ( ) ; / ? : @ & = + $ , # "));

  // Convert delta string into a diff.
  QCOMPARE(dmp.diff_fromDelta("", delta), diffs);
}

void diff_match_patch_test::testDiffXIndex() {
  // Translate a location in text1 to text2.
  QList<Diff> diffs = {Diff(Delete, "a"), Diff(Insert, "1234"),
                       Diff(Equal, "xyz")};
  // Translation on equality.
  QCOMPARE(dmp.diff_xIndex(diffs, 2), 5);

  diffs = {Diff(Equal, "a"), Diff(Delete, "1234"), Diff(Equal, "xyz")};
  // Translation on deletion.
  QCOMPARE(dmp.diff_xIndex(diffs, 3), 1);
}

void diff_match_patch_test::testDiffLevenshtein() {
  QList<Diff> diffs = {Diff(Delete, "abc"), Diff(Insert, "1234"),
                       Diff(Equal, "xyz")};
  // Trailling equality.
  QCOMPARE(dmp.diff_levenshtein(diffs), 4);

  diffs = {Diff(Equal, "xyz"), Diff(Delete, "abc"), Diff(Insert, "1234")};
  // Leading equality.
  QCOMPARE(dmp.diff_levenshtein(diffs), 4);

  diffs = {Diff(Delete, "abc"), Diff(Equal, "xyz"), Diff(Insert, "1234")};
  // Middle equality.
  QCOMPARE(dmp.diff_levenshtein(diffs), 7);
}

void diff_match_patch_test::testDiffBisect() {
  // Normal.
  QString a = "cat";
  QString b = "map";
  // Since the resulting diff hasn't been normalized, it would be ok if
  // the insertion and deletion pairs are swapped.
  // If the order changes, tweak this test as required.
  QList<Diff> diffs = {Diff(Delete, "c"), Diff(Insert, "m"), Diff(Equal, "a"),
                       Diff(Delete, "t"), Diff(Insert, "p")};
  // Normal.
  QCOMPARE(dmp.diff_bisect(a, b, std::numeric_limits<clock_t>::max()), diffs);

  // Timeout.
  diffs = {Diff(Delete, "cat"), Diff(Insert, "map")};
  QCOMPARE(dmp.diff_bisect(a, b, 0), diffs);
}

void diff_match_patch_test::testDiffMain() {
  // Perform a trivial diff.
  QList<Diff> diffs;
  // Null case.
  QCOMPARE(dmp.diff_main("", "", false), diffs);

  diffs = {Diff(Equal, "abc")};
  // Equality.
  QCOMPARE(dmp.diff_main("abc", "abc", false), diffs);

  diffs = {Diff(Equal, "ab"), Diff(Insert, "123"), Diff(Equal, "c")};
  // Simple insertion.
  QCOMPARE(dmp.diff_main("abc", "ab123c", false), diffs);

  diffs = {Diff(Equal, "a"), Diff(Delete, "123"), Diff(Equal, "bc")};
  // Simple deletion.
  QCOMPARE(dmp.diff_main("a123bc", "abc", false), diffs);

  diffs = {Diff(Equal, "a"), Diff(Insert, "123"), Diff(Equal, "b"),
           Diff(Insert, "456"), Diff(Equal, "c")};
  // Two insertions.
  QCOMPARE(dmp.diff_main("abc", "a123b456c", false), diffs);

  diffs = {Diff(Equal, "a"), Diff(Delete, "123"), Diff(Equal, "b"),
           Diff(Delete, "456"), Diff(Equal, "c")};
  // Two deletions.
  QCOMPARE(dmp.diff_main("a123b456c", "abc", false), diffs);

  // Perform a real diff.
  // Switch off the timeout.
  dmp.Diff_Timeout = 0;
  diffs = {Diff(Delete, "a"), Diff(Insert, "b")};
  QCOMPARE(dmp.diff_main("a", "b", false), diffs);

  diffs = {Diff(Delete, "Apple"), Diff(Insert, "Banana"),
           Diff(Equal, "s are a"), Diff(Insert, "lso"), Diff(Equal, " fruit.")};
  QCOMPARE(
      dmp.diff_main("Apples are a fruit.", "Bananas are also fruit.", false),
      diffs);

  diffs = {Diff(Delete, "a"),
           Diff(Insert, QString::fromWCharArray((const wchar_t *)L"\u0680", 1)),
           Diff(Equal, "x"), Diff(Delete, "\t"),
           Diff(Insert, QString::fromWCharArray((const wchar_t *)L"\000", 1))};
  QCOMPARE(
      dmp.diff_main("ax\t",
                    QString::fromWCharArray((const wchar_t *)L"\u0680x\000", 3),
                    false),
      diffs);

  diffs = {Diff(Delete, "1"), Diff(Equal, "a"),  Diff(Delete, "y"),
           Diff(Equal, "b"),  Diff(Delete, "2"), Diff(Insert, "xab")};
  QCOMPARE(dmp.diff_main("1ayb2", "abxab", false), diffs);

  diffs = {Diff(Insert, "xaxcx"), Diff(Equal, "abc"), Diff(Delete, "y")};
  QCOMPARE(dmp.diff_main("abcy", "xaxcxabc", false), diffs);

  diffs = {Diff(Delete, "ABCD"),
           Diff(Equal, "a"),
           Diff(Delete, "="),
           Diff(Insert, "-"),
           Diff(Equal, "bcd"),
           Diff(Delete, "="),
           Diff(Insert, "-"),
           Diff(Equal, "efghijklmnopqrs"),
           Diff(Delete, "EFGHIJKLMNOefg")};
  QCOMPARE(dmp.diff_main("ABCDa=bcd=efghijklmnopqrsEFGHIJKLMNOefg",
                         "a-bcd-efghijklmnopqrs", false),
           diffs);

  diffs = {Diff(Insert, " "), Diff(Equal, "a"), Diff(Insert, "nd"),
           Diff(Equal, " [[Pennsylvania]]"), Diff(Delete, " and [[New")};
  QCOMPARE(dmp.diff_main("a [[Pennsylvania]] and [[New",
                         " and [[Pennsylvania]]", false),
           diffs);

  dmp.Diff_Timeout = 0.1f;  // 100ms
  // This test may 'fail' on extremely fast computers.  If so, just increase the
  // text lengths.
  QString a =
      "`Twas brillig, and the slithy toves\nDid gyre and gimble in the "
      "wabe:\nAll mimsy were the borogoves,\nAnd the mome raths outgrabe.\n";
  QString b =
      "I am the very model of a modern major general,\nI've information "
      "vegetable, animal, and mineral,\nI know the kings of England, and I "
      "quote the fights historical,\nFrom Marathon to Waterloo, in order "
      "categorical.\n";
  // Increase the text lengths by 1024 times to ensure a timeout.
  for (int x = 0; x < 10; x++) {
    a = a + a;
    b = b + b;
  }
  clock_t startTime = clock();
  dmp.diff_main(a, b);
  clock_t endTime = clock();
  QVERIFY(dmp.Diff_Timeout * CLOCKS_PER_SEC <= endTime - startTime);
  QVERIFY(dmp.Diff_Timeout * CLOCKS_PER_SEC * 2 > endTime - startTime);
  dmp.Diff_Timeout = 0;

  // Test the linemode speedup.
  // Must be long to pass the 100 char cutoff.
  a = "1234567890\n1234567890\n1234567890\n1234567890\n1234567890\n1234567890\n"
      "1234567890\n1234567890\n1234567890\n1234567890\n1234567890\n1234567890\n"
      "1234567890\n";
  b = "abcdefghij\nabcdefghij\nabcdefghij\nabcdefghij\nabcdefghij\nabcdefghij\n"
      "abcdefghij\nabcdefghij\nabcdefghij\nabcdefghij\nabcdefghij\nabcdefghij\n"
      "abcdefghij\n";
  QCOMPARE(dmp.diff_main(a, b, true), dmp.diff_main(a, b, false));

  a = "123456789012345678901234567890123456789012345678901234567890123456789012"
      "3456789012345678901234567890123456789012345678901234567890";
  b = "abcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijab"
      "cdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghij";
  QCOMPARE(dmp.diff_main(a, b, true), dmp.diff_main(a, b, false));

  a = "1234567890\n1234567890\n1234567890\n1234567890\n1234567890\n1234567890\n"
      "1234567890\n1234567890\n1234567890\n1234567890\n1234567890\n1234567890\n"
      "1234567890\n";
  b = "abcdefghij\n1234567890\n1234567890\n1234567890\nabcdefghij\n1234567890\n"
      "1234567890\n1234567890\nabcdefghij\n1234567890\n1234567890\n1234567890\n"
      "abcdefghij\n";
  QStringList texts_linemode = diff_rebuildtexts(dmp.diff_main(a, b, true));
  QStringList texts_textmode = diff_rebuildtexts(dmp.diff_main(a, b, false));
  QCOMPARE(texts_textmode, texts_linemode);

  // Test null inputs.
  try {
    dmp.diff_main(NULL, NULL);
    QVERIFY(false);
  } catch (const char *ex) {
    // Exception expected.
  }
}

//  MATCH TEST FUNCTIONS

void diff_match_patch_test::testMatchAlphabet() {
  // Initialise the bitmasks for Bitap.
  QMap<QChar, int> bitmask;
  bitmask.insert('a', 4);
  bitmask.insert('b', 2);
  bitmask.insert('c', 1);
  // Unique.
  QCOMPARE(dmp.match_alphabet("abc"), bitmask);

  bitmask = QMap<QChar, int>();
  bitmask.insert('a', 37);
  bitmask.insert('b', 18);
  bitmask.insert('c', 8);
  // Duplicates.
  QCOMPARE(dmp.match_alphabet("abcaba"), bitmask);
}

void diff_match_patch_test::testMatchBitap() {
  // Bitap algorithm.
  dmp.Match_Distance = 100;
  dmp.Match_Threshold = 0.5f;
  // Exact match #1.
  QCOMPARE(dmp.match_bitap("abcdefghijk", "fgh", 5), 5);

  // Exact match #2.
  QCOMPARE(dmp.match_bitap("abcdefghijk", "fgh", 0), 5);

  // Fuzzy match #1.
  QCOMPARE(dmp.match_bitap("abcdefgh", "efxhi", 0), 4);

  // Fuzzy match #2.
  QCOMPARE(dmp.match_bitap("abcdefghijk", "cdefxyhijk", 5), 2);

  // Fuzzy match #3.
  QCOMPARE(dmp.match_bitap("abcdefghijk", "bxy", 1), -1);

  // Overflow.
  QCOMPARE(dmp.match_bitap("123456789xx0", "3456789x0", 2), 2);

  // Before start match.
  QCOMPARE(dmp.match_bitap("abcdef", "xxabc", 4), 0);

  // Beyond end match.
  QCOMPARE(dmp.match_bitap("abcdef", "defyy", 4), 3);

  // Oversized pattern.
  QCOMPARE(dmp.match_bitap("abcdef", "xabcdefy", 0), 0);

  dmp.Match_Threshold = 0.4f;
  // Threshold #1.
  QCOMPARE(dmp.match_bitap("abcdefghijk", "efxyhi", 1), 4);

  dmp.Match_Threshold = 0.3f;
  // Threshold #2.
  QCOMPARE(dmp.match_bitap("abcdefghijk", "efxyhi", 1), -1);

  dmp.Match_Threshold = 0.0f;
  // Threshold #3.
  QCOMPARE(dmp.match_bitap("abcdefghijk", "bcdef", 1), 1);

  dmp.Match_Threshold = 0.5f;
  // Multiple select #1.
  QCOMPARE(dmp.match_bitap("abcdexyzabcde", "abccde", 3), 3);

  // Multiple select #2.
  QCOMPARE(dmp.match_bitap("abcdexyzabcde", "abccde", 5), 8);

  dmp.Match_Distance = 10;  // Strict location.
  // Distance test #1.
  QCOMPARE(dmp.match_bitap("abcdefghijklmnopqrstuvwxyz", "abcdef", 24), 0);

  // Distance test #2.
  QCOMPARE(dmp.match_bitap("abcdefghijklmnopqrstuvwxyz", "abcdxxefg", 1), 0);

  dmp.Match_Distance = 1000;  // Loose location.
  // Distance test #3.
  QCOMPARE(dmp.match_bitap("abcdefghijklmnopqrstuvwxyz", "abcdefg", 24), 0);
}

/*
void diff_match_patch_test::testMatchMain() {
  // Full match.
  assertEquals("match_main: Equality.", 0,
               dmp.match_main("abcdef", "abcdef", 1000));

  assertEquals("match_main: Null text.", -1, dmp.match_main("", "abcdef", 1));

  assertEquals("match_main: Null pattern.", 3, dmp.match_main("abcdef", "", 3));

  assertEquals("match_main: Exact match.", 3,
               dmp.match_main("abcdef", "de", 3));

  dmp.Match_Threshold = 0.7f;
  assertEquals("match_main: Complex match.", 4,
               dmp.match_main("I am the very model of a modern major general.",
                              " that berry ", 5));
  dmp.Match_Threshold = 0.5f;

  // Test null inputs.
  try {
    dmp.match_main(NULL, NULL, 0);
    assertFalse("match_main: Null inputs.", true);
  } catch (const char *ex) {
    // Exception expected.
  }
}

//  PATCH TEST FUNCTIONS

void diff_match_patch_test::testPatchObj() {
  // Patch Object.
  Patch p;
  p.start1 = 20;
  p.start2 = 21;
  p.length1 = 18;
  p.length2 = 17;
  p.diffs = diffList(Diff(Equal, "jump"), Diff(Delete, "s"), Diff(Insert, "ed"),
                     Diff(Equal, " over "), Diff(Delete, "the"),
                     Diff(Insert, "a"), Diff(Equal, "\nlaz"));
  QString strp =
      "@@ -21,18 +22,17 @@\n jump\n-s\n+ed\n  over \n-the\n+a\n %0Alaz\n";
  assertEquals("Patch: toString.", strp, p.toString());
}

void diff_match_patch_test::testPatchFromText() {
  assertTrue("patch_fromText: #0.", dmp.patch_fromText("").isEmpty());

  QString strp =
      "@@ -21,18 +22,17 @@\n jump\n-s\n+ed\n  over \n-the\n+a\n %0Alaz\n";
  assertEquals("patch_fromText: #1.", strp,
               dmp.patch_fromText(strp).value(0).toString());

  assertEquals("patch_fromText: #2.", "@@ -1 +1 @@\n-a\n+b\n",
               dmp.patch_fromText("@@ -1 +1 @@\n-a\n+b\n").value(0).toString());

  assertEquals(
      "patch_fromText: #3.", "@@ -1,3 +0,0 @@\n-abc\n",
      dmp.patch_fromText("@@ -1,3 +0,0 @@\n-abc\n").value(0).toString());

  assertEquals(
      "patch_fromText: #4.", "@@ -0,0 +1,3 @@\n+abc\n",
      dmp.patch_fromText("@@ -0,0 +1,3 @@\n+abc\n").value(0).toString());

  // Generates error.
  try {
    dmp.patch_fromText("Bad\nPatch\n");
    assertFalse("patch_fromText: #5.", true);
  } catch (QString ex) {
    // Exception expected.
  }
}

void diff_match_patch_test::testPatchToText() {
  QString strp =
      "@@ -21,18 +22,17 @@\n jump\n-s\n+ed\n  over \n-the\n+a\n  laz\n";
  QList<Patch> patches;
  patches = dmp.patch_fromText(strp);
  assertEquals("patch_toText: Single", strp, dmp.patch_toText(patches));

  strp =
      "@@ -1,9 +1,9 @@\n-f\n+F\n oo+fooba\n@@ -7,9 +7,9 @@\n obar\n-,\n+.\n  "
      "tes\n";
  patches = dmp.patch_fromText(strp);
  assertEquals("patch_toText: Dual", strp, dmp.patch_toText(patches));
}

void diff_match_patch_test::testPatchAddContext() {
  dmp.Patch_Margin = 4;
  Patch p;
  p = dmp.patch_fromText("@@ -21,4 +21,10 @@\n-jump\n+somersault\n").value(0);
  dmp.patch_addContext(p, "The quick brown fox jumps over the lazy dog.");
  assertEquals("patch_addContext: Simple case.",
               "@@ -17,12 +17,18 @@\n fox \n-jump\n+somersault\n s ov\n",
               p.toString());

  p = dmp.patch_fromText("@@ -21,4 +21,10 @@\n-jump\n+somersault\n").value(0);
  dmp.patch_addContext(p, "The quick brown fox jumps.");
  assertEquals("patch_addContext: Not enough trailing context.",
               "@@ -17,10 +17,16 @@\n fox \n-jump\n+somersault\n s.\n",
               p.toString());

  p = dmp.patch_fromText("@@ -3 +3,2 @@\n-e\n+at\n").value(0);
  dmp.patch_addContext(p, "The quick brown fox jumps.");
  assertEquals("patch_addContext: Not enough leading context.",
               "@@ -1,7 +1,8 @@\n Th\n-e\n+at\n  qui\n", p.toString());

  p = dmp.patch_fromText("@@ -3 +3,2 @@\n-e\n+at\n").value(0);
  dmp.patch_addContext(
      p, "The quick brown fox jumps.  The quick brown fox crashes.");
  assertEquals("patch_addContext: Ambiguity.",
               "@@ -1,27 +1,28 @@\n Th\n-e\n+at\n  quick brown fox jumps. \n",
               p.toString());
}

void diff_match_patch_test::testPatchMake() {
  QList<Patch> patches;
  patches = dmp.patch_make("", "");
  assertEquals("patch_make: Null case", "", dmp.patch_toText(patches));

  QString text1 = "The quick brown fox jumps over the lazy dog.";
  QString text2 = "That quick brown fox jumped over a lazy dog.";
  QString expectedPatch =
      "@@ -1,8 +1,7 @@\n Th\n-at\n+e\n  qui\n@@ -21,17 +21,18 @@\n "
      "jump\n-ed\n+s\n  over \n-a\n+the\n  laz\n";
  // The second patch must be "-21,17 +21,18", not "-22,17 +21,18" due to
  // rolling context.
  patches = dmp.patch_make(text2, text1);
  assertEquals("patch_make: Text2+Text1 inputs", expectedPatch,
               dmp.patch_toText(patches));

  expectedPatch =
      "@@ -1,11 +1,12 @@\n Th\n-e\n+at\n  quick b\n@@ -22,18 +22,17 @@\n "
      "jump\n-s\n+ed\n  over \n-the\n+a\n  laz\n";
  patches = dmp.patch_make(text1, text2);
  assertEquals("patch_make: Text1+Text2 inputs", expectedPatch,
               dmp.patch_toText(patches));

  QList<Diff> diffs = dmp.diff_main(text1, text2, false);
  patches = dmp.patch_make(diffs);
  assertEquals("patch_make: Diff input", expectedPatch,
               dmp.patch_toText(patches));

  patches = dmp.patch_make(text1, diffs);
  assertEquals("patch_make: Text1+Diff inputs", expectedPatch,
               dmp.patch_toText(patches));

  patches = dmp.patch_make(text1, text2, diffs);
  assertEquals("patch_make: Text1+Text2+Diff inputs (deprecated)",
               expectedPatch, dmp.patch_toText(patches));

  patches = dmp.patch_make("`1234567890-=[]\\;',./", "~!@#$%^&*()_+{}|:\"<>?");
  assertEquals("patch_toText: Character encoding.",
               "@@ -1,21 +1,21 "
               "@@\n-%601234567890-=%5B%5D%5C;',./"
               "\n+~!@#$%25%5E&*()_+%7B%7D%7C:%22%3C%3E?\n",
               dmp.patch_toText(patches));

  diffs = diffList(Diff(Delete, "`1234567890-=[]\\;',./"),
                   Diff(Insert, "~!@#$%^&*()_+{}|:\"<>?"));
  assertEquals("patch_fromText: Character decoding.", diffs,
               dmp.patch_fromText("@@ -1,21 +1,21 "
                                  "@@\n-%601234567890-=%5B%5D%5C;',./"
                                  "\n+~!@#$%25%5E&*()_+%7B%7D%7C:%22%3C%3E?\n")
                   .value(0)
                   .diffs);

  text1 = "";
  for (int x = 0; x < 100; x++) {
    text1 += "abcdef";
  }
  text2 = text1 + "123";
  expectedPatch =
      "@@ -573,28 +573,31 @@\n cdefabcdefabcdefabcdefabcdef\n+123\n";
  patches = dmp.patch_make(text1, text2);
  assertEquals("patch_make: Long string with repeats.", expectedPatch,
               dmp.patch_toText(patches));

  // Test null inputs.
  try {
    dmp.patch_make(nullptr, nullptr);
    assertFalse("patch_make: Null inputs.", true);
  } catch (const char *ex) {
    // Exception expected.
  }
}

void diff_match_patch_test::testPatchSplitMax() {
  // Assumes that Match_MaxBits is 32.
  QList<Patch> patches;
  patches = dmp.patch_make(
      "abcdefghijklmnopqrstuvwxyz01234567890",
      "XabXcdXefXghXijXklXmnXopXqrXstXuvXwxXyzX01X23X45X67X89X0");
  dmp.patch_splitMax(patches);
  assertEquals("patch_splitMax: #1.",
               "@@ -1,32 +1,46 @@\n+X\n ab\n+X\n cd\n+X\n ef\n+X\n gh\n+X\n "
               "ij\n+X\n kl\n+X\n mn\n+X\n op\n+X\n qr\n+X\n st\n+X\n uv\n+X\n "
               "wx\n+X\n yz\n+X\n 012345\n@@ -25,13 +39,18 @@\n zX01\n+X\n "
               "23\n+X\n 45\n+X\n 67\n+X\n 89\n+X\n 0\n",
               dmp.patch_toText(patches));

  patches = dmp.patch_make(
      "abcdef123456789012345678901234567890123456789012345678901234567890123456"
      "7890uvwxyz",
      "abcdefuvwxyz");
  QString oldToText = dmp.patch_toText(patches);
  dmp.patch_splitMax(patches);
  assertEquals("patch_splitMax: #2.", oldToText, dmp.patch_toText(patches));

  patches = dmp.patch_make(
      "1234567890123456789012345678901234567890123456789012345678901234567890",
      "abc");
  dmp.patch_splitMax(patches);
  assertEquals("patch_splitMax: #3.",
               "@@ -1,32 +1,4 @@\n-1234567890123456789012345678\n 9012\n@@ "
               "-29,32 +1,4 @@\n-9012345678901234567890123456\n 7890\n@@ "
               "-57,14 +1,3 @@\n-78901234567890\n+abc\n",
               dmp.patch_toText(patches));

  patches = dmp.patch_make(
      "abcdefghij , h : 0 , t : 1 abcdefghij , h : 0 , t : 1 abcdefghij , h : "
      "0 , t : 1",
      "abcdefghij , h : 1 , t : 1 abcdefghij , h : 1 , t : 1 abcdefghij , h : "
      "0 , t : 1");
  dmp.patch_splitMax(patches);
  assertEquals(
      "patch_splitMax: #4.",
      "@@ -2,32 +2,32 @@\n bcdefghij , h : \n-0\n+1\n  , t : 1 abcdef\n@@ "
      "-29,32 +29,32 @@\n bcdefghij , h : \n-0\n+1\n  , t : 1 abcdef\n",
      dmp.patch_toText(patches));
}

void diff_match_patch_test::testPatchAddPadding() {
  QList<Patch> patches;
  patches = dmp.patch_make("", "test");
  assertEquals("patch_addPadding: Both edges full.", "@@ -0,0 +1,4 @@\n+test\n",
               dmp.patch_toText(patches));
  dmp.patch_addPadding(patches);
  assertEquals("patch_addPadding: Both edges full.",
               "@@ -1,8 +1,12 @@\n %01%02%03%04\n+test\n %01%02%03%04\n",
               dmp.patch_toText(patches));

  patches = dmp.patch_make("XY", "XtestY");
  assertEquals("patch_addPadding: Both edges partial.",
               "@@ -1,2 +1,6 @@\n X\n+test\n Y\n", dmp.patch_toText(patches));
  dmp.patch_addPadding(patches);
  assertEquals("patch_addPadding: Both edges partial.",
               "@@ -2,8 +2,12 @@\n %02%03%04X\n+test\n Y%01%02%03\n",
               dmp.patch_toText(patches));

  patches = dmp.patch_make("XXXXYYYY", "XXXXtestYYYY");
  assertEquals("patch_addPadding: Both edges none.",
               "@@ -1,8 +1,12 @@\n XXXX\n+test\n YYYY\n",
               dmp.patch_toText(patches));
  dmp.patch_addPadding(patches);
  assertEquals("patch_addPadding: Both edges none.",
               "@@ -5,8 +5,12 @@\n XXXX\n+test\n YYYY\n",
               dmp.patch_toText(patches));
}

void diff_match_patch_test::testPatchApply() {
  dmp.Match_Distance = 1000;
  dmp.Match_Threshold = 0.5f;
  dmp.Patch_DeleteThreshold = 0.5f;
  QList<Patch> patches;
  patches = dmp.patch_make("", "");
  QPair<QString, QVector<bool> > results =
      dmp.patch_apply(patches, "Hello world.");
  QVector<bool> boolArray = results.second;

  QString resultStr =
      QString("%1\t%2").arg(results.first).arg(boolArray.count());
  assertEquals("patch_apply: Null case.", "Hello world.\t0", resultStr);

  patches = dmp.patch_make("The quick brown fox jumps over the lazy dog.",
                           "That quick brown fox jumped over a lazy dog.");
  results =
      dmp.patch_apply(patches, "The quick brown fox jumps over the lazy dog.");
  boolArray = results.second;
  resultStr = results.first + "\t" + (boolArray[0] ? "true" : "false") + "\t" +
              (boolArray[1] ? "true" : "false");
  assertEquals("patch_apply: Exact match.",
               "That quick brown fox jumped over a lazy dog.\ttrue\ttrue",
               resultStr);

  results = dmp.patch_apply(patches,
                            "The quick red rabbit jumps over the tired tiger.");
  boolArray = results.second;
  resultStr = results.first + "\t" + (boolArray[0] ? "true" : "false") + "\t" +
              (boolArray[1] ? "true" : "false");
  assertEquals("patch_apply: Partial match.",
               "That quick red rabbit jumped over a tired tiger.\ttrue\ttrue",
               resultStr);

  results = dmp.patch_apply(patches,
                            "I am the very model of a modern major general.");
  boolArray = results.second;
  resultStr = results.first + "\t" + (boolArray[0] ? "true" : "false") + "\t" +
              (boolArray[1] ? "true" : "false");
  assertEquals("patch_apply: Failed match.",
               "I am the very model of a modern major general.\tfalse\tfalse",
               resultStr);

  patches = dmp.patch_make(
      "x1234567890123456789012345678901234567890123456789012345678901234567890"
      "y",
      "xabcy");
  results = dmp.patch_apply(patches,
                            "x123456789012345678901234567890-----++++++++++----"
                            "-123456789012345678901234567890y");
  boolArray = results.second;
  resultStr = results.first + "\t" + (boolArray[0] ? "true" : "false") + "\t" +
              (boolArray[1] ? "true" : "false");
  assertEquals("patch_apply: Big delete, small change.", "xabcy\ttrue\ttrue",
               resultStr);

  patches = dmp.patch_make(
      "x1234567890123456789012345678901234567890123456789012345678901234567890"
      "y",
      "xabcy");
  results = dmp.patch_apply(patches,
                            "x12345678901234567890---------------++++++++++----"
                            "-----------12345678901234567890y");
  boolArray = results.second;
  resultStr = results.first + "\t" + (boolArray[0] ? "true" : "false") + "\t" +
              (boolArray[1] ? "true" : "false");
  assertEquals("patch_apply: Big delete, large change 1.",
               "xabc12345678901234567890---------------++++++++++--------------"
               "-12345678901234567890y\tfalse\ttrue",
               resultStr);

  dmp.Patch_DeleteThreshold = 0.6f;
  patches = dmp.patch_make(
      "x1234567890123456789012345678901234567890123456789012345678901234567890"
      "y",
      "xabcy");
  results = dmp.patch_apply(patches,
                            "x12345678901234567890---------------++++++++++----"
                            "-----------12345678901234567890y");
  boolArray = results.second;
  resultStr = results.first + "\t" + (boolArray[0] ? "true" : "false") + "\t" +
              (boolArray[1] ? "true" : "false");
  assertEquals("patch_apply: Big delete, large change 2.", "xabcy\ttrue\ttrue",
               resultStr);
  dmp.Patch_DeleteThreshold = 0.5f;

  dmp.Match_Threshold = 0.0f;
  dmp.Match_Distance = 0;
  patches =
      dmp.patch_make("abcdefghijklmnopqrstuvwxyz--------------------1234567890",
                     "abcXXXXXXXXXXdefghijklmnopqrstuvwxyz--------------------"
                     "1234567YYYYYYYYYY890");
  results = dmp.patch_apply(
      patches, "ABCDEFGHIJKLMNOPQRSTUVWXYZ--------------------1234567890");
  boolArray = results.second;
  resultStr = results.first + "\t" + (boolArray[0] ? "true" : "false") + "\t" +
              (boolArray[1] ? "true" : "false");
  assertEquals("patch_apply: Compensate for failed patch.",
               "ABCDEFGHIJKLMNOPQRSTUVWXYZ--------------------"
               "1234567YYYYYYYYYY890\tfalse\ttrue",
               resultStr);
  dmp.Match_Threshold = 0.5f;
  dmp.Match_Distance = 1000;

  patches = dmp.patch_make("", "test");
  QString patchStr = dmp.patch_toText(patches);
  dmp.patch_apply(patches, "");
  assertEquals("patch_apply: No side effects.", patchStr,
               dmp.patch_toText(patches));

  patches =
      dmp.patch_make("The quick brown fox jumps over the lazy dog.", "Woof");
  patchStr = dmp.patch_toText(patches);
  dmp.patch_apply(patches, "The quick brown fox jumps over the lazy dog.");
  assertEquals("patch_apply: No side effects with major delete.", patchStr,
               dmp.patch_toText(patches));

  patches = dmp.patch_make("", "test");
  results = dmp.patch_apply(patches, "");
  boolArray = results.second;
  resultStr = results.first + "\t" + (boolArray[0] ? "true" : "false");
  assertEquals("patch_apply: Edge exact match.", "test\ttrue", resultStr);

  patches = dmp.patch_make("XY", "XtestY");
  results = dmp.patch_apply(patches, "XY");
  boolArray = results.second;
  resultStr = results.first + "\t" + (boolArray[0] ? "true" : "false");
  assertEquals("patch_apply: Near edge exact match.", "XtestY\ttrue",
               resultStr);

  patches = dmp.patch_make("y", "y123");
  results = dmp.patch_apply(patches, "x");
  boolArray = results.second;
  resultStr = results.first + "\t" + (boolArray[0] ? "true" : "false");
  assertEquals("patch_apply: Edge partial match.", "x123\ttrue", resultStr);
}

*/

// Construct the two texts which made up the diff originally.
QStringList diff_match_patch_test::diff_rebuildtexts(QList<Diff> diffs) {
  QStringList text;
  text << QString("") << QString("");
  foreach (Diff myDiff, diffs) {
    if (myDiff.operation != Insert) {
      text[0] += myDiff.text;
    }
    if (myDiff.operation != Delete) {
      text[1] += myDiff.text;
    }
  }
  return text;
}
