#include <linux/module.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/syscore_ops.h>
#include <linux/irq.h>
#include <linux/fcntl.h>
#include <linux/spinlock.h>


#include <linux/fs.h>
#include <asm/uaccess.h>        // for put_user 


// include RPi harware specific constants 
#include <mach/hardware.h>




/* 
 * MARCO Define
 */


#define DHT11_DRIVER_NAME "dht11"
#define RING_BUFFER_MAX 256
#define SUCCESS 0
#define BUFFER_SIZE 64                // Max length of the message from the device 


// set GPIO pin g as input 
#define GPIO_DIR_INPUT(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
// set GPIO pin g as output 
#define GPIO_DIR_OUTPUT(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
// get logical value from gpio pin g 
#define GPIO_READ_PIN(g) (*(gpio+13) & (1<<(g))) && 1
// sets   bits which are 1 ignores bits which are 0 
#define GPIO_SET_PIN(g)        *(gpio+7) = 1<<g;
// clears bits which are 1 ignores bits which are 0 
#define GPIO_CLEAR_PIN(g) *(gpio+10) = 1<<g;
// Clear GPIO interrupt on the pin we use 
#define GPIO_INT_CLEAR(g) *(gpio+16) = (*(gpio+16) | (1<<g));
// GPREN0 GPIO Pin Rising Edge Detect Enable/Disable 
#define GPIO_INT_RISING(g,v) *(gpio+19) = v ? (*(gpio+19) | (1<<g)) : (*(gpio+19) ^ (1<<g))
// GPFEN0 GPIO Pin Falling Edge Detect Enable/Disable 
#define GPIO_INT_FALLING(g,v) *(gpio+22) = v ? (*(gpio+22) | (1<<g)) : (*(gpio+22) ^ (1<<g))


// module parameters 
static int time_check = 0;
static struct timeval lasttv = {0, 0};


static spinlock_t lock;


// Forward declarations
static int dht11_data_read(struct inode *, struct file *);
static int dht11_remove_module(struct inode *, struct file *);
static ssize_t rp3_data_read(struct file *, char *, size_t, loff_t *);
static void CLR_IRQ(void);


// Global variables are declared as static, so are global within the file. 
static int num_dev_open_count = 0;                                // Is device open?  Used to prevent multiple access to device 
static char msg[BUFFER_SIZE];                                // The msg the device will give when asked 
static char *msg_Ptr;
static spinlock_t lock;
static unsigned int bitcount=0;
static unsigned int bytecount=0;
static unsigned int started=0;                        //Indicate if we have started a read or not
static unsigned char dht[5];                        // For result bytes
static int print_out_case = 0;                //the format of the result to printout
static int dht11_pin_gpio = 0;                //The pin configuration of GPIO pin
static int dev_driver_number = 80;                //The driver number which shown on linux


//Operations that can be performed on the device
static struct file_operations fops = {
		.read = rp3_data_read,
		.open = dht11_data_read,
		.release = dht11_remove_module
};


// Possible valid GPIO pins
int DHT11_RP3_GPIO[] = { 0, 1, 4, 8, 7, 9, 10, 11, 14, 15, 17, 18, 21, 22, 23,        24, 25 };


volatile unsigned *gpio;


// IRQ handler - where the timing takes place
static irqreturn_t irq_handler(int i, void *blah, struct pt_regs *regs)
{
		struct timeval tv;
		long deltv;
		int data = 0;
		int data_token;


		// use the GPIO data_token level 
		data_token = GPIO_READ_PIN(dht11_pin_gpio);


		/* reset interrupt */
		GPIO_INT_CLEAR(dht11_pin_gpio);


		if (time_check != -1) {
				// get current time 
				do_gettimeofday(&tv);


				// get time since last interrupt in microseconds 
				deltv = tv.tv_sec-lasttv.tv_sec;

				data = (int) (deltv*1000000 + (tv.tv_usec - lasttv.tv_usec));
				lasttv = tv;        //Save last interrupt time

				if((data_token == 1)&(data > 40))
				{
						started = 1;
						return IRQ_HANDLED;        
				}

				if((data_token == 0)&(started==1))
				{
						if(data > 80)
								return IRQ_HANDLED;                                                                                //Start/spurious? data_token
						if(data < 15)
								return IRQ_HANDLED;                                                                                //Spurious data_token?
						if (data > 60)//55 
								dht[bytecount] = dht[bytecount] | (0x80 >> bitcount);        //Add a 1 to the data byte

						//Uncomment to log bits and durations - may affect performance and not be accurate!
						//pr_info("B:%d, d:%d, dt:%d\n", bytecount, bitcount, data);
						bitcount++;
						if(bitcount == 8)
						{
								bitcount = 0;
								bytecount++;
						}
						//if(bytecount == 5)
						//        pr_info(KERN_INFO DHT11_DRIVER_NAME "Result: %d, %d, %d, %d, %d\n", dht[0], dht[1], dht[2], dht[3], dht[4]);
				}
		}
		return IRQ_HANDLED;
}


static int setup_interrupts(void)
{
		int set_irq_token;
		unsigned long flags;


		set_irq_token = request_irq(INTERRUPT_GPIO0, (irq_handler_t) irq_handler, 0, DHT11_DRIVER_NAME, (void*) gpio);
		/*
		 *  (1) EINVAL: -22 “invalid argument”
		 *  (2) EBUSY: -16 “Device or resource busy ”
		 */
		switch (set_irq_token) {

				case -EINVAL:
						pr_info(KERN_ERR DHT11_DRIVER_NAME ": Bad irq number or handler\n");
						return -EINVAL;
				case -EBUSY:
						pr_info(KERN_ERR DHT11_DRIVER_NAME ": IRQ %d is busy\n", INTERRUPT_GPIO0);
						return -EBUSY;
				default:
						pr_info(KERN_INFO DHT11_DRIVER_NAME        ": Interrupt %04x obtained\n", INTERRUPT_GPIO0);
						break;
		};


		spin_lock_irqsave(&lock, flags);


		// GPREN0 GPIO Pin Rising Edge Detect Enable 
		GPIO_INT_RISING(dht11_pin_gpio, 1);
		// GPFEN0 GPIO Pin Falling Edge Detect Enable 
		GPIO_INT_FALLING(dht11_pin_gpio, 1);
		// clear interrupt flag 
		GPIO_INT_CLEAR(dht11_pin_gpio);
		spin_unlock_irqrestore(&lock, flags);


		return 0;
}


// Initialise GPIO memory
static int init_port(void)
{
		// reserve GPIO memory region. 
		if (request_mem_region(GPIO_BASE, SZ_4K, DHT11_DRIVER_NAME) == NULL) {
				pr_info(KERN_ERR DHT11_DRIVER_NAME ": Falied to get the I/O memory addr of  GPIO\n");
				return -EBUSY;
		}


		// remap the GPIO memory 
		if ((gpio = ioremap_nocache(GPIO_BASE, SZ_4K)) == NULL) {
				pr_info(KERN_ERR DHT11_DRIVER_NAME ": failed to map GPIO I/O memory\n");
				return -EBUSY;
		}
		return 0;
}
/*
 *        Module Intialization! Entering Point for this Module
 */
static int __init dht11_init_module(void)
{
		int init_token;
		int i;


		// check for valid gpio pin number
		init_token = 0;
		for(i = 0; (i < ARRAY_SIZE(DHT11_RP3_GPIO)) && (init_token != 1); i++) {
				if(dht11_pin_gpio == DHT11_RP3_GPIO[i]) 
						init_token++;
		}
		/*gpio pin checked failed*/
		if (init_token != 1) {
				init_token = -EINVAL; // 22  ->Invalid argument
				pr_info(KERN_ALERT DHT11_DRIVER_NAME ": Invalid configuration of GPIO !\n");
				goto module_exit;
		}

		init_token = register_chrdev(dev_driver_number, DHT11_DRIVER_NAME, &fops);


		if (init_token < 0) {
				pr_info(KERN_ALERT DHT11_DRIVER_NAME "dht11 driver regidteration  failed :%d\n", init_token);
				return init_token;
		}


		pr_info(KERN_INFO DHT11_DRIVER_NAME ": driver registered!\n");


		init_token = init_port();
		if (init_token < 0)
				goto module_exit;
		return 0;
		/* Intialiton Failed”*/
module_exit:
		return init_token;
}


static void __exit dht11_exit_module(void)
{
		// release mapped memory and allocated region 
		if(gpio != NULL) {
				iounmap(gpio);
				release_mem_region(GPIO_BASE, SZ_4K);
				pr_info(DHT11_DRIVER_NAME ": cleaned up resources\n");
		}


		// Unregister the driver 
		unregister_chrdev(dev_driver_number, DHT11_DRIVER_NAME);
		pr_info(DHT11_DRIVER_NAME ": cleaned up module\n");
}


static int dht11_data_read(struct inode *inode, struct file *file)
{
		char result[3];                        //To say if the result is trustworthy or not
		int retry = 0;

		if (num_dev_open_count)
				return -EBUSY;


		try_module_get(THIS_MODULE);                //Increase use count


		num_dev_open_count++;


		// Take data low for min 18mS to start up DHT11
		//pr_info(KERN_INFO DHT11_DRIVER_NAME " Start setup (dht11_data_read)\n");


start_read:
		started = 0;
		bitcount = 0;
		bytecount = 0;
		dht[0] = 0;
		dht[1] = 0;
		dht[2] = 0;
		dht[3] = 0;
		dht[4] = 0;
		GPIO_DIR_OUTPUT(dht11_pin_gpio);         // Set pin to output
		GPIO_CLEAR_PIN(dht11_pin_gpio);        // Set low
		mdelay(20);                                        // DHT11 needs min 18mS to data_token a startup
		GPIO_SET_PIN(dht11_pin_gpio);                // Take pin high
		udelay(40);                                        // Stay high for a bit before swapping to read mode
		GPIO_DIR_INPUT(dht11_pin_gpio);         // Change to read

		//Start timer to time pulse length
		do_gettimeofday(&lasttv);

		// Set up interrupts
		setup_interrupts();

		//Give the dht11 time to reply
		mdelay(10);

		//Check if the read results are valid. If not then try again!
		if((dht[0] + dht[1] + dht[2] + dht[3] == dht[4]) & (dht[4] > 0))
				sprintf(result, "OK");
		else
		{
				retry++;
				sprintf(result, "BAD");
				if(retry == 5)
						goto return_result;                //We tried 5 times so bail out
				CLR_IRQ();
				mdelay(1100);                                //Can only read from sensor every 1 second so give it time to recover
				goto start_read;
		}


		//Return the result in various different formats
return_result:        
		switch(print_out_case){
				case 0:
						sprintf(msg, "Values: %d, %d, %d, %d, %d, %s\n", dht[0], dht[1], dht[2], dht[3], dht[4], result);
						break;
				case 1:
						sprintf(msg, "%0X,%0X,%0X,%0X,%0X,%s\n", dht[0], dht[1], dht[2], dht[3], dht[4], result);
						break;
				case 2:
						sprintf(msg, "%02X%02X%02X%02X%02X%s\n", dht[0], dht[1], dht[2], dht[3], dht[4], result);
						break;
				case 3:
						sprintf(msg, "Temperature: %dC\nHumidity: %d%%\nResult:%s\n", dht[0], dht[2], result);
						break;

		}
		msg_Ptr = msg;


		return SUCCESS;
}


// Called when a process closes the device file.
static int dht11_remove_module(struct inode *inode, struct file *file)
{
		// Decrement the usage count, or else once you opened the file, you'll never get get rid of the module. 
		module_put(THIS_MODULE);        
		num_dev_open_count--;


		CLR_IRQ();
		pr_info(KERN_INFO DHT11_DRIVER_NAME ": DHT11 Module is Removed\n");


		return 0;
}

/*
 * Everytime when the GPIO Interrupt event is triggered we must have to 
 *  clear the the register of the GPIO’s edge(Rising/Falling)
 */
static void CLR_IRQ(void)
{
		unsigned long irq_flags;


		/* spin lock protection*/
		spin_lock_irqsave(&lock, irq_flags);
		// GPREN0 GPIO Pin Rising Edge Detect Disable 
		GPIO_INT_RISING(dht11_pin_gpio, 0);
		// GPFEN0 GPIO Pin Falling Edge Detect Disable 
		GPIO_INT_FALLING(dht11_pin_gpio, 0);
		spin_unlock_irqrestore(&lock, irq_flags);
		free_irq(INTERRUPT_GPIO0, (void *) gpio);
}


// Called when a process, which already opened the dev file, attempts to read from it.
static ssize_t rp3_data_read(struct file *filp,        // file type for opening   
				char *data_buffer,        // buffer to fill with data 
				size_t length,        // length of the buffer     
				loff_t * offset)
{
		// Number of bytes actually written to the buffer 
		int read_buffer_count = 0;


		// If we're at the end of the message, return 0 signifying end of file 
		if (*msg_Ptr == 0)
				return 0;
		/* store  the data and sent to the buffer*/ 
		while (length && *msg_Ptr) {
				put_user(*(msg_Ptr++), buffer++);
				length--;
				read_buffer_count++;
		}
		return read_buffer_count;// Return the number of bytes put into the data_buffer
}


module_init(dht11_init_module);
module_exit(dht11_exit_module);


MODULE_DESCRIPTION("The Device Driver DHT11 for Raspberry Pi3 && Panzer.");
MODULE_AUTHOR("Polin Chen");
MODULE_LICENSE("GPL");
// Command line paramaters for gpio pin and driver major number
module_param(print_out_case, int, S_IRUGO);
MODULE_PARM_DESC(dht11_pin_gpio, "Format of output");
module_param(dht11_pin_gpio, int, S_IRUGO);
MODULE_PARM_DESC(dht11_pin_gpio, "GPIO pin to use");
module_param(dev_driver_number, int, S_IRUGO);
MODULE_PARM_DESC(dev_driver_number, "Driver handler major value");
