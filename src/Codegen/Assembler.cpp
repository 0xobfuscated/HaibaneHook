#include "Codegen/Assembler.h"

#include "Memory/CodeAllocator.h"
#include "Memory/CodePatch.h"

constexpr uint32_t Nop = 0xD503201F;
constexpr uint32_t LdrLiteral = 0x58000000;
constexpr uint32_t Br = 0xD61F0000;
constexpr uint32_t Blr = 0xD63F0000;

void Assembler::Emit(uint32_t instruction) {
  code.push_back(instruction);
}

void Assembler::LoadLiteral(uint32_t reg, uint64_t value) {
  literals.push_back({code.size(), value});
  Assembler::Emit(LdrLiteral | reg);
}

void Assembler::LoadAddress(uint32_t reg, const void* address) {
  Assembler::LoadLiteral(reg, reinterpret_cast<uintptr_t>(address));
}

void Assembler::Jump(const uint32_t* target, bool link) {
  LoadAddress(ScratchRegister, target);

  if (link) {
    Assembler::Emit(Blr | ScratchRegister << 5);
  } else {
    Assembler::Emit(Br | ScratchRegister << 5);
  }
}

uint32_t* Assembler::Commit(const uint32_t* near) {
  EmitLiteralPool();

  uint32_t* address = CodeAllocator::Allocate(code.size(), near);
  if (!address || !CodePatch::Write(address, code.data(), code.size() * sizeof(uint32_t))) {
    return nullptr;
  }

  return address;
}

void Assembler::EmitLiteralPool() {
  // 64bit literals must be 8 byte aligned
  if (code.size() % 2 != 0) {
    Assembler::Emit(Nop);
  }

  for (const Literal& literal : literals) {
    uint32_t distance = code.size() - literal.index;
    code[literal.index] |= distance << 5;

    Assembler::Emit(literal.value);
    Assembler::Emit(literal.value >> 32);
  }
}
