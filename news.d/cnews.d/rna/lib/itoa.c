char *
itoa(i)
int i;
{
	static char buf[30];

	sprintf(buf, "%d", i);
	return buf;
}
