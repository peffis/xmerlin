####################################################################
########        Merlin Makefile                             ########
####################################################################


####### Flags
# Where characters are installed
SAMPLES_INSTALL_DIR =  /usr/local/share/merlin

# Where the binary is installed
INSTALL_DIR =   /usr/X11R6/bin

INCFLAGS    =   -I.
CFLAGS      =   -O2 -Wall -DDEFAULT_HOME=\"$(SAMPLES_INSTALL_DIR)\" -DUSE_ENGLISH
LDFLAGS     =  	-lm -L/usr/X11R6/lib -lX11
CC          =   gcc
STRIP       =   strip
BACKUP_NAME = 	xmerlin.tar.gz
KEYMAPS     =   keymap/swedish-kbd.map keymap/english-kbd.map

####### Files

OBJECTS     =   filter.o pattern.o preprocessor.o merlin.o elasticWizard.o \
		event.o graphics.o mapping.o
TARGET      = 	merlin

####### Default rules

.SUFFIXES: 		.c

.c.o:
			$(CC) -c $(CFLAGS) $(INCFLAGS) $<


####### Build rules

all: 			$(TARGET)

$(TARGET):		$(OBJECTS) 
			$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

pattern.o:		def.h pattern.h point.h pattern.c
			$(CC) -c $(CFLAGS) $(INCFLAGS) pattern.c

mapping.o:		mapping.c $(KEYMAPS)
			$(CC) -c $(CFLAGS) $(INCFLAGS) mapping.c

elasticWizard.o: 	def.h pattern.h wizard.h point.h elasticWizard.c
			$(CC) -c $(CFLAGS) $(INCFLAGS) elasticWizard.c

backup:
			make distclean;\
			cd ..;\
			tar cvzf $(BACKUP_NAME) xmerlin ; \
			mv $(BACKUP_NAME) xmerlin/

clean:	
			rm -f *.o *~ core nil*

distclean:
			rm -f *.o *~ core nil* $(TARGET)

install:
			$(STRIP) $(TARGET)
			install -d $(SAMPLES_INSTALL_DIR)
			install samples/*.dat $(SAMPLES_INSTALL_DIR)
			install merlin $(INSTALL_DIR)

