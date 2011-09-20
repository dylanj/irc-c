# p3q

Project 3Q - A bot in C with lua!

Authors:

* [Dylan Johnston](https://github.com/DylanJ "Dylan Johnston") 
* [Brad Janke](https://github.com/bradj "Brad Janke")

## Build

### Dependencies
You must have a version of GCC installed or some other C compiler. 
_makefile_ can be configured to change the compiler.

p3q is dependent on lua. This means you must have _lua.h_ available for use. This is typically available on your
favorite distros package manager.

**Arch linux**

```sh
pacman -S lua
```

**Debian/Ubuntu**

```sh
apt-get install lua lua-devel
```

### Build using make
```sh
make
```

