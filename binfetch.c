#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>


#include "arch.h"
#include "osabi.h"


typedef enum ansi_color
{
	blank   = 0,
	red     = 31,
	green   = 32,
	yellow  = 33,
	blue    = 34,
	magenta = 35,
	cyan    = 36,
	white   = 37
}
color;


const color col = magenta;
const size_t max_tok = 128;


static void set_color(const color c)
{
	printf("\033[%dm", c);
}

static void print_label(const char * label)
{
	set_color(col);
	printf("%s: ", label);
	set_color(blank);
}

static void advance(byte * tok, const size_t n, FILE * fp)
{
	bzero(tok, max_tok);
	fread(tok, n, 1, fp);
}

static void pair_parser(const byte val, const pr * prs, const size_t size, const char * type)
{
	size_t c = 0;
	while(c < size)
	{
		if (prs[c].key == val)
		{
			printf("%s\n", prs[c].str);
			break;
		}
		else if(++c == size)
		{
			printf("unknown %s %d", type, val);
		}
	}
}

static void elf_parser(FILE * fp)
{
	byte tok[max_tok];
	
	advance(tok, 5, fp);
	
	int bits = 0 ;// 32 or 64
	
	print_label("Class");
	
	if (tok[0] == 0x01)	
	{
		bits = 1;
		printf("32 bit\n");
	}
	else if (tok[0] == 0x02)
	{
		bits = 2;
		printf("64 bit\n");
	}
	
	print_label("Endianness");
	
	if (tok[1] == 0x01)
	{
		printf("little endian\n");
	}
	else if (tok[1] == 0x02)
	{
		printf("big endian\n");
	}
	else
	{
		printf("unknown endian\n");
	}
	
	print_label("Version");
	
	if (tok[2] == 0x01)
	{
		printf("ELFv1\n");
	}
	else if (tok[2] == 0x00)
	{
		printf("noncompliant\n");
	}
	
	print_label("OS ABI");
	
	pair_parser(tok[3], osabis, sizeof osabis / sizeof(pr), "abi");
	
	if (tok[4] != 0)
	{
		print_label("ABI Version");
		
		printf("%d\n", tok[4]);
	}
	
	while (fread(tok, 1, 1, fp) && tok[0] == 0);
	fseek(fp, -1, SEEK_CUR);;
	// do nothing
	
	advance(tok, 2, fp);
	
	print_label("Type");
	
	switch (tok[0])
	{
		case 0x01:
			printf("Object\n");
			break;
		case 0x02:
			printf("Static\n");
			break;
		case 0x03:
			printf("Shared\n");
			break;
		default:
			printf("Unknown object type %d\n", tok[0]);
	}
	
	advance(tok, 2, fp);
	
	print_label("Arch");
	
	pair_parser(tok[0], arches, sizeof arches / sizeof(pr), "arch");
	
	advance(tok, 4, fp);
	
	// do nothing
	
	advance(tok, bits == 1 ? 4 : 8, fp);
	
	print_label("Entry");
	
	printf("0x");
	
	{
		size_t b = 0;
		while (b < (bits == 1 ? 4 : 8))
		{
			printf("%x", tok[b]);
			++b;
		}
	}
	
	set_color(col);
	
	printf("\n");
	
}

int main(int argc, char **argv)
{
	
	if (argc < 2)
	{
		set_color(red);
		printf("you did not provide a binary\n");
		set_color(blank);
		return 1;
	}
	
	FILE * fp = fopen(argv[1], "rb");
	
	if (!fp)
	{
		set_color(red);
		printf("failed to open binary\n");
		set_color(blank);
		return 1;
	}
	
	print_label("Name");
	
	puts(basename(argv[1]));
	
	
	byte tok[max_tok];
	
	advance(tok, 4, fp);
	
	print_label("Header");
	
	if (!strcmp(tok + 1, "ELF"))
	{
		printf("ELF\n");
		elf_parser(fp);
	}
	else 
	{
		printf("unknown\n");
	}
	
	printf("Size: ");
	
	set_color(blank);
	
	struct stat st;
	stat(argv[1], &st);
	size_t sz = st.st_size;
	printf("%lu\n", sz);
	
	fclose(fp);
	
	return 0;
}
