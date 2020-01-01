 如果发现把这功能包包含之后， 调用函数，在执行时串口输出严重错误，如下
/**示例
 assertion "rc522_read(0x24) == 0x25" failed: file "/home/hans/git/esp32_learn/esp32/components/hans_mfrc522/bsp_mfrc522.c", line 95, function: rc522_init
 abort() was called at PC 0x400d27ff on core 0
 0x400d27ff: __assert_func at /builds/idf/crosstool-NG/.build/xtensa-esp32-elf/src/newlib/newlib/libc/stdlib/assert.c:62 (discriminator 8)

 ***/
 这个提示是在说，你这行调用的这个函数出问题了。
 assertion是会根据传入参数对系统进行操作的，比如这个rc522_read()函数 ，执行失败，会有一个返回值，传给assert（）函数，这个函数接收到后，会把程序重启。
 问题就在你用了这个模块，但是你没有接mfrc522
 
 解决办法，把mfrc522线接对

