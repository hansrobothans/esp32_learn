
/home/hans/esp/xtensa-esp32-elf/bin/../lib/gcc/xtensa-esp32-elf/8.2.0/../../../../xtensa-esp32-elf/bin/ld: 
/home/hans/git/esp32_learn/esp32/build/hans_mfrc522/libhans_mfrc522.a(rc522.o):
/home/hans/git/esp32_learn/esp32/components/hans_mfrc522/include/rc522.h:18: 
multiple definition of `rc522_timer_running'; 

/home/hans/git/esp32_learn/esp32/build/main/libmain.a(esp32.o):
/home/hans/git/esp32_learn/esp32/components/hans_mfrc522/include/rc522.h:18:
 first defined herecollect2: 
 error:
  ld returned 1 exit status/home/hans/esp/esp-idf/make/project.mk:524: 
  recipe for target '/home/hans/git/esp32_learn/esp32/build/esp32.elf' failedmake: *** [/home/hans/git/esp32_learn/esp32/build/esp32.elf] Error 1