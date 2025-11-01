
Graph which scanline button press changes were registered. Quick and dirty implementation.

This is a riff on Pinobatch's "Telling Lys", but with a more detailed visual breakdown:
https://gbdev.gg8.se/forums/viewtopic.php?id=580

# Usage
This test program turns off the OAM copy during VBlank, however
there is still the APA vblank interrupt running. So for better results:
1. Turn interrupts off (display will look off): SELECT+DOWN
2. Mash buttons
3. Turn interrupts back on (display correct again): SELECT+UP

# Results
Results from checking FPGA clone behavior in the `results` folder.
Spoiler: they all seem to more or less match hardware. Emulators var, some try to mimic more entropy.

Some emulator differences to illustrate (running on Linux with a USB gamepad):
![emulicious_bgb_button_scanline_graphs](/results/emulicious_bgb_button_scanline_graphs.png)


# Data Collection
After reading a button press and plotting it on the screen, it discards the next button press as a lazy way to try and avoid vsync and hsync biasing due to safe vram access waiting. So it takes two button presses to get a single screen point.

A non-interactive mode that collects all the points and then plots afterward could be added.

# Building
Made in GBDK, requires the version of GBDK-2020 after 4.4.0 due to the use of SFR vars.
