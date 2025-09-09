#define ARENA_SIZE (4*1024*1024) // TODO: think about this

typedef struct ARENA
{
  unsigned char* base;
  unsigned int used;
  unsigned int size;


} ARENA;

ARENA Arena;

void ArenaInit(void* memory, unsigned int size)
{
  Arena.base = (unsigned char*)memory;
  Arena.used = 0;
  Arena.size = size;
}

static void* ArenaAlloc(unsigned int bytes, unsigned int align)
{
  unsigned int p = (unsigned int)((Arena.used + (align-1)) & ~(align-1));
  if(p + bytes > Arena.size)
  {
    return(0);
  }
  void* ptr = Arena.base + p;
  Arena.used = p + bytes;
  return ptr;
}

static char* ArenaPushString(const char* str)
{
  if(!str) return(0);
  unsigned int len = (unsigned int)lstrlenA(str) + 1;
  char* dst = (char*)ArenaAlloc(len, 1);
  if(!dst)
  {
    return(0);
  }
  lstrcpynA(dst, str, (int)len);
  return dst;
}
