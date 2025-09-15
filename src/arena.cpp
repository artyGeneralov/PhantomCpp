#include "globals.hpp"
/* Arena: */
#define ARENA_MAX_CAPACITY (4ULL*1024ULL*1024ULL*1024ULL)

#define ARENA_PUSH_STRUCT(arena, Type) \
  ((Type*) SecureZeroMemory(ArenaAlloc(arena, sizeof(Type), _alignof(Type)), sizeof(Type)))


global_variable size_t PAGE_SIZE = 4096;

typedef struct
{
  unsigned char* base;
  size_t capacity;
  size_t used;
} Arena;

// Prototypes:
size_t RoundUp(size_t a, size_t b);
DWORD ArenaPtrToOffset(const Arena*, const void*);
void* ArenaOffsetToPtr(const Arena*, const DWORD);
Arena ArenaCreate(size_t);
void ArenaFree(Arena*);
void ArenaReset(Arena*);
void* ArenaAlloc(Arena*, size_t, size_t);
int ArenaEnsureCommitted(Arena*, size_t);



Arena ArenaCreate(size_t requiredBytes)
{
  Arena a = {0};
  if(requiredBytes > ARENA_MAX_CAPACITY)
  {
    return a;
  }
  void* arenaBase = VirtualAlloc(0, requiredBytes, MEM_RESERVE, PAGE_READWRITE);
  a.base =(unsigned char*) arenaBase;
  a.capacity = requiredBytes;
  a.used = 0;

  return a;
}

int ArenaEnsureCommitted(Arena* arena, size_t new_used)
{
  size_t already_commited = RoundUp(arena->used, PAGE_SIZE);
  size_t required = RoundUp(new_used, PAGE_SIZE);

  if(required > already_commited)
  {
    size_t toCommit = required - already_commited;
    void* p = VirtualAlloc(arena->base + already_commited, toCommit, MEM_COMMIT, PAGE_READWRITE);
    if(!p)
    {
      return(0);
    }
  }
  return(1);
  
}

void* ArenaAlloc(Arena* arena, size_t bytes_to_alloc, size_t align)
{
  size_t currentAligned = RoundUp(arena->used, align);
  size_t new_used = currentAligned + bytes_to_alloc;

  if(new_used > arena->capacity)
  {
    return(0);
  }
  if(ArenaEnsureCommitted(arena, new_used) == 0)
  {
    return(0);
  }

  void* out = arena->base + currentAligned;
  arena->used = new_used;
  return out;
}

void ArenaReset(Arena* arena)
{
  arena->used = 0;
}

void ArenaFree(Arena* arena)
{
  if(arena->base)
  {
    VirtualFree(arena->base, 0, MEM_RELEASE);
  }
  arena->base = 0;
  arena->used = 0;
  arena->capacity = 0;
}

// TODO: see if the check for empty script is needed?

char* ArenaPushString(Arena* arena, const char* s)
{
  if(!s)
  {
    return(0);
  }
  int length = lstrlenA(s) + 1;
  char* p = (char*) ArenaAlloc(arena, length, 1);

  CopyMemory(p, s, length);
  return p;
}


// Helpers:
size_t RoundUp(size_t a, size_t b)
{
  return ((a + (b-1)) & ~(b-1));
}

DWORD ArenaPtrToOffset(const Arena* arena, const void* p)
{
    if (!p) return 0;
    uintptr_t diff = (uintptr_t)p - (uintptr_t)arena->base;
    return (DWORD)diff;
}

void* ArenaOffsetToPtr(const Arena* arena, const DWORD off)
{
  // TODO: do i need to check if offset is 0?
  void* p = (void*)((unsigned char*)(arena->base + off));
  return p;
}
