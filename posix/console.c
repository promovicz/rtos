
#include "console.h"

#include <posix/file.h>

bool_t console_enabled;

static int console_write(struct file *f, const void *buf, size_t nbytes)
{
	if(console_enabled) {
		return vcom_tx_fifo(buf, nbytes);
	} else {
		return nbytes;
	}
}

static int console_read(struct file *f, void *buf, size_t nbytes)
{
	if(console_enabled) {
		return vcom_rx_fifo(buf, nbytes);
	} else {
		return nbytes;
	}
}

struct file_operations console_operations = {
	.fop_write = &console_write,
	.fop_read = &console_read,
};

void console_init(void)
{
	struct file *fi = file_alloc();
	struct file *fo = file_alloc();
	struct file *fe = file_alloc();

	/* initialize tables - must be done here to guarantee correct FD allocation */
	file_table_init();

	/* stdin */
	fi->f_used = 1;
	fi->f_flags = O_RDONLY;
	fi->f_ops = &console_operations;
	fd_alloc(fi); /* XXX ASSERT returns 0 */

	/* stdout */
	fo->f_used = 1;
	fo->f_flags = O_WRONLY;
	fo->f_ops = &console_operations;
	fd_alloc(fo); /* XXX ASSERT returns 1 */

	/* stderr */
	fe->f_used = 1;
	fe->f_flags = O_WRONLY;
	fe->f_ops = &console_operations;
	fd_alloc(fe); /* XXX ASSERT returns 2 */
}

void console_enable(void)
{
	console_enabled = BOOL_TRUE;
}
