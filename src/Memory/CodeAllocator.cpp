#include "Memory/CodeAllocator.h"

#include <inttypes.h>
#include <stdio.h>
#include <sys/mman.h>

#include <algorithm>
#include <vector>

#include "Codegen/Branch.h"
#include "Memory/Page.h"

namespace CodeAllocator {
constexpr size_t BlockAlignment = 4;

struct CodePage {
  uint32_t* start;
  size_t used;
};

std::vector<CodePage>& CodePages() {
  static std::vector<CodePage> pages;
  return pages;
}

uint32_t* MapPage(void* hint) {
  void* page = mmap(hint, Page::Size(), PROT_READ | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (page == MAP_FAILED) return nullptr;

  return static_cast<uint32_t*>(page);
}

uint32_t* MapReachablePage(uintptr_t hint, const uint32_t* near) {
  uint32_t* page = MapPage(reinterpret_cast<void*>(hint));
  if (!page || Branch::InRange(near, page)) return page;

  munmap(page, Page::Size());
  return nullptr;
}

uint32_t* MapNearPage(const uint32_t* near) {
  uintptr_t center = reinterpret_cast<uintptr_t>(near);
  uintptr_t reach = Branch::Range * sizeof(uint32_t);
  uintptr_t lower = 0;
  if (center > reach) {
    lower = center - reach;
  }
  uintptr_t upper = center + reach;

  FILE* maps = fopen("/proc/self/maps", "re");
  if (!maps) {
    return nullptr;
  }

  uint32_t* page = nullptr;
  uintptr_t gapStart = 0;
  uintptr_t regionStart = 0;
  uintptr_t regionEnd = 0;
  while (!page && fscanf(maps, "%" SCNxPTR "-%" SCNxPTR "%*[^\n]", &regionStart, &regionEnd) == 2) {
    uintptr_t candidate = Page::Align(std::max(gapStart, lower), Page::Size());
    gapStart = regionEnd;

    if (candidate < upper && candidate + Page::Size() <= regionStart) page = MapReachablePage(candidate, near);
  }

  fclose(maps);
  return page;
}

uint32_t* Allocate(size_t count, const uint32_t* near) {
  size_t instructionsPerPage = Page::Size() / sizeof(uint32_t);
  count = Page::Align(count, BlockAlignment);
  if (count > instructionsPerPage) {
    return nullptr;
  }

  for (CodePage& page : CodePages()) {
    uint32_t* block = page.start + page.used;
    if (page.used + count > instructionsPerPage || (near && !Branch::InRange(near, block))) {
      continue;
    }

    page.used += count;
    return block;
  }

  uint32_t* start = nullptr;
  if (near) {
    start = MapNearPage(near);
  } else {
    start = MapPage(nullptr);
  }
  if (!start) {
    return nullptr;
  }

  CodePages().push_back({start, count});
  return start;
}

}  // namespace CodeAllocator
