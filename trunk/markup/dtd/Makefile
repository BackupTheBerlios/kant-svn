all: monolith.dtd dtdindex.dtd standalone/standalone.dtd.in

clean:
	rm -f monolith.dtd
	rm -f dtdindex.dtd
	rm -f standalone/standalone.dtd.in

monolith.dtd: monolith.dtd.in dtdindex.dtd.in dtdindex.dtd
	rm -f monolith.dtd
	scripts/make-monolith-dtd.sh

dtdindex.dtd: dtdindex.dtd.in
	rm -f dtdindex.dtd
	scripts/make-dtdindex.sh

dtdindex.dtd.in: dtdindex.dtd.in.in
	mv -f dtdindex.dtd.in dtdindex.dtd.in~
	scripts/make-dtdindex-in.sh

standalone/standalone.dtd.in: standalone/standalone.dtd.in.in
	rm -f standalone/standalone.dtd.in
	scripts/make-standalone-dtds.sh
