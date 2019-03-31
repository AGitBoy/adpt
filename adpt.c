/*
 * Copyright (C) 2019 Aidan Williams
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <stdlib.h>
#include <stdio.h>

#include <errno.h>
#include <getopt.h>
#include <string.h>

#if defined(__FreeBSD__)
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <dev/acpica/acpiio.h>
#include <libexplain/ioctl.h>
#endif

static const char *versionstr =
	"adpt v1.0\n"
	"Copyright (C) 2019 Aidan Williams\n"
	"License GPLv3+: GNU GPL version 3 or later "
	"<https://gnu.org/licenses/gpl.html>\n"
	"This is free software: you are free to change and redistribute it.\n"
	"There is NO WARRANTY, to the extent permitted by the law.\n\n"
	"Written by Aidan Williams";

static const char *usagestr =
	"Usage: %s [OPTIONS]\n\n"
	"-v, --version    \tPrints the version information\n"
	"-h, --help       \tPrints this help text\n"
	"-o, --on-string  \tSpecifies the string used when adapter is plugged in\n"
	"-f, --off-string \tSpecifies the string used when adapter isn't plugged in\n";

static struct option long_options[] = {
	{ "help",       no_argument,       NULL, 'h' },
	{ "version",    no_argument,       NULL, 'v' },
	{ "on-string",  required_argument, NULL, 'o' },
	{ "off-string", required_argument, NULL, 'f' },
	{ NULL,         0,                 NULL, 0   }
};


// Prints usage information
static void usage(char *progpth, int err)
{
	fprintf(err ? stderr : stdout, usagestr, progpth);
	exit(err);
}


// Prints version information
static void version()
{
	puts(versionstr);
	exit(EXIT_SUCCESS);
}


#if defined(__linux) 
// Internal for getting status of sysfs adapters
int sysfsadptstat()
{
	FILE* fptr;
	int plugged; 
	
	if ((fptr = fopen("/sys/class/power_supply/ADP0/online", "r"))) {
		fscanf(fptr, "%d", &plugged);
		fclose(fptr);
	} else {
		fprintf(
			stderr,
			"Error opening file: %s",
			"/sys/class/power_supply/ADP0/online"
		);

		exit(EIO);
	}
	
	return plugged;
}
#endif

#if defined(__FreeBSD__)
int bsdadptstat() 
{
	static int acpi;
	int status;

	acpi = open("/dev/acpi", O_RDWR);
	if (acpi < 0)
		acpi = open("/dev/acpi", O_RDONLY);
	
	if (acpi < 0) {
		fprintf(stderr, "%s: %s\n", strerror(errno), "/dev/acpi");
		exit(errno);	
	}
	
	if (ioctl(acpi, ACPIIO_ACAD_GET_STATUS, &status) < 0) {
		fprintf(
			stderr,
			"ioctl error for adapter: %s",
			explain_ioctl(acpi, ACPIIO_ACAD_GET_STATUS, &status)
		);

		exit(EXIT_FAILURE);
	}

	return status;
}
#endif

// Gets the status of adpater
int adptstatus()
{
#if defined(__linux)
	return sysfsadptstat();
#elif defined(__FreeBSD__)
	return bsdadptstat();
#else
	fputs("Unsupported platform");
	exit(EXIT_FAILURE);
#endif
}


int main(int argc, char **argv)
{
	int opt;
	char *onstr = ONSTR, *offstr = OFFSTR;

	while ((opt = getopt_long(argc, argv, ":hvf:o:", long_options, NULL)) != -1) {
		switch (opt) {
		case 'h':
			usage(argv[0], EXIT_SUCCESS);
		case 'v':
			version();
		case 'o':
			onstr = optarg;
			break;
		case 'f':
			offstr = optarg;
			break;
		default:
			usage(argv[0], EINVAL);
		}
	}
	
	puts(adptstatus() ? onstr : offstr);

	return 0;
}
