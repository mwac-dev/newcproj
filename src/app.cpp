#include <mwac/allocator.hpp>
#include <mwac/defer.hpp>
#include <mwac/result.hpp>

#include <print>
#include <string_view>

namespace {

  /// non_empty returns an Optional type
  Optional<std::string_view> non_empty(std::string_view value)
  {
    if (value.empty())
    {
      return std::nullopt;
    }

    return value;
  }

  /// make_greeting returns a Result type of mwac::String
  /// Result means it could fail and should be handled with a TRY() as it returns a type
  /// the type mwac::String is shorthand for std::pmr::string which takes our custom allocator
  ///
  /// ALLOC is shorthand for mwac::Allocator allocator,
  /// making it obvious that it will call an allocation, but a bit less code
  /// if you prefer to pass the parameter type and name yourself simply ignore or delete the ALLOC macro
  ///
  ///  ERRORSITE is shorthand for ErrorSite fail = {}
  ///  this allows you to return fail("some error message")
  ///
  ///  The convention I personally follow:
  ///  `ALLOC` is always the *first* parameter
  ///  `ERRORSITE` is always the *last* parameter
  Result<mwac::String> make_greeting(
      ALLOC,
      std::string_view name,
      ERRORSITE)
  {
    if (name.empty())
    {
      return fail("cannot create a greeting with an empty name");
    }

    mwac::String greeting{allocator};

    greeting += "Hello, ";
    greeting += name;
    greeting += '!';

    return greeting;
  }

} // namespace

Result<void> run()
{
  // Here the chosen allocator is a heap allocator,
  // this is also by default the general_allocator()
  // unless your PMR resource is configured differently
  auto allocator = mwac::heap_allocator();

  // We passed in an empty initialized string_view
  // this triggers the `.value_or` which return "world" instead
  // try initialized configured_name with actual text and see what happens!
  constexpr std::string_view configured_name{};

  // if we keep `configured_name{}` empty and change `.value_or("world")` to `.value_or("")`
  // then the TRY will fail and the error message will get printed and allow navigation to the callsite
  // where the error occured - try it!
  const auto name = non_empty(configured_name).value_or("world");

  TRY(greeting, make_greeting(allocator, name));

  int* number = allocator.new_object<int>(42);

  // defer is used how you might expect, will get called at end of scope
  // here we allocate memory for an int and point to it using our heap allocator
  // delete_object will call the object's destructor AND deallocate which is what you most certainly will want
  defer
  {
    allocator.delete_object(number);
  };

  std::println("{}", greeting);
  std::println("Allocated value: {}", *number);

  return {};
}
