#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct passwd;
struct group;
struct stat;

ssize_t readlink(const char *__restrict __path, char *__restrict __buf, size_t __len) { return 0; }
int getpwuid_r(__uid_t __uid, struct passwd *__restrict __resultbuf, char *__restrict __buffer, size_t __buflen,
			   struct passwd **__restrict __result) {
	return 0;
}
int getgrgid_r(__gid_t __gid, struct group *__restrict __resultbuf, char *__restrict __buffer, size_t __buflen,
			   struct group **__restrict __result) {
	return 0;
}
ssize_t fgetxattr(int __fd, const char *__name, void *__value, size_t __size) { return 0; }
ssize_t flistxattr(int __fd, char *__list, size_t __size) { return 0; }
int fsetxattr(int __fd, const char *__name, const void *__value, size_t __size, int __flags) { return 0; }
int ioctl(int __fd, unsigned long int __request, ...) { return 0; }
char *getcwd(char *__buf, size_t __size) { return 0; }
int faccessat(int __fd, const char *__file, int __type, int __flag) { return 0; }
int linkat(int __fromfd, const char *__from, int __tofd, const char *__to, int __flags) { return 0; }
int mknodat(int __fd, const char *__path, __mode_t __mode, __dev_t __dev) { return 0; }
int fchownat(int __fd, const char *__file, __uid_t __owner, __gid_t __group, int __flag) { return 0; }
int fchown(int __fd, __uid_t __owner, __gid_t __group) { return 0; }
__uid_t geteuid(void) { return 0; }
int fchmod(int __fd, __mode_t __mode) { return 0; }
int stat(const char *__restrict __file, struct stat *__restrict __buf) { return 0; }
int dup(int __fd) { return 0; }
// void *sbrk (intptr_t __delta){ return 0; }
int kill(__pid_t __pid, int __sig) { return 0; }
__pid_t getpid(void) { return 0; }
__off_t lseek(int __fd, __off_t __offset, int __whence) { return 0; }
int mkdirat(int __fd, const char *__path, __mode_t __mode) { return 0; }
// ssize_t read (int __fd, void *__buf, size_t __nbytes){}
int symlinkat(const char *__from, int __tofd, const char *__to) { return 0; }
__mode_t umask(__mode_t __mask) { return 0; }
// char *basename (const char *__filename){}
ssize_t readlinkat(int __fd, const char *__restrict __path, char *__restrict __buf, size_t __len) { return 0; }
// ssize_t write (int __fd, const void *__buf, size_t __n){}
int isatty(int __fd) { return 0; }
int unlinkat(int __fd, const char *__name, int __flag) { return 0; }
int fstat(int __fd, struct stat *__buf) { return 0; }
int fstatat(int __fd, const char *__restrict __file, struct stat *__restrict __buf, int __flag) { return 0; }
int openat(int __fd, const char *__file, int __oflag, ...) { return 0; }
int close(int __fd) { return 0; }

char *basename(char *__filename) { return 0; }
