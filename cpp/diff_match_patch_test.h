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

#ifndef DIFF_MATCH_PATCH_TEST_H
#define DIFF_MATCH_PATCH_TEST_H

#include <QObject>

#include "diff_match_patch.h"

class diff_match_patch_test : public QObject {
  Q_OBJECT

 private slots:
  //  DIFF TEST FUNCTIONS
  void testDiffCommonPrefix();
  void testDiffCommonSuffix();
  void testDiffCommonOverlap();
  void testDiffHalfmatch();
  void testDiffLinesToChars();
  void testDiffCharsToLines();
  void testDiffCleanupMerge();
  void testDiffCleanupSemanticLossless();
  void testDiffCleanupSemantic();
  void testDiffCleanupEfficiency();
  void testDiffPrettyHtml();
  void testDiffText();
  void testDiffDelta();
  void testDiffXIndex();
  void testDiffLevenshtein();
  void testDiffBisect();
  void testDiffMain();

  //  MATCH TEST FUNCTIONS
  void testMatchAlphabet();
  void testMatchBitap();
  // void testMatchMain();

  /*
  //  PATCH TEST FUNCTIONS
  void testPatchObj();
  void testPatchFromText();
  void testPatchToText();
  void testPatchAddContext();
  void testPatchMake();
  void testPatchSplitMax();
  void testPatchAddPadding();
  void testPatchApply();
  */

 private:
  diff_match_patch dmp;

  // Construct the two texts which made up the diff originally.
  QStringList diff_rebuildtexts(QList<Diff> diffs);
};

#endif  // DIFF_MATCH_PATCH_TEST_H
