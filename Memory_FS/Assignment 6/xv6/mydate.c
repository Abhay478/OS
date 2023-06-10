#include "types.h"
#include "user.h"
#include "date.h"
// #include "defs.h"

int main(int argc, char *argv[])
{
  struct rtcdate r;

  if (date(&r)) {
    printf(2, "date failed\n");
    exit();
  }

  // your code to print the time in any format you like...
  /* example output format should be like this:
     	Year: 2016
	 Month: 1 or January
	 Date: 26
	 Hour: 15
	 Minute: 12
	 Second: 11
  */
 if(argc > 1) {
  int t = r.minute + 30;
  r.hour = r.hour + 5 + t / 60;
  r.minute = t % 60;
 }
  printf(1, "%d-%d-%d %d:%d:%d\n", r.year, r.month, r.day, r.hour, r.minute, r.second);
  exit();
}


