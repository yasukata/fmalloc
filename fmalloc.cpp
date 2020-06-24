/*
 *
 * Copyright 2020 Kenichi Yasukata
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <fmalloc.hpp>

__thread uint64_t __fm_addr_base = 0;

extern void do_ptmalloc_init(unsigned long chunk_size);

/* init routine */
struct fm_info *fmalloc_init(const char *filepath, bool *init)
{
	struct stat st;
	struct fm_super *s;
	void *mem;
	uint64_t *magicp;
	int fd;
	size_t len;

	if (stat(filepath, &st) < 0) {
		perror("stat");
		exit(1);
	}

	len = st.st_size;

	fd = open(filepath, O_RDWR, 0644);
	if (fd < 0) {
		perror("open");
		fprintf(stderr, "file: %s (%d)\n", filepath, fd);
		exit(1);
	}

	mem = mmap(0, len, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	assert(mem != MAP_FAILED);

	s = (struct fm_super *) mem;

	magicp = (uint64_t *) mem;
	if (*magicp != FMALLOC_MAGIC) {
		int *initialized;

		s->magic = FMALLOC_MAGIC;
		s->set_total_size(len);

		initialized = (int *)((uint64_t) mem + FMALLOC_OFF);
		*initialized = -1;
		*init = true;
	}

	__fm_addr_base = (uint64_t) mem;

	do_ptmalloc_init(s->chunk_size);

	return new fm_info(fd, mem, s);
}

void fmalloc_set_target(struct fm_info *fi)
{
	__fm_addr_base = (uint64_t) fi->mem;
}

void *fmalloc(size_t size)
{
	return dlmalloc(size);
}

void ffree(void *addr)
{
	dlfree(addr);
}
