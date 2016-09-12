
#pkgdata_DATA = platform/@PLATFORM@ translations skins/@SCREEN_RES@

install-data-local:
	test -z "$(pkgdatadir)" || $(MKDIR_P) "$(DESTDIR)$(pkgdatadir)"
	$(MKDIR_P) "$(DESTDIR)$(pkgdatadir)/skins"
	cp -r translations "$(DESTDIR)$(pkgdatadir)"
	cp -r skins/@SCREEN_RES@/* "$(DESTDIR)$(pkgdatadir)/skins"
	cp -r platform/@PLATFORM@/* "$(DESTDIR)$(pkgdatadir)"
	ln -r -s "$(DESTDIR)$(pkgdatadir)/scripts/launch" "$(DESTDIR)$(pkgdatadir)/scripts/launch.d"
	ln -r -s "$(DESTDIR)$(pkgdatadir)/scripts/launch" "$(DESTDIR)$(pkgdatadir)/scripts/app.stop"
	ln -r -s "$(DESTDIR)$(pkgdatadir)/scripts/launch" "$(DESTDIR)$(pkgdatadir)/scripts/app.exec"
	ln -r -s "$(DESTDIR)$(pkgdatadir)/scripts/launch" "$(DESTDIR)$(pkgdatadir)/scripts/onHome"
