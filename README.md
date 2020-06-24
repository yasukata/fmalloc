# fmalloc - malloc for memory-mapped files

This package is a modified version of Wolfram Gloger's ptmalloc3
that is a modified version of Doug Lea's malloc-2.8.3 implementation.

As part of the GNU C library, the source files may be available under
the GNU Library General Public License (see the comments in the
files). But as part of this stand-alone package, the code is also
available under the (probably less restrictive) conditions described
in the file 'COPYRIGHT'. In any case, there is no warranty whatsoever
for this package.

## What is fmalloc?

fmalloc aims to bridge the representation gap between in-memory and on-disk data structures.

fmalloc allows applications to directly store im-memory data structures on files through the memory-mapped file interface.

Technically, fmalloc implements two functionalities.

1. fmalloc allocates memory from a virtual memory region of a memory-mapped file.
2. fmalloc offers a smart pointer named fmalloc pointer (fm\_ptr) that automatically translates pointer values for a virtual memory address and file offset accordingly.

### 1. Efficient memory allocation

Essentially, fmalloc is a wrapper of ptmalloc that is widely used as part of the GNU C library.
fmalloc slightly modifies ptmalloc to allocate memory from
the virtual memory region of a memory-mapped file instead of calling the brk and mmap system calls.

In other words, fmalloc's internal memory allocation algorithm is the one implemented in ptmalloc that
has been testified to be efficient and performant.

### 2. fm\_ptr: A smart pointer for automatic file offset and memory address translation

fm\_ptr is a key mediator of in-memory and on-disk pointer representations.

Commonly, pointers for in-memory data structures are virtual addresses,
and for on-disk data structures, they are file offsets.

Usually, the transformation between these two types of pointers is done by application programs.

However, such pointer translation is not common in the standard in-memory data structure programming and tends to increase the implementation complexity.

fm\_ptr obviates the necessity of manual pointer translation, and highly simplifies the programming using memory-mapped files.

## API

### Primary data structure associated with a memory-mapped file

```struct fm_info``` is a data structure associated with a memory-mapped file.

### Allocate struct fm_info

```c
struct fm_info *fmalloc_init(const char *filepath, bool *init)
```

- ```filepath```: specifies the file path to store the in-memory data.
- ```init```: The ```fmalloc_init``` function perform initialization if the file specified by ```filepath``` is not initialized for fmalloc. If ```fmalloc_init``` initializes the file, it stores ```true``` for the bool variable pointed by ```init```.

### Set target memory-mapped file

```c
void fmalloc_set_target(struct fm_info *fi)
```

### Allocate memory

```c
void *fmalloc(size_t size)
```

```fmalloc``` allocates memory from a virtual memory region of a memory-mapped file that is associated with
the currently targeting ```fm_info``` structure.

As seen, it is substantially similar to the standard malloc.

### Free memory

```c
void ffree(void *addr)
```

```ffree``` frees memory for a memory-mapped file that is associated with
the currently targeting ```fm_info``` structure.

### Data layout on a file

The following depicts the data layout on a file. The first 4KB is used for the internal information for fmalloc itself.
The range between 4KB~8KB is reserved for application programs. fmalloc assumes that applications put their root objects there.
From 8KB to the end is managed by ptmalloc.

```
 0       4KB      8KB         end
 |-- fm_super --|-- for app --|-- ... malloc ...--|
```

## Compilation

The following command will generate a library file named ```libfmalloc.a```.

```
$ make
```

## Demo

The following is a simple persistent list implementation using fmalloc. This is found in the ```examples/list``` directory.

```c
#include <fmalloc.hpp>

/* list implementation */
struct node {
	int val;
	fm_ptr<struct node> next;
} __attribute__((packed));

static void list_append(struct node *head, struct node *newnode)
{
	struct node *n = head;
	while (n->next) {
		n = n->next;
	}
	n->next = newnode;
}

/* app reserved super block */
struct app_super {
	fm_ptr<struct node> head;
};

/* example append operation */
static void append_nodes(struct node *head)
{
	int i;
	printf("writing list data...\n");
	for (i = 0; i < 10; i++) {
		struct node *n = (struct node *) fmalloc(sizeof(struct node));
		n->val = i;
		n->next = NULL;
		list_append(head, n);
	}
	printf("done.\n");
}

/* example read operation */
static void read_nodes(struct node *head)
{
	struct node *n = head->next;
	printf("reading list data...\n");
	while (n) {
		printf("%d\n", n->val);
		n = n->next;
	}
	printf("done.\n");
}

int main(int argc, char const* argv[])
{
	struct app_super *super;
	bool init = false;
	struct fm_info *fi;

	if (argc < 2) {
		printf("please specify a data file\n");
		exit(1);
	}

	fi = fmalloc_init(argv[1], &init);
	fmalloc_set_target(fi);

	/* fmalloc reserves 4KB ~ 8KB for app for locating super block */
	super = (struct app_super *) ((unsigned long) fi->mem + PAGE_SIZE);

	if (init) {
		super->head = (struct node *) fmalloc(sizeof(struct node));
		append_nodes(super->head);
	} else {
		if (super->head) {
			read_nodes(super->head);
		}
	}

	return 0;
}
```

### A few points

- ```struct node``` has fm\_ptr to point the next node that is located on a memory-mapped file.
- ```list_append```, ```append_nodes```, and ```read_nodes``` are the same as the standard in-memory list implementation. But, actually, this implementation stores and reads the list on a file.

