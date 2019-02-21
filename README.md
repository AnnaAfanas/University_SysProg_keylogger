# Keylogger
Linux kernel module to log pressed keys on a keyboard
The logfile contains:
* code: scancode of pressed key  
* duration: time interval of key pressing  **(msec)**
* interval: time interval between t1 (previous key was released) and t2 (current key was pressed) **(msec)**
### Build
> make

### Install module
> sudo insmod kbd_module.ko

### Remove module
> sudo rmmod kbd_module

### Get info about module condition and/or possible errors
> dmesg

### Check whether the module is installed
> lsmod

### Open logfile
> sudo gedit /var/log/kbd.log  

***!!! IMPORTANT !!!*** You should have root rights to get access to this logfile

