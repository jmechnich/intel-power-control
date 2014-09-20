CFLAGS=-Wall #-DDEBUG

TARGET=intel-power-control
HELPER=$(TARGET)-helper

INSTDIR=/usr/local/bin

all: $(HELPER)

install: all
	install -m 755  $(TARGET)  $(INSTDIR)
	install -m 4755 $(HELPER) $(INSTDIR)

uninstall:
	rm -f $(INSTDIR)/$(TARGET)
	rm -f $(INSTDIR)/$(HELPER)

clean:
	rm -f $(HELPER) *~
