# libDaisy/DaisySP + PlatformIO

This is a template for a project using libDaisy and DaisySP on the Electrosmith Daisy PIO board.

Discussion here:
https://forum.electro-smith.com/t/libdaisy-daisysp-on-platformio/966


## Initialize submodules
This repository uses external references to libDaisy and DaisySP, since they aren't directly included in the framework. To include them in the project, run the following commands in a terminal at the root of this repository:
```
git submodule init
git submodule update --remote
```

## Downgrade STM32Cube framework
After installing, you will need to modify the framework that PlatformIO includes in the project, since libDaisy is programmed using an older version of it.
Unzip `framework-stm32cubeh7.zip` at the root of the repo, copy its contents and paste in `packages/framework-stm32cubeh7`.

