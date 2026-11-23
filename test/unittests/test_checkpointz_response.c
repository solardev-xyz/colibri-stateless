/*
 * Copyright (c) 2025,2026 corpus.core GmbH
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * SPDX-License-Identifier: MIT
 */

// The checkpointz answer the verifier asks for when it has no chain state
// (`c4_req_checkpointz_status`): an answer that fails its validation frees
// the validation's error. (An unbalanced answer is test_c_api.c's.)
#include "bytes.h"
#include "state.h"
#include "sync_committee.h"
#include "unity.h"

void setUp(void) {
}

void tearDown(void) {
}

// Valid JSON, the wrong shape: the validation's error is freed.
void test_an_invalid_answer_frees_its_validation_error(void) {
#ifdef USE_CHECKPOINTZ
  c4_state_t state = {0};
  uint64_t   epoch = 0;
  bytes32_t  root  = {0};
  TEST_ASSERT_FALSE(c4_req_checkpointz_status(&state, C4_CHAIN_MAINNET, &epoch, root));
  TEST_ASSERT_NOT_NULL(state.requests);
  TEST_ASSERT_NULL(state.error);
  state.requests->response = bytes_dup(bytes((uint8_t*) "{}", 3));
  TEST_ASSERT_FALSE(c4_req_checkpointz_status(&state, C4_CHAIN_MAINNET, &epoch, root));
  TEST_ASSERT_NOT_NULL(state.error);
  c4_state_free(&state);
#else
  TEST_IGNORE_MESSAGE("USE_CHECKPOINTZ is off");
#endif
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_an_invalid_answer_frees_its_validation_error);
  return UNITY_END();
}
