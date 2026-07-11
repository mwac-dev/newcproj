/// Lightweight aliases and helpers for C++ polymorphic memory resources.
///
/// `mwac::Allocator` is a generic allocator handle backed by a
/// `std::pmr::memory_resource`. Functions that allocate memory can accept an
/// allocator through the `ALLOC` parameter macro:
/// (note that `ALLOC` is simply shorthand for mwac::Allocator allocator)
///
///     Enemy* spawn_enemy(ALLOC, Vector2 position)
///     {
///       return allocator.new_object<Enemy>(position);
///     }
///
/// The caller chooses where the memory comes from:
///
///     mwac::Arena arena;
///     Enemy* enemy = spawn_enemy(&arena, position);
///
/// To create a fixed buffer allocator you can repurpose the arena allocator as shown:
///
/// std::array<std::byte, 64 * 1024> storage;
///
/// mwac::Arena arena{
///     storage.data(),
///     storage.size(),
///     std::pmr::null_memory_resource()
/// };
///
///  The same allocator may allocate any number of unrelated types:
///
///      Enemy* enemy = allocator.new_object<Enemy>(position);
///      PowerUp* powerup = allocator.new_object<PowerUp>(position, velocity);
///
///  PMR containers use the allocator for their dynamic storage:
///
///      mwac::Vector<Enemy*> enemies{allocator};
///      mwac::String name{allocator};
///      mwac::HashMap<int, Enemy*> enemies_by_id{allocator};
///
///  The container object itself may still live on the stack. Only its dynamic
///  storage is obtained from the supplied memory resource.
///
///  Objects allocated with `new_object<T>()` can be destroyed with:
///
///      allocator.delete_object(enemy);
///
///  `delete_object()` runs the object's destructor. Whether the individual
///  memory block is reclaimed depends on the underlying memory resource.
///  A monotonic arena normally reclaims all of its memory together when
///  `release()` is called or when the arena is destroyed.
///
///  The memory resource must outlive every object and container using it.
///  All pointers, strings, vectors, and other allocations backed by an arena
///  become invalid after that arena is released or destroyed.
///
///  Use `mwac::heap_allocator()` when normal heap allocation is desired:
///
///      auto allocator = mwac::heap_allocator();
///      Enemy* enemy = allocator.new_object<Enemy>(position);
///      allocator.delete_object(enemy);
///
///  This system controls ordinary CPU memory allocated through PMR. It does not
///  control external resources such as GPU textures, file handles, sockets, or
///  memory allocated internally by third-party libraries.

#pragma once

#include <memory_resource>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace mwac {

  using MemoryResource = std::pmr::memory_resource;

  using Arena = std::pmr::monotonic_buffer_resource;
  using Pool = std::pmr::unsynchronized_pool_resource;
  using ThreadSafePool = std::pmr::synchronized_pool_resource;

  // Generic allocator handle backed by a memory_resource.
  using Allocator = std::pmr::polymorphic_allocator<>;

  template <typename T>
  using Vector = std::pmr::vector<T>;

  using String = std::pmr::string;

  template <typename K, typename V>
  using HashMap = std::pmr::unordered_map<K, V>;

  template <typename T>
  using HashSet = std::pmr::unordered_set<T>;

  [[nodiscard]]
  inline Allocator heap_allocator() noexcept
  {
    return Allocator{std::pmr::new_delete_resource()};
  }

  // Note that general_allocator is by default the heap_allocator
  // but general_allocator will be whatever PMR resource is configured
  // prefer heap_allocator to make intentions clear
  [[nodiscard]]
  inline Allocator general_allocator() noexcept
  {
    return Allocator{std::pmr::get_default_resource()};
  }

} // namespace mwac

#define ALLOC mwac::Allocator allocator
