
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/slab.h>

#define AUTHOR_NAME "Anna Afanaseva IU7-73"
#define DESCRIPTION "Course project for BMSTU. Driver for detecting special device"
#define LICENSE "GPL"


// Flash-USB information
// idVendor=0951, idProduct=1666
// Mfr=1, Product=2, SerialNumber=3
// Product: DataTraveler 3.0
// Manufacturer: Kingston
// SerialNumber: 60A44C425091AF30B0001F01

unsigned int logging_enabled = 0;
unsigned int dev = 0;

static void activate_logging(void) {
	logging_enabled = 1;
}

static void deactivate_logging(void) {
	logging_enabled = 0;
}

static int check_usb_device(char *serial) {
	if (!strcmp(serial, "60A44C425091AF30B0001F01")) {
		printk(KERN_INFO "THIS IS CORRECT DEVICE");
		dev = 1;
	}
	return dev;
}

static int probe(struct usb_interface *intf, const struct usb_device_id *id) {
	struct usb_device *dev = interface_to_usbdev(intf);
	printk("\n>SPY DRIVER< : Detected new usb device.");	
	if (check_usb_device(dev->serial) == 1) 
		activate_logging();
	return 0;
}


static void disconnect(struct usb_interface *intf) {
	printk(">SPY DRIVER< : USB device was disconnected.");
	if (dev) {
		deactivate_logging();
		dev = 0;
	}
}


static struct usb_device_id empty_usb_table[] = {
 {.driver_info = 1},
 { }
};

MODULE_DEVICE_TABLE(usb, empty_usb_table);

static struct usb_driver log_usb_driver = 
{
	.name = "logkdb",
	.probe = probe,
	.disconnect = disconnect,
	.id_table = empty_usb_table,
};

static int my_init_module(void)
{
	int err;
	printk(">\nSPY DRIVER< : USB devices detector activated.\n");
	err = usb_register(&log_usb_driver);
	return err;
}

static void my_cleanup_module(void)
{
	printk(">SPY DRIVER< : USB devices detector deactivated.\n");
	usb_deregister(&log_usb_driver);
}

EXPORT_SYMBOL(logging_enabled);

MODULE_LICENSE(LICENSE);
MODULE_AUTHOR(AUTHOR_NAME);
MODULE_DESCRIPTION(DESCRIPTION);

module_init(my_init_module);
module_exit(my_cleanup_module);