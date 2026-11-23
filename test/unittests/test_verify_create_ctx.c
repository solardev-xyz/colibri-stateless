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

// A verifier context that can't be created leaves nothing behind: the
// error `c4_verify_init` set is freed with the rest. The Valgrind job
// fails on the leak this used to be.
#include "colibri.h"
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

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_create_ctx_without_a_proof_frees_its_error);
  RUN_TEST(test_create_ctx_with_an_invalid_proof_frees_its_error);
  return UNITY_END();
}
