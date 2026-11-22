#pragma once

#include <stddef.h>
#include <stdint.h>

#include <vector>

class Assembler {
 public:
  static constexpr uint32_t ScratchRegister = 17;

  void Emit(uint32_t instruction);

  // ldr x[reg], =value
  void LoadLiteral(uint32_t reg, uint64_t value);
  void LoadAddress(uint32_t reg, const void* address);
  void Jump(const uint32_t* target, bool link = false);
  uint32_t* Commit(const uint32_t* near = nullptr);

 private:
  struct Literal {
    size_t index;
    uint64_t value;
  };

  void EmitLiteralPool();

  std::vector<uint32_t> code;
  std::vector<Literal> literals;
};
