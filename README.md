STM32_Nucleo

Linker and Start up code modified to make the Nucleo board with the STM32F413ZHT6 microcontroller work fine with:<br>
-arm-none-eabi toolchain<br>
-stlink<br>
<br>
In order to download the example on the board:<br>
1- Download and extract the folder<br>
2- Run 'make all' from a terminal<br>
3- run 'make flash' with your Nucleo board connected to a USB port<br>
<br>
This is a simple project with the only aim to check the modified linker and startup scripts. <br>
