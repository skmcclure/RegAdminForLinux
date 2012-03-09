
dtd_entity_data = \
	xhtml-lat1.ent \
	xhtml-special.ent \
	xhtml-symbol.ent \
	ISOamsa.ent \
	ISOamsb.ent \
	ISOamsc.ent \
	ISOamsn.ent \
	ISOamso.ent \
	ISOamsr.ent \
	ISObox.ent \
	ISOcyr1.ent \
	ISOcyr2.ent \
	ISOdia.ent \
	ISOgrk1.ent \
	ISOgrk2.ent \
	ISOgrk3.ent \
	ISOgrk4.ent \
	ISOlat1.ent \
	ISOlat2.ent \
	ISOnum.ent \
	ISOpub.ent \
	ISOtech.ent

EXTRA_DIST = \
	$(dtd_entity_data) \
	uchar_info_data.c \
	convert-entities

all-local: uchar_info_data.c

uchar_info_data.c: convert-entities $(dtd_entity_data)
	./convert-entities $^ > $@

MAINTAINERCLEANFILES = uchar_info_data.c
