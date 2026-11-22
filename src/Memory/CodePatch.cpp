#include "Memory/CodePatch.h"

#include <string.h>
#include <sys/mman.h>

#include "Memory/Page.h"

namespace CodePatch {

bool Write(void* address, const void* data, size_t size) {
  char* begin = static_cast<char*>(address);
  size_t offset = reinterpret_cast<uintptr_t>(address) % Page::Size();
  char* page = begin - offset;
  size_t length = Page::Align(offset + size, Page::Size());
  if (mprotect(page, length, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
    return false;
  }

  memcpy(begin, data, size);
  __builtin___clear_cache(begin, begin + size);

  return mprotect(page, length, PROT_READ | PROT_EXEC) == 0;
}

}  // namespace CodePatch
