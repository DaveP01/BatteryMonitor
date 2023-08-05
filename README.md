"# BatteryMonitor" For hobbyist Radio Control type Lithium batteries.
Arduino code base for a Lithium, Ion or Polymer battery voltage checker and discharger (Suitable for Lithium cells of max 4.2V). Battery packs of between 1S to 6S are catered for as long as balance leads are avaiable, as the balance leads connect to the checker. The discharge function is via the main positive and negative leads on the battery.
Based around the ATMega 328P chipset, spi OLED screen and an activity button.
Functionality:
Upon connecting a battery, the screen displays the boot graphic logo and all leds light.
Pessing the button once starts the process of checking the individual battery cells voltage and displaying each voltage on screen along with an overall battery charge percentage.
If the button is held for over 3 seconds, the display will now show two options.
1. Depress the button for 3 seconds to disharge the battery to a 30% storgae charge level.
2. Depress the button for 5 seconds to discharge the battery to a 50% charge storage level.
Note that to discharge a battery pack safely, both the balance leads AND the power leads need to be connected to the checker. The balance leads measure real time voltage of the packs, without the balance leads connected the pack could discharge below the desired level and create an unsafe situation and possibly a fire, at best a completely discharged and possible ruined battery.
That's it for now.............
