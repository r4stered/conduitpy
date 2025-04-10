// Copyright 2021-2025 FRC 6328
// http://github.com/Mechanical-Advantage
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// version 3 as published by the Free Software Foundation or
// available in the root directory of this project.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.

#pragma once

#include <cstdint>

namespace akit {
namespace conduit {
namespace wpilibio {

// Size of the data exchange buffer (CoreInputs) to use
static const int BUF_SIZE = 100000;

// Shared buffer
extern void* shared_buf;

void start();

// Allocates the shared buffer
void make_buffer();

// Captures data into the shared buffer
void capture_data();

}  // namespace wpilibio
}  // namespace conduit
}  // namespace akit