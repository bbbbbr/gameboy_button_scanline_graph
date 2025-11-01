
Graph which scanline button press changes were registered. Quick and dirty implementation.

This is a riff on Pinobatch's "Telling Lys", but with a more detailed visual breakdown:
https://gbdev.gg8.se/forums/viewtopic.php?id=580


This test program turns off the OAM copy during VBlank, however
there is still the APA vblank interrupt running. So for better results:
1. Turn interrupts off (display will look off): SELECT+DOWN
2. Mash buttons
3. Turn interrupts back on (display correct again): SELECT+UP

Results from checking FPGA clone behavior in the `results` folder.

Spoiler: they all seem to more or less match hardware. Emulators var, some try to mimic more entropy.


Made in GBDK, requires the version of GBDK-2020 after 4.4.0 due to the use of SFR vars.
