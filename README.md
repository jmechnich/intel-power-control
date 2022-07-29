# intel-power-control

GPU power management for Intel hardware on Linux

## Functionality
- Displays cpu online states, temperatures and gpu clock settings.
- Toggle cpu online states
- Throttling of gpu clock (also automatic)
- intel-power-control-helper changes settings as root user

## Requirements
- C compiler (i.e. gcc)
- Qt 5.x
- python3

## Installation from source

```
git clone https://github.com/jmechnich/intel-power-control.git
    
# compile
cd intel-power-control
make

# install to /usr/local
sudo make install

# OR install to local prefix
make install PREFIX=$HOME/.local
sudo chown root:root $HOME/.local/bin/intel-power-control-helper
sudo chmod 4755 $HOME/.local/bin/intel-power-control-helper

# run program
intel-power-control
```

## Installation from PyPi

```
# system-wide
sudo pip install intel-power-control
echo "%sudo ALL=(ALL) NOPASSWD: /usr/local/bin/intel-power-control-helper" | \
  sudo tee /etc/sudoers.d/intel-power-control

# OR as user
pip install intel-power-control --user
sudo chown root:root $HOME/.local/bin/intel-power-control-helper
sudo chmod 4755 $HOME/.local/bin/intel-power-control-helper

# run program
intel-power-control
```

## Changing the look and feel

Use the `-style` commandline option to select a different
theme. Available themes can be listed by calling the application with
an invalid style argument, e.g. `-style help`.

## Screenshots

![](https://raw.github.com/jmechnich/intel-power-control/master/screens/screen.png)
