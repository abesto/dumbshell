# DumbShell

This is my excuse to write C++. (See the branch called `c` to see my excuse for writing C.)

Features:
 - it can run ` ; ls   ; ls -la  ; echo  foobar`, even though they run parallelly atm, due to hackage for: 
 - it can run `ls -l | head -3`, even though the implementation is not quite clean right now.

Dependencies:
 - Boost 1.55.0

### Running

```sh
cmake .
make
./dsh
```

### Tests

```sh
cmake .
make check
```

### Screenshot

```
$ (15) ls -l | head -3
total 2688
-rw-r--r--  1 abesto staff   14047 Jul 26 08:53 CMakeCache.txt
drwxr-xr-x 19 abesto staff     646 Jul 26 17:21 CMakeFiles
```

----

Shell cat:

![shell cat](http://s3.favim.com/orig/47/black-and-white-cat-cute-shell-Favim.com-438008.jpg)
