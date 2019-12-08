#include <stdio.h>
#include "../components/IRRemote/include/IRRemote.h"

#define RECV_PIN	4
uint8_t command = 0;

void app_main()
{
  IRrecvInit(RECV_PIN, 3);

  while(1)
  {
    command = IRrecvReadIR();
    if (command != 0)
    {
      printf("%d\n", command);
    }
  }
}
