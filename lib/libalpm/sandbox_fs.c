/*
 *  sandbox_fs.c
 *
 *  Copyright (c) 2021-2022 Pacman Development Team <pacman-dev@lists.archlinux.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>

#include "config.h"
#include "sandbox_fs.h"

#ifdef HAVE_LINUX_LANDLOCK_H
# include <linux/landlock.h>
# include <sys/prctl.h>
# include <sys/syscall.h>
#endif /* HAVE_LINUX_LANDLOCK_H */

#ifdef HAVE_LINUX_LANDLOCK_H
#ifndef landlock_create_ruleset
static inline int landlock_create_ruleset(const struct landlock_ruleset_attr *const attr,
		const size_t size, const __u32 flags)
{
	return syscall(__NR_landlock_create_ruleset, attr, size, flags);
}
#endif /* landlock_create_ruleset */

#ifndef landlock_add_rule
static inline int landlock_add_rule(const int ruleset_fd,
		const enum landlock_rule_type rule_type,
		const void *const rule_attr, const __u32 flags)
{
	return syscall(__NR_landlock_add_rule, ruleset_fd, rule_type, rule_attr, flags);
}
#endif /* landlock_add_rule */

#ifndef landlock_restrict_self
static inline int landlock_restrict_self(const int ruleset_fd, const __u32 flags)
{
	return syscall(__NR_landlock_restrict_self, ruleset_fd, flags);
}
#endif /* landlock_restrict_self */

#define _LANDLOCK_ACCESS_FS_WRITE ( \
  LANDLOCK_ACCESS_FS_WRITE_FILE | \
  LANDLOCK_ACCESS_FS_REMOVE_DIR | \
  LANDLOCK_ACCESS_FS_REMOVE_FILE | \
  LANDLOCK_ACCESS_FS_MAKE_CHAR | \
  LANDLOCK_ACCESS_FS_MAKE_DIR | \
  LANDLOCK_ACCESS_FS_MAKE_REG | \
  LANDLOCK_ACCESS_FS_MAKE_SOCK | \
  LANDLOCK_ACCESS_FS_MAKE_FIFO | \
  LANDLOCK_ACCESS_FS_MAKE_BLOCK | \
  LANDLOCK_ACCESS_FS_MAKE_SYM)

#define _LANDLOCK_ACCESS_FS_READ ( \
  LANDLOCK_ACCESS_FS_READ_FILE | \
  LANDLOCK_ACCESS_FS_READ_DIR)
#endif /* HAVE_LINUX_LANDLOCK_H */

bool _alpm_sandbox_fs_restrict_writes_to(const char *path)
{
#ifdef HAVE_LINUX_LANDLOCK_H
	struct landlock_ruleset_attr ruleset_attr = {
		.handled_access_fs = \
			_LANDLOCK_ACCESS_FS_READ | \
			_LANDLOCK_ACCESS_FS_WRITE | \
			LANDLOCK_ACCESS_FS_EXECUTE,
	};
	struct landlock_path_beneath_attr path_beneath = {
		.allowed_access = _LANDLOCK_ACCESS_FS_READ,
	};
	int abi = 0;
	int result = 0;
	int ruleset_fd;

	abi = landlock_create_ruleset(NULL, 0, LANDLOCK_CREATE_RULESET_VERSION);
	if(abi < 0) {
		/* landlock is not supported/enabled in the kernel */
		return true;
	}
#ifdef LANDLOCK_ACCESS_FS_TRUNCATE
	if(abi >= 2) {
		ruleset_attr.handled_access_fs |= LANDLOCK_ACCESS_FS_TRUNCATE;
	}
#endif /* LANDLOCK_ACCESS_FS_TRUNCATE */

	ruleset_fd = landlock_create_ruleset(&ruleset_attr, sizeof(ruleset_attr), 0);
	if(ruleset_fd < 0) {
		return false;
	}

	/* allow / as read-only */
	path_beneath.parent_fd = open("/", O_PATH | O_CLOEXEC | O_DIRECTORY);
	path_beneath.allowed_access = _LANDLOCK_ACCESS_FS_READ | LANDLOCK_ACCESS_FS_EXECUTE;

	if(landlock_add_rule(ruleset_fd, LANDLOCK_RULE_PATH_BENEATH, &path_beneath, 0)) {
		result = errno;
	}

	close(path_beneath.parent_fd);

	if(result == 0) {
		/* allow read-write access to the directory passed as parameter */
		path_beneath.parent_fd = open(path, O_PATH | O_CLOEXEC | O_DIRECTORY);
		path_beneath.allowed_access = _LANDLOCK_ACCESS_FS_READ | _LANDLOCK_ACCESS_FS_WRITE | LANDLOCK_ACCESS_FS_EXECUTE;

		if(!landlock_add_rule(ruleset_fd, LANDLOCK_RULE_PATH_BENEATH, &path_beneath, 0)) {
			if(landlock_restrict_self(ruleset_fd, 0)) {
				result = errno;
			}
		} else {
			result = errno;
		}

		close(path_beneath.parent_fd);
	}

	close(ruleset_fd);
	return result == 0;
#else /* HAVE_LINUX_LANDLOCK_H */
	return true;
#endif /* HAVE_LINUX_LANDLOCK_H */
}
