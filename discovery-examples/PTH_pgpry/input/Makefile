# Very lightweight execution, the pass phrase is just "discovery"
discovery.out : discovery.re discovery.gpg
	$(PGPRY) --no-resume --prefixes=iscovery --regexes=discovery.re \
	      < discovery.gpg > discovery.out

clean:
	rm -rf discovery.out state trace *~
