#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>

#include "zaurusstuff.h"
#include "sharp_char.h"

ZaurusStuff::ZaurusStuff() : QObject()
{

}

void ZaurusStuff::blinkLedOn()
{
	sharp_led_status st;
	st.which = SHARP_LED_COLLIE_1;
	st.status = LED_COLLIE_1_FLASHON;
	ioctl(open("/dev/sharp_led", O_WRONLY), SHARP_LED_SETSTATUS, &st);
}

void ZaurusStuff::blinkLedOff()
{
	sharp_led_status st;
	st.which = SHARP_LED_COLLIE_1;
	st.status = LED_COLLIE_1_DEFAULT;
	ioctl(open("/dev/sharp_led", O_WRONLY), SHARP_LED_SETSTATUS, &st);
}

void ZaurusStuff::buzzerOn()
{
	ioctl(open("/dev/sharp_buz", O_WRONLY), SHARP_BUZZER_MAKESOUND, 4);
}

void ZaurusStuff::buzzerOff()
{

}

