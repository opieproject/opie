/*
 * Inode based directory notification for Linux
 *
 * Copyright (C) 2005 John McCutchan
 */

#ifndef _LINUX_INOTIFY_H
#define _LINUX_INOTIFY_H

#include <sys/types.h>
#include <linux/limits.h>
#include <linux/types.h>

/*
 * struct inotify_event - structure read from the inotify device for each event
 *
 * When you are watching a directory, you will receive the filename for events
 * such as IN_CREATE, IN_DELETE, IN_OPEN, IN_CLOSE, ..., relative to the wd.
 */
struct inotify_event {
	__s32		wd;		/* watch descriptor */
	__u32		mask;		/* watch mask */
	__u32		cookie;		/* cookie to synchronize two events */
	__u32		len;		/* length (including nulls) of name */
	char		name[0];	/* stub for possible name */
};

/*
 * struct inotify_watch_request - represents a watch request
 *
 * Pass to the inotify device via the INOTIFY_WATCH ioctl
 */
struct inotify_watch_request {
	char		*name;		/* filename name */
	__u32		mask;		/* event mask */
};

/* the following are legal, implemented events */
#define IN_ACCESS		0x00000001	/* File was accessed */
#define IN_MODIFY		0x00000002	/* File was modified */
#define IN_ATTRIB		0x00000004	/* File changed attributes */
#define IN_CLOSE_WRITE		0x00000008	/* Writtable file was closed */
#define IN_CLOSE_NOWRITE	0x00000010	/* Unwrittable file closed */
#define IN_OPEN			0x00000020	/* File was opened */
#define IN_MOVED_FROM		0x00000040	/* File was moved from X */
#define IN_MOVED_TO		0x00000080	/* File was moved to Y */
#define IN_DELETE_SUBDIR	0x00000100	/* Subdir was deleted */ 
#define IN_DELETE_FILE		0x00000200	/* Subfile was deleted */
#define IN_CREATE_SUBDIR	0x00000400	/* Subdir was created */
#define IN_CREATE_FILE		0x00000800	/* Subfile was created */
#define IN_DELETE_SELF		0x00001000	/* Self was deleted */
#define IN_UNMOUNT		0x00002000	/* Backing fs was unmounted */
#define IN_Q_OVERFLOW		0x00004000	/* Event queued overflowed */
#define IN_IGNORED		0x00008000	/* File was ignored */

/* special flags */
#define IN_ALL_EVENTS		0xffffffff	/* All the events */
#define IN_CLOSE		(IN_CLOSE_WRITE | IN_CLOSE_NOWRITE)

#define INOTIFY_IOCTL_MAGIC	'Q'
#define INOTIFY_IOCTL_MAXNR	2

#define INOTIFY_WATCH  		_IOR(INOTIFY_IOCTL_MAGIC, 1, struct inotify_watch_request)
#define INOTIFY_IGNORE 		_IOR(INOTIFY_IOCTL_MAGIC, 2, int)

#ifdef __KERNEL__

#include <linux/dcache.h>
#include <linux/fs.h>
#include <linux/config.h>
#include <asm/atomic.h>

#ifdef CONFIG_INOTIFY

extern void inotify_inode_queue_event(struct inode *, __u32, __u32,
				      const char *);
extern void inotify_dentry_parent_queue_event(struct dentry *, __u32, __u32,
					      const char *);
extern void inotify_super_block_umount(struct super_block *);
extern void inotify_inode_is_dead(struct inode *);
extern u32 inotify_get_cookie(void);

#else

static inline void inotify_inode_queue_event(struct inode *inode,
					     __u32 mask, __u32 cookie,
					     const char *filename)
{
}

static inline void inotify_dentry_parent_queue_event(struct dentry *dentry,
						     __u32 mask, __u32 cookie,
						     const char *filename)
{
}

static inline void inotify_super_block_umount(struct super_block *sb)
{
}

static inline void inotify_inode_is_dead(struct inode *inode)
{
}

static inline u32 inotify_get_cookie(void)
{
	return 0;
}

#endif	/* CONFIG_INOTIFY */

#endif	/* __KERNEL __ */

#endif	/* _LINUX_INOTIFY_H */
