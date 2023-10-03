// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// A simple unit-test style driver for libfuzzer tests.
// Usage: <fuzzer_test> <file>...

#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

extern "C" int LLVMFuzzerTestOneInput(const unsigned char* data, size_t size);

std::vector<char> readFile(std::string path) {
  std::ifstream in(path);
  return std::vector<char>((std::istreambuf_iterator<char>(in)),
      std::istreambuf_iterator<char>());
}

int main(int argc, char **argv) {
  if (argc == 1) {
    std::cerr << "Usage: " << argv[0] << " <file>..." << std::endl;
    exit(1);
  }

  for (int i = 1; i < argc; ++i) {
    std::cout << argv[i] << std::endl;
    auto v = readFile(argv[i]);
    LLVMFuzzerTestOneInput((const unsigned char *)v.data(), v.size());
  }
}
