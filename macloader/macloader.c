#define LOG_TAG "macloader"
#define LOG_NDEBUG 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>

#include <cutils/log.h>

/* macloader for Samsung S5 LTE Plus, G901F.
 *
 * The Samsung proprietary macloader uses an undocumented way to load
 * the address stored in /efs/wifi/.mac.info into the wlan driver.
 * The permissions required for this method collides with default
 * SELinux settings since Android 7.
 *
 * What we do here instead is to implement our own macloader.
 *
 * The wlan driver fetches its fixed MAC addresses from NVRAM.  Then it
 * checks if it can open a firmware file with the relative path
 * wlan/qca_cld/wlan_mac.bin.  This file in /system/etc/firmware is a
 * symlink pointing to /persist/wlan_mac.bin.
 *
 * So our macloader tries to open /efs/wifi/.mac.info.  If this file
 * contains a valid MAC, it copies this over into /persist/wlan_mac.bin.temp
 * in the required format.  On success it renames the file to it's final
 * name /persist/wlan_mac.bin.
 */

#define MAC_EFSINFO_PATH	"/efs/wifi/.mac.info"
#define MAC_PERSIST_PATH	"/persist/wlan_mac.bin.temp"
#define MAC_PERSIST_PATH_FINAL	"/persist/wlan_mac.bin"

#define MACINFO_LEN 17
#define MACINFO_FMT "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx"

#define MACBIN_LEN 33
#define MACBIN_FMT "Intf0MacAddress=%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\nEND\n"

int
main ()
{
  struct stat st;
  FILE *efsinfo_fp, *persist_fp;
  char infomac[MACINFO_LEN + 1];
  char *got;
  uint8_t hex[6];
  int ret;

  /* If the .mac.info file doesn't exist, we're done. */
  if (access (MAC_EFSINFO_PATH, F_OK))
    {
      ALOGD ("%s doesn't exist, exit\n", MAC_EFSINFO_PATH);
      return 0;
    }

  /* If the wlan_mac.bin file already exists and is at least MACBIN_LEN bytes,
     don't overwrite.  The size check is done to make sure the file is using
     the right format.  An earlier version of macloader neglected to write
     the "END" line, so the files were 4 bytes shorter. */
  if (!stat (MAC_PERSIST_PATH_FINAL, &st) && st.st_size >= MACBIN_LEN)
    {
      ALOGD ("%s exists and is big enough (%jd >= %d bytes), exit\n",
	     MAC_PERSIST_PATH_FINAL, (intmax_t) st.st_size, MACBIN_LEN);
      return 0;
    }

  /* Open .mac.info source file. */
  efsinfo_fp = fopen (MAC_EFSINFO_PATH, "r");
  if (!efsinfo_fp)
    {
      fprintf (stderr, "open(%s) failed (errno=%d)\n", MAC_EFSINFO_PATH, errno);
      ALOGE ("Can't open %s (errno %d)\n", MAC_EFSINFO_PATH, errno);
      return 1;
    }

  /* Read MAC. */
  memset (infomac, 0, sizeof infomac);
  got = fgets (infomac, sizeof infomac, efsinfo_fp);
  fclose (efsinfo_fp);
  if (!got)
    {
      fprintf (stderr, "reading from file %s failed (errno=%d)\n",
	       MAC_EFSINFO_PATH, errno);
      ALOGE ("Can't read from %s (errno %d)\n", MAC_EFSINFO_PATH, errno);
      return 1;
    }

  /* Convert MAC to six hex values and check for correctness. */
  if (sscanf (infomac, MACINFO_FMT, &hex[0], &hex[1], &hex[2],
				    &hex[3], &hex[4], &hex[5])
      != 6
      || (hex[0] == 0x00 && hex[1] == 0x00 && hex[2] == 0x00
	  && hex[3] == 0x00 && hex[4] == 0x00 && hex[5] == 0x00)
      || (hex[0] == 0xff && hex[1] == 0xff && hex[2] == 0xff
	  && hex[3] == 0xff && hex[4] == 0xff && hex[5] == 0xff))
    {
      fprintf (stderr, "Invalid MAC in file %s\n", MAC_EFSINFO_PATH);
      ALOGE ("Got invalid MAC from %s\n", MAC_EFSINFO_PATH);
      return 1;
    }

  /* Open wlan_mac.bin.temp file for writing.  Make sure umask is only
     allowing 640 permissions. */
  umask (0027);
  persist_fp = fopen (MAC_PERSIST_PATH, "w");
  if (!persist_fp)
    {
      fprintf (stderr, "open(%s) failed (errno=%d)\n", MAC_PERSIST_PATH, errno);
      ALOGE ("Can't open %s (errno %d)\n", MAC_PERSIST_PATH, errno);
      return 1;
    }

  /* Write out MAC in wlan_mac.bin format. */
  ret = fprintf (persist_fp, MACBIN_FMT, hex[0], hex[1], hex[2],
					 hex[3], hex[4], hex[5]);
  fclose (persist_fp);
  if (ret != MACBIN_LEN)
    {
      fprintf (stderr, "writing to file %s failed (errno=%d)\n",
	       MAC_PERSIST_PATH, errno);
      ALOGE ("Can't write to %s (errno %d)\n", MAC_PERSIST_PATH, errno);
      unlink (MAC_PERSIST_PATH);
      return 1;
    }
  if (rename (MAC_PERSIST_PATH, MAC_PERSIST_PATH_FINAL))
    {
      fprintf (stderr, "renaming file %s to %s failed (errno=%d)\n",
	       MAC_PERSIST_PATH, MAC_PERSIST_PATH_FINAL, errno);
      ALOGE ("Can't rename %s to %s (errno %d)\n",
	     MAC_PERSIST_PATH, MAC_PERSIST_PATH_FINAL, errno);
      unlink (MAC_PERSIST_PATH);
      return 1;
    }
  return 0;
}
