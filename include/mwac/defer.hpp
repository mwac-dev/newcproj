// SPDX-FileCopyrightText: 2015 Marek Rusinowski
// SPDX-License-Identifier: MIT

/// Simple scope-exit helper inspired by and adapted from Marek Rusinowski's
/// MIT-licensed defer implementation.
///
/// Code inside a `defer` block runs automatically when the current scope exits,
/// including through an early return.
///
/// Example:
///
///     Texture texture = LoadTexture("image.png");
///
///     defer
///     {
///       UnloadTexture(texture);
///     };
///
/// Deferred blocks run in reverse declaration order. Captured references must
/// remain valid until the end of the scope, and deferred code should not throw.
///
/// Cannot have defers on the same line due to switching to __LINE__ from __COUNTER__

#pragma once

#include <type_traits>
#include <utility>

template <typename F>
class DeferFinalizer
{
  public:
  explicit DeferFinalizer(F&& f) noexcept(std::is_nothrow_move_constructible_v<F>)
      : f_(std::move(f))
  {
  }

  DeferFinalizer(const DeferFinalizer&) = delete;
  DeferFinalizer& operator=(const DeferFinalizer&) = delete;

  DeferFinalizer(DeferFinalizer&& other) noexcept(std::is_nothrow_move_constructible_v<F>)
      : f_(std::move(other.f_)),
        active_(other.active_)
  {
    other.active_ = false;
  }

  DeferFinalizer& operator=(DeferFinalizer&&) = delete;

  ~DeferFinalizer() noexcept
  {
    if (active_)
    {
      f_();
    }
  }

  void cancel() noexcept
  {
    active_ = false;
  }

  private:
  F f_;
  bool active_{true};
};

struct Deferrer
{
  template <typename F>
  [[nodiscard]] auto operator<<(F&& f) const
  {
    using Func = std::decay_t<F>;
    return DeferFinalizer<Func>{Func{std::forward<F>(f)}};
  }
};

inline constexpr Deferrer deferrer{};

#define TOKENPASTE(x, y)  x##y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define defer             auto TOKENPASTE2(__deferred_lambda_call_, __LINE__) = deferrer << [&]()
