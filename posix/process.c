
void _exit(int status)
{
	printf("Halted with status %d\n", status);
	while(1) {}
}
