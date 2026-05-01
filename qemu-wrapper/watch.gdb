target remote :1234
symbol-file /qemu-project/build/zt_esp32_qemu.elf
watch *(uint32_t*)0x3ffb2870
commands
  silent
  bt full
  info registers
  printf "=== WATCHPOINT HIT ===\n"
  continue
end
continue
