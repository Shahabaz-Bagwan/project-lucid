// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "nativeui/gfx/geometry/vector2d.h"

#include <cmath>

#include "base/strings/stringprintf.h"

namespace nu {

bool Vector2d::IsZero() const {
  return x_ == 0 && y_ == 0;
}

void Vector2d::Add(const Vector2d& other) {
  x_ += other.x_;
  y_ += other.y_;
}

void Vector2d::Subtract(const Vector2d& other) {
  x_ -= other.x_;
  y_ -= other.y_;
}

int64_t Vector2d::LengthSquared() const {
  return static_cast<int64_t>(x_) * x_ + static_cast<int64_t>(y_) * y_;
}

float Vector2d::Length() const {
  return static_cast<float>(std::sqrt(static_cast<double>(LengthSquared())));
}

std::string Vector2d::ToString() const {
  return base::StringPrintf("[%d %d]", x_, y_);
}

}  // namespace nu
