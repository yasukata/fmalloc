# A persistent list implementation using fmalloc

If you do not compile the fmalloc library yet, please do that before starting this demo.

```
$ make
g++  -O3 -pipe -g -Werror -Wall -Wunused-function -Wextra -I./ -I../../ -std=c++11 -L../../  -c -o list.o list.cpp
g++ -O3 -pipe -g -Werror -Wall -Wunused-function -Wextra -I./ -I../../ -std=c++11 -L../../ -L../../ list.o -o fmalloc_list -lfmalloc -lpthread

$ dd if=/dev/zero of=./list.dat bs=4k count=16
16+0 records in
16+0 records out
65536 bytes (66 kB, 64 KiB) copied, 0.000341662 s, 192 MB/s

$ ./fmalloc_list ./list.dat
writing list data...
done.

$ ./fmalloc_list ./list.dat
reading list data...
0
1
2
3
4
5
6
7
8
9
done.
```
