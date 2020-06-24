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
