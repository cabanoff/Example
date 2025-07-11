#include "Application.h"
#include "main.h"

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;

  Application app;
  
  app.setup();
  
  app.loop();
  
  return 0;
}
