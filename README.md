intel-power-control
===================

GPU power management for Intel hardware on Linux

# Functionality
- Displays cpu online states, temperatures and gpu clock settings.
- Toggle cpu online states
- Throttling of gpu clock (also automatic)
- intel-power-control-helper changes settings as root user

# Requirements
- C compiler (i.e. gcc)
- Qt 5.x
- python3

# Installation
    git clone https://github.com/jmechnich/intel-power-control.git
    
    # compile
    cd intel-power-control
    make

    # install to /usr/local
    sudo make install

    # OR install to local prefix
    make install PREFIX=$HOME/.local

    # run program
    intel-power-control

# Screenshots

![](https://raw.github.com/jmechnich/intel-power-control/master/screens/screen.png)
