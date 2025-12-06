ALL_OBJS = tiny.o text.o wrong_magic.o bad_class.o short_hdr.o bad_sect_tbl.o

all: $(ALL_OBJS)

base_tmp.o:
	cp /bin/ls base_tmp.o

tiny.o:
	printf 'AB' > tiny.o

text.o:
	echo 'this is not an object file' > text.o

wrong_magic.o: base_tmp.o
	cp base_tmp.o wrong_magic.o
	printf 'ABCD' | dd of=wrong_magic.o bs=1 seek=0 conv=notrunc 2>/dev/null

bad_class.o: base_tmp.o
	cp base_tmp.o bad_class.o
	printf '\003' | dd of=bad_class.o bs=1 seek=4 conv=notrunc 2>/dev/null

short_hdr.o: base_tmp.o
	dd if=base_tmp.o of=short_hdr.o bs=1 count=20 2>/dev/null

bad_sect_tbl.o: base_tmp.o
	cp base_tmp.o bad_sect_tbl.o
	printf '\377\377\377\377\377\377\377\377' | dd of=bad_sect_tbl.o bs=1 seek=40 conv=notrunc 2>/dev/null

clean:
	rm -f $(ALL_OBJS) base_tmp.o

fclean: clean

re: fclean all

.PHONY: all clean fclean re
