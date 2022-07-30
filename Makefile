override CFLAGS += -Wall #-DDEBUG

TARGET = intel-power-control

PREFIX ?= /usr/local
BINDIR  = $(PREFIX)/bin
DATADIR = $(PREFIX)/share

all: $(TARGET)-helper


install: all
	install -d $(BINDIR)
	install -m 755  $(TARGET) $(BINDIR)
	install -m 755  $(TARGET)-setup $(BINDIR)
ifeq (0, $(shell id -u))
	install -m 4755 $(TARGET)-helper $(BINDIR)
else
	$(warning Cannot install helper as SUID as we are not root)
	install -m 755 $(TARGET)-helper $(BINDIR)
endif
	$(install_helper)
	install -d $(DATADIR)/$(TARGET)
	install -m 644 -t $(DATADIR)/$(TARGET) misc/intel-power-control.png
	install -d $(DATADIR)/applications
	install -m 644 -t $(DATADIR)/applications \
		misc/intel-power-control.desktop
	install -d $(DATADIR)/icons/hicolor/32x32
	install -m 644 -t $(DATADIR)/icons/hicolor/32x32 \
		misc/intel-power-control.png
	install -d $(DATADIR)/icons/hicolor/scalable
	install -m 644 -t $(DATADIR)/icons/hicolor/scalable \
		misc/intel-power-control.svg

uninstall:
	rm -f $(BINDIR)/$(TARGET)
	rm -f $(BINDIR)/$(TARGET)-setup
	rm -f $(BINDIR)/$(TARGET)-helper
	rm -rf $(DATADIR)/$(TARGET)
	rm -f $(DATADIR)/applications/intel-power-control.desktop
	rm -f $(DATADIR)/icons/hicolor/32x32/intel-power-control.png
	rm -f $(DATADIR)/icons/hicolor/scalable/intel-power-control.svg

clean:
	rm -f $(TARGET)-helper *~
