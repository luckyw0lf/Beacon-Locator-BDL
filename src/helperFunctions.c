
#include <board.h>

void setUpPin(PORT_Type *port, int pin, unsigned char output, int pin_config){
    GPIO_Type *gpio;

    if(port == PORT0){
        MRCC0->MRCC_GLB_CC0_SET  |=  MRCC_MRCC_GLB_CC0_PORT0(1);
        MRCC0->MRCC_GLB_CC1_SET  |=  MRCC_MRCC_GLB_CC1_GPIO0(1);
        MRCC0->MRCC_GLB_RST0_SET |=  MRCC_MRCC_GLB_CC0_PORT0(1);
        MRCC0->MRCC_GLB_RST1_SET |=  MRCC_MRCC_GLB_CC1_GPIO0(1);
        gpio = GPIO0;
    }

    if(port == PORT1){
        MRCC0->MRCC_GLB_CC0_SET  |=  MRCC_MRCC_GLB_CC0_PORT1(1);
        MRCC0->MRCC_GLB_CC1_SET  |=  MRCC_MRCC_GLB_CC1_GPIO1(1);
        MRCC0->MRCC_GLB_RST0_SET |=  MRCC_MRCC_GLB_CC0_PORT1(1);
        MRCC0->MRCC_GLB_RST1_SET |=  MRCC_MRCC_GLB_CC1_GPIO1(1);
        gpio = GPIO1;
    }
    
    if(port == PORT2){
        MRCC0->MRCC_GLB_CC0_SET  |=  MRCC_MRCC_GLB_CC0_PORT2(1);
        MRCC0->MRCC_GLB_CC1_SET  |=  MRCC_MRCC_GLB_CC1_GPIO2(1);
        MRCC0->MRCC_GLB_RST0_SET |=  MRCC_MRCC_GLB_CC0_PORT2(1);
        MRCC0->MRCC_GLB_RST1_SET |=  MRCC_MRCC_GLB_CC1_GPIO2(1);
        gpio = GPIO2;
    }

    if(port == PORT3){
        MRCC0->MRCC_GLB_CC1_SET  |=  MRCC_MRCC_GLB_CC1_PORT3(1) | MRCC_MRCC_GLB_CC1_GPIO3(1);
        MRCC0->MRCC_GLB_RST1_SET |=  MRCC_MRCC_GLB_CC1_PORT3(1);
        MRCC0->MRCC_GLB_RST1_SET |=  MRCC_MRCC_GLB_CC1_GPIO3(1);
        gpio = GPIO3;
    }

    port->PCR[pin] |= pin_config | PORT_PCR_LK(1);
    if(output){
        gpio->PDDR |= (1<<pin);
    }
}