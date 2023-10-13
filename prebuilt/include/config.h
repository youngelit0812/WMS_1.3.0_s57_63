#ifndef OCPN_CONFIG_H
#define OCPN_CONFIG_H

/* #undef LINUX_CRASHRPT */
#define OCPN_USE_CRASHREPORT
/* #undef OCPN_HAVE_X11 */

// Garmin Host Mode support
#define USE_GARMINHOST

#define OCPN_USE_LZMA

#define OCPN_USE_CURL

/* #undef USE_LIBELF */

#define USE_GARMINHOST

#define OCPN_USE_NEWSERIAL

/* #undef ENABLE_VCAN_TESTS */

// Enable dark mode on modern MacOS.
/* #undef OCPN_USE_DARKMODE */

/* #undef ocpnUSE_GLES */


#define OPENGL_FOUND
#undef HAVE_UNISTD_H
#define OCPN_USE_UDEV_PORTS
/* #undef OCPN_USE_SYSFS_PORTS */
/* #undef HAVE_LIBUSB_10 */
/* #undef HAVE_DIRENT_H */
/* #undef HAVE_LIBGEN_H */
/* #undef HAVE_LIBUDEV */
/* #undef HAVE_PORTAUDIO */
/* #undef HAVE_READLINK */
/* #undef HAVE_SNDFILE */
/* #undef HAVE_SYSTEM_CMD_SOUND */
/* #undef HAVE_UNISTD_H */
/* #undef USE_SYSFS_PORTS */
/* #undef HAVE_LINUX_SERIAL_H */
/* #undef HAVE_SYS_STAT_H */
/* #undef HAVE_SYS_TYPES_H */
/* #undef HAVE_SYS_FCNTL_H */
/* #undef HAVE_SYS_IOCTL_H */

// The os compatible plugins are are built for
#define PKG_TARGET "msvc-wx32"

// The os version compatible plugins are are built for
#define PKG_TARGET_VERSION "10"

// The architecture compatible plugins are built for
#define PKG_TARGET_ARCH "x86_64"

// Command line to play a sound in system(3), like "aplay %s".
#define OCPN_SOUND_CMD  "PowerShell (New-Object Media.SoundPlayer \\\"%s\\\").PlaySync();"

// Flag for ancient compilers without C++11 support
/* #undef COMPILER_SUPPORTS_CXX11 */

#define VERSION_MAJOR 5
#define VERSION_MINOR 9
#define VERSION_PATCH 0
#define VERSION_DATE "2023-10-06"
#define VERSION_FULL "5.9.0"
#define PACKAGE_VERSION "5.9.0"

#define DEBIAN_PPA_VERSION ""

#endif  // OCPN_CONFIG_H
