/// Lightweight result, optional-value, and error propagation helpers.
///
/// Use `Result<T>` for operations that may fail and should provide an error
/// message. Use `Result<void>` when an operation may fail but has no meaningful
/// success value.
///
/// Use `Optional<T>` when the absence of a value is expected and does not
/// represent an error. Optional values do not contain diagnostic information.
///
/// New errors are created with `fail(...)`, while `TRY` and `TRY_VOID`
/// propagate errors returned by other functions.
///
/// In order to be able to use `fail(...)`,
/// Add `ERRORSITE` as the final parameter when a function may create a new
/// error. This captures the function's call site so `err_log()` can print a
/// compiler-style diagnostic that tools such as Emacs compilation mode can
/// navigate.
///
/// Result example:
///
///     Result<Texture> load_texture(const char* path, ERRORSITE)
///     {
///       Texture texture = LoadTexture(path);
///
///       if (!IsTextureValid(texture))
///       {
///         return fail(std::format("failed to load texture: {}", path));
///       }
///
///       return texture;
///     }
///
/// Optional example:
///
///     Optional<Enemy*> find_enemy(int id)
///     {
///       if (Enemy* enemy = lookup_enemy(id))
///       {
///         return enemy;
///       }
///
///       return std::nullopt;
///     }
///
/// Propagation example:
///
///     Result<void> run()
///     {
///       TRY(texture, load_texture("assets/image.png"));
///       TRY_VOID(initialize_audio());
///       return {};
///     }
///
/// `TRY(name, expression)` unwraps a successful `Result<T>` into `name`.
/// `TRY_VOID(expression)` propagates a failed `Result<void>`.
/// `err_log()` should normally be used at the top-level program boundary after
/// an error can no longer be propagated.

#pragma once

#include <expected>
#include <format>
#include <optional>
#include <print>
#include <source_location>
#include <string>
#include <utility>

struct Error
{
  std::string message;
  std::source_location location;
};

namespace result_detail {

  inline std::unexpected<Error> make_error(
      std::string message,
      std::source_location location)
  {
    return std::unexpected<Error>{
        Error{
              .message = std::move(message),
              .location = location,
              },
    };
  }

} // namespace result_detail

/// Adds call-site error creation support to a function.
///
/// Use as the final parameter when the function calls `fail(...)`.
/// Functions that only propagate existing errors through `TRY` or `TRY_VOID`
/// do not need `ERRORSITE`.
#define ERRORSITE ErrorSite fail = {}

class ErrorSite
{
  public:
  constexpr ErrorSite(
      std::source_location location = std::source_location::current())
      : location_{location}
  {
  }

  [[nodiscard]]
  std::unexpected<Error> operator()(std::string message) const
  {
    return result_detail::make_error(
        std::move(message),
        location_);
  }

  private:
  std::source_location location_;
};

template <typename T>
using Optional = std::optional<T>;

template <typename T>
using Result = std::expected<T, Error>;

inline void err_log(const Error& error)
{
  std::println(
      stderr,
      "{}:{}:{}: error: {}",
      error.location.file_name(),
      error.location.line(),
      error.location.column(),
      error.message);
}

/// Unwraps a successful Result<T> into `name`.
/// Returns the existing error from the current function on failure.
///
/// Example:
///
///     TRY(texture, load_texture_checked("assets/image.png"));
///
#define TRY(name, expression)              \
  auto name##_result = (expression);       \
  if (!name##_result) [[unlikely]]         \
  {                                        \
    return std::unexpected<Error>{         \
        std::move(name##_result.error())}; \
  }                                        \
  auto name = std::move(*name##_result)

/// Propagates an error from a Result<void>.
///
/// Example:
///
///     TRY_VOID(initialize_audio());
///
#define TRY_VOID(expression)                   \
  do                                           \
  {                                            \
    auto try_void_result = (expression);       \
    if (!try_void_result) [[unlikely]]         \
    {                                          \
      return std::unexpected<Error>{           \
          std::move(try_void_result.error())}; \
    }                                          \
  } while (false)
