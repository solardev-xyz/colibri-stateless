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

// The generic C FFI (colibri.h), which the Rust, Kotlin, Swift, Python and
// Dart bindings use:
// - a verifier context that can't be created leaves nothing behind: the
//   error `c4_verify_init` set is freed with the rest (the Valgrind job
//   fails on the leak this used to be);
// - a response given through `c4_req_set_response` is zero-terminated past
//   its length, as the Node and WASM bindings' copies are, so the verifier's
//   JSON parsing stops at its end.
#include "bytes.h"
#include "colibri.h"
#include "state.h"
#include "sync_committee.h"
#include "unity.h"
#include <stddef.h>

void setUp(void) {
}

void tearDown(void) {
}

// No proof for a provable method: "missing proof!" (`THROW_ERROR`).
void test_create_ctx_without_a_proof_frees_its_error(void) {
  bytes_t none = {.data = NULL, .len = 0};
  void*   ctx  = c4_verify_create_ctx(none, "eth_getBlockByNumber", "[\"latest\",false]", 1, NULL, 0);
  TEST_ASSERT_NULL(ctx);
}

// A proof that isn't a request: the SSZ validation error (`THROW_INVALID`).
void test_create_ctx_with_an_invalid_proof_frees_its_error(void) {
  uint8_t byte  = 0;
  bytes_t proof = {.data = &byte, .len = 1};
  void*   ctx   = c4_verify_create_ctx(proof, "eth_getBlockByNumber", "[\"latest\",false]", 1, NULL, 0);
  TEST_ASSERT_NULL(ctx);
}

// The copy keeps the length and ends with a zero past it.
void test_a_response_is_zero_terminated(void) {
  data_request_t req = {0};
  c4_req_set_response(&req, (bytes_t) {.data = (uint8_t*) "{\"a\":", .len = 5}, 2);
  TEST_ASSERT_EQUAL_UINT32(5, req.response.len);
  TEST_ASSERT_EQUAL_UINT8(0, req.response.data[5]);
  TEST_ASSERT_EQUAL_UINT16(2, req.response_node_index);
  safe_free(req.response.data);
  c4_req_set_response(&req, (bytes_t) {.data = NULL, .len = 0}, 0);
  TEST_ASSERT_EQUAL_UINT32(0, req.response.len);
  TEST_ASSERT_EQUAL_UINT8(0, req.response.data[0]);
  safe_free(req.response.data);
}

// An unbalanced JSON answer to the verifier's checkpointz request: the
// parser stops at the answer's end, and the request fails.
void test_an_unbalanced_answer_is_read_within_its_length(void) {
#ifdef USE_CHECKPOINTZ
  c4_state_t state = {0};
  uint64_t   epoch = 0;
  bytes32_t  root  = {0};
  TEST_ASSERT_FALSE(c4_req_checkpointz_status(&state, C4_CHAIN_MAINNET, &epoch, root));
  TEST_ASSERT_NOT_NULL(state.requests);
  c4_req_set_response(state.requests, (bytes_t) {.data = (uint8_t*) "{\"data\":{", .len = 9}, 0);
  TEST_ASSERT_FALSE(c4_req_checkpointz_status(&state, C4_CHAIN_MAINNET, &epoch, root));
  TEST_ASSERT_NOT_NULL(state.error);
  c4_state_free(&state);
#else
  TEST_IGNORE_MESSAGE("USE_CHECKPOINTZ is off");
#endif
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_create_ctx_without_a_proof_frees_its_error);
  RUN_TEST(test_create_ctx_with_an_invalid_proof_frees_its_error);
  RUN_TEST(test_a_response_is_zero_terminated);
  RUN_TEST(test_an_unbalanced_answer_is_read_within_its_length);
  return UNITY_END();
}
