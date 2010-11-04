
#include <posix/file.h>

#include <lpc/uart.h>

static int uart_write(struct file *f, const void *buf, size_t nbytes)
{
	return uart_tx_fifo(f->f_cookie_int, buf, nbytes);
}

static int uart_read(struct file *f, void *buf, size_t nbytes)
{
	return uart_rx_fifo(f->f_cookie_int, buf, nbytes);
}

struct file_operations uart_operations = {
	.fop_write = &uart_write,
	.fop_read = &uart_read,
};

int uart_open(int uart)
{
	struct file *f = file_alloc();

	f->f_name = "uart";
	f->f_flags = O_RDWR;
	f->f_ops = &uart_operations;

	f->f_cookie_int = uart;

	return fd_alloc(f);
}
