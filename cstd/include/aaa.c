portabllity.h

#include <sys/types.h>
#include <wchar.h>
#include <time.h>
#include <getopt.h>
#include <libgen.h> // __GLIBC__

#include <libkern/OSByteOrder.h> // APPLE
#include <sys/endian.h> // BSD
#include <byteswap.h> // other
#include <endian.h> // other

#include <sys/mount.h>

#include <sys/statfs.h> // linux
#include <sys/swap.h> // linux
#include <sys/sysinfo.h> // linux

#include <util.h> // APPLE
#include <pty.h> // BSD
#include <termios.h> // other

#include <sys/xattr.h> // APPLE || linux

#include <shadow.h> // CFG_TOYBOX_SHADOW
#include <utmpx.h> // CFG_TOYBOX_UTMPX

#include <fcntl.h>

#include <android/log.h> // BIONIC
#include <processgroup/sched_policy.h> // BIONIC

#include <sys/random.h> // CFG_TOYBOX_GETRANDOM

toys.h
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/utsname.h>
#include <sys/wait.h>

...networking
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#include <sys/ioctl.h>




<android/log.h>
<arpa/inet.h>
<byteswap.h>
<ctype.h>
<dirent.h>
<endian.h>
<errno.h>
<fcntl.h>
<fnmatch.h>
<getopt.h>
<grp.h>
<inttypes.h>
<libgen.h>
<libkern/OSByteOrder.h>
<limits.h>
<linux/xattr.h>
<locale.h>
<math.h>
<net/if.h>
<netdb.h>
<netinet/in.h>
<netinet/tcp.h>
<paths.h>
<poll.h>
<processgroup/sched_policy.h>
<pty.h>
<pwd.h>
<regex.h>
<sched.h>
<selinux/selinux.h>
<setjmp.h>
<shadow.h>
<signal.h>
<stdarg.h>
<stddef.h>
<stdint.h>
<stdio.h>
<stdlib.h>
<string.h>
<strings.h>
<sys/endian.h>
<sys/ioctl.h>
<sys/mman.h>
<sys/mount.h>
<sys/random.h>
<sys/resource.h>
<sys/smack.h>
<sys/socket.h>
<sys/stat.h>
<sys/statfs.h>
<sys/statvfs.h>
<sys/swap.h>
<sys/sysinfo.h>
<sys/time.h>
<sys/times.h>
<sys/types.h>
<sys/un.h>
<sys/utsname.h>
<sys/wait.h>
<sys/xattr.h>
<syslog.h>
<termios.h>
<time.h>
<unistd.h>
<util.h>
<utime.h>
<utmpx.h>
<wchar.h>
<wctype.h>
