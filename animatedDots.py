## This code generates an animation that sweeps dots across the screen in a rectangular shape.
##
## Author: David Kelly
## Date:

import os
from time import sleep

seconds = 0.1
i = 0


# This function clears the screen for the next frame.
def clean():
    sleep(seconds)
    os.system('clear')


# Repeat the animation 3 times.
while i < 3:

    #1
    print(".")
    print("")
    print("")
    print("")
    clean()

    #2
    print("..")
    print(".")
    print("")
    print("")
    print("")
    clean()

    #3
    print("...")
    print("..")
    print(".")
    print("")
    print("")
    clean()

    #4
    print(" ...")
    print("...")
    print("..")
    print(".")
    print("")
    clean()

    #5
    print("  ...")
    print(" ...")
    print("...")
    print("..")
    print(".")
    clean()

    #6
    print("   ..")
    print("  ...")
    print(" ...")
    print("...")
    print("..")
    clean()

    #7
    print("    .")
    print("   ..")
    print("  ...")
    print(" ...")
    print("...")
    clean()

    #8
    print("")
    print("    .")
    print("   ..")
    print("  ...")
    print(" ...")
    clean()

    #9
    print("")
    print("")
    print("    .")
    print("   ..")
    print("  ...")
    clean()

    #10
    print("")
    print("")
    print("")
    print("    .")
    print("   ..")
    clean()

    #11
    print("")
    print("")
    print("")
    print("")
    print("    .")
    clean()

    i = i + 1

## end while
