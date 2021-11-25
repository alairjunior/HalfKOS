#include <msp430.h>
#include <hkos.h>


//char (*__kaboom)[sizeof( void* )] = 1;

void blink( uint8_t pin )
{

	for(;;) {
		volatile unsigned int i;	// volatile to prevent optimization

		hkos_hal_gpio_toggle(pin);

		i = 65535;					// SW Delay
		do i--;
		while(i != 0);
	}
}

void blink_green( void )
{
	blink(14);
}

void blink_red( void )
{
	blink(2);
}

void blink_error( void )
{

	for(;;) {
		volatile unsigned int i;	// volatile to prevent optimization

		hkos_hal_gpio_toggle(2);
		hkos_hal_gpio_toggle(14);

		i = 65535;					// SW Delay
		do i--;
		while(i != 0);
	}
}

int main( void ) {
	hkos_init();
    hkos_hal_gpio_write( 2, LOW );
	hkos_hal_gpio_write( 14, LOW );
	hkos_hal_gpio_config( 2, OUTPUT );
	hkos_hal_gpio_config( 14, OUTPUT );

	if ( hkos_add_task( blink_red, 64 ) == 0 )
		blink_error();

	if ( hkos_add_task( blink_green, 64 ) == 0 )
		blink_error();

	// Should not return
	hkos_start();

	// We shouldn't be here
	blink_error();
}



