
enum {
	DEVICE_CLASS_GPIO,
	DEVICE_CLASS_UART,
};

struct device {
	char *name;
	int class;

	struct file_operations *operations;
};



typedef int (*spi_config_cb) (struct spi_device *dev,
							  struct spi_config *cfg);

typedef int (*spi_transfer_cb) (struct spi_device *dev,
								size_t nbytes,
								const char *srcbuf,
								char *dstbuf);

struct spi_master {
};


typedef int (*uart_config_cb) ();

typedef int (*uart_flush_cb) ();

typedef int (*uart_push_cb) ();
typedef int (*uart_pull_cb) ();

struct uart_device {
};


typedef int (*gpio_config_cb) ();

typedef int (*gpio_get_cb) ();
typedef int (*gpio_set_cb) ();

typedef int (*gpio_wait_cb) ();

struct gpio_device {
};


struct file *device_open(struct device *dev);

struct device *device_by_name(const char *name);

int register_device(struct device *dev);
