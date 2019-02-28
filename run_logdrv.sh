sudo rmmod usbhid
sudo rmmod uas 
sudo rmmod usb_storage

sudo insmod logdrv.ko
sudo insmod kbd_module.ko

sudo modprobe usb_storage
sudo modprobe uas
sudo modprobe usbhid