#ifndef __ARENA_H__
#define __ARENA_H__
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define INTERNAL_ALLOC malloc
#define INTERNAL_REALLOC realloc
#define INTERNAL_FREE free
#define internal static inline

#define or || // my new kb is ass and it's pain full to type pipes

#define ARENA_DEFAULT_CAPACITY 8 * 1024
typedef struct ArenaInfo {
  size_t capacity;
  size_t count;
  struct Arena *next;
} ArenaInfo;

typedef struct Arena {
  ArenaInfo info;
  char items[];
} Arena;

Arena *arena_init(size_t size);
void arena_deinit(Arena *arena);
void *arena_alloc(Arena *arena, size_t size);
void arena_reset(Arena *arena);
bool arena_has_enough_capacity(Arena *arena, size_t required);

#ifdef ARENA_IMPLEMENTATION

internal size_t arena__get_alignment(size_t size) {
  _UNREACHABLE("UNIMPLEMENTED");
}

Arena *arena_init(size_t size) {
  if (size == 0 or size < ARENA_DEFAULT_CAPACITY) size = ARENA_DEFAULT_CAPACITY;
  Arena *arena = INTERNAL_ALLOC(sizeof(ArenaInfo) + size);
  arena->info.capacity = size;
  arena->info.count = 0;
  arena->info.next = NULL;
  return arena;
}

void arena_deinit(Arena *arena) {
  Arena *current_arena = arena;
  while (current_arena != NULL) {
    Arena *next = current_arena->info.next;
    INTERNAL_FREE(current_arena);
    current_arena = next;
  }
}
void *arena_alloc(Arena *arena, size_t size) {
  Arena **current_arena = &arena;
  size_t idx = 0;
  while (*current_arena != NULL) {
    if (arena_has_enough_capacity(*current_arena, size)) break;
    current_arena = &(*current_arena)->info.next;
    idx++;
  }
  if (*current_arena == NULL) *current_arena = arena_init(size);

  assert(arena_has_enough_capacity(*current_arena, size));

  char *buffer = (*current_arena)->items + (*current_arena)->info.count;
  (*current_arena)->info.count += size;
  return buffer;
}

void arena_reset(Arena *arena) {
  Arena *current_arena = arena;
  while (current_arena != NULL) {
    current_arena->info.count = 0;
    current_arena = current_arena->info.next;
  }
}

bool arena_has_enough_capacity(Arena *arena, size_t required) {
  const size_t available = arena->info.capacity - arena->info.count;
  assert(available <= arena->info.capacity); // overflow check
  return available >= required;
}
#endif // ARENA_IMPLEMENTATION

#endif // __ARENA_H__
