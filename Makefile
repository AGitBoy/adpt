# Copyright (C) 2019 Aidan Williams
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

PREFIX   ?= /usr/local
BINDIR   ?= bin
MANDIR   ?= man/man1
DOCDIR   ?= share/doc/adpt
BINPERMS ?= 755
DOCPERMS ?= 644

STD ?= c99
CC  ?= cc

SOURCES := $(shell find -name "*.c")

adpt: $(SOURCES)
	$(CC) -std=$(STD) $(CFLAGS) -o adpt $(SOURCES)

debug: $(SOURCES)
	$(CC) -std=$(STD) $(CFLAGS) -g -o adpt $(SOURCES)

clean:
	[ ! -f adpt ] || rm adpt
	[ ! -f docs/adpt.1 ] || rm docs/adpt.1

install: adpt
	install -d $(DESTDIR)$(PREFIX)/$(BINDIR)
	install -m $(BINPERMS) adpt $(DESTDIR)$(PREFIX)/$(BINDIR)
	install -d $(DESTDIR)$(PREFIX)/$(DOCDIR)
	install -m $(DOCPERMS) COPYING $(DESTDIR)$(PREFIX)/$(DOCDIR)
	install -m $(DOCPERMS) README.md $(DESTDIR)$(PREFIX)/$(DOCDIR)
	[ ! -f docs/adpt.1 ] || \
		(install -d $(DESTDIR)$(PREFIX)/$(MANDIR) && \
		install -m $(DOCPERMS) docs/adpt.1 $(DESTDIR)$(PREFIX)/$(MANDIR) && \
		gzip -f $(DESTDIR)$(PREFIX)/$(MANDIR)/adpt.1)

run: adpt
	./adpt

.PHONY: docs
docs: docs/adpt.1.ronn
	ronn -r docs/adpt.1.ronn

