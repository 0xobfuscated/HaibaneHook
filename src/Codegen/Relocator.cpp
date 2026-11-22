#include "Codegen/Relocator.h"

#include "Codegen/Assembler.h"

namespace Relocator {

constexpr uint32_t BranchOver = 0x14000003;  // b #12

constexpr uint32_t LoadFromScratch[] = {
    0xB9400220,  // ldr wt, [x17]
    0xBD400220,  // ldr st, [x17]
    0xF9400220,  // ldr xt, [x17]
    0xFD400220,  // ldr dt, [x17]
    0xB9800220,  // ldrsw xt, [x17]
    0x3DC00220,  // ldr qt, [x17]
    0xF9800220,  // prfm <op>, [x17]
};
constexpr uint32_t Loads = sizeof(LoadFromScratch) / sizeof(LoadFromScratch[0]);

bool Matches(uint32_t instruction, uint32_t mask, uint32_t pattern) {
  return (instruction & mask) == pattern;
}

uint32_t Field(uint32_t instruction, int shift, int width) {
  return (instruction >> shift) & ((1u << width) - 1);
}

uint32_t Register(uint32_t instruction) {
  return Field(instruction, 0, 5);
}

int64_t SignExtend(uint64_t value, int width) {
  int shift = 64 - width;
  int64_t extended = value << shift;
  return extended >> shift;
}

const uint32_t* Target(const uint32_t* pc, int shift, int width) {
  return pc + SignExtend(Field(*pc, shift, width), width);
}

void RelocateConditionalBranch(Assembler& assembler, const uint32_t* pc, int width) {
  uint32_t offsetMask = ((1u << width) - 1) << 5;
  uint32_t branchToJump = (*pc & ~offsetMask) | 2 << 5;

  assembler.Emit(branchToJump);
  assembler.Emit(BranchOver);
  assembler.Jump(Target(pc, 5, width));
}

bool RelocateLoadLiteral(Assembler& assembler, const uint32_t* pc) {
  uint32_t opc = Field(*pc, 30, 2);
  uint32_t vector = Field(*pc, 26, 1);
  uint32_t kind = opc << 1 | vector;
  if (kind >= Loads) {
    return false;
  }

  assembler.LoadAddress(Assembler::ScratchRegister, Target(pc, 5, 19));
  assembler.Emit(LoadFromScratch[kind] | Register(*pc));
  return true;
}

void RelocateAddress(Assembler& assembler, const uint32_t* pc) {
  uintptr_t address = reinterpret_cast<uintptr_t>(pc);
  int64_t immediate = SignExtend(Field(*pc, 5, 19) << 2 | Field(*pc, 29, 2), 21);
  bool page = Field(*pc, 31, 1);

  uint64_t value = address + immediate;
  if (page) {
    value = (address & ~uint64_t{0xFFF}) + immediate * 4096;
  }

  assembler.LoadLiteral(Register(*pc), value);
}

bool RelocateInstruction(Assembler& assembler, const uint32_t* pc) {
  uint32_t instruction = *pc;

  if (Matches(instruction, 0x7C000000, 0x14000000)) {  // B, BL
    assembler.Jump(Target(pc, 0, 26), Field(instruction, 31, 1));
  } else if (Matches(instruction, 0xFF000000, 0x54000000)) {  // B.cond
    RelocateConditionalBranch(assembler, pc, 19);
  } else if (Matches(instruction, 0x7E000000, 0x34000000)) {  // CBZ, CBNZ
    RelocateConditionalBranch(assembler, pc, 19);
  } else if (Matches(instruction, 0x7E000000, 0x36000000)) {  // TBZ, TBNZ
    RelocateConditionalBranch(assembler, pc, 14);
  } else if (Matches(instruction, 0x3B000000, 0x18000000)) {  // LDR, LDRSW, PRFM (literal)
    return RelocateLoadLiteral(assembler, pc);
  } else if (Matches(instruction, 0x1F000000, 0x10000000)) {  // ADR, ADRP
    RelocateAddress(assembler, pc);
  } else {
    assembler.Emit(instruction);
  }

  return true;
}

uint32_t* Relocate(const uint32_t* code, size_t count) {
  Assembler assembler;

  for (const uint32_t* pc = code; pc < code + count; pc++) {
    if (!RelocateInstruction(assembler, pc)) {
      return nullptr;
    }
  }

  assembler.Jump(code + count);
  return assembler.Commit();
}

}  // namespace Relocator
