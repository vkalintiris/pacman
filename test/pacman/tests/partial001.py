self.description = "Partial upgrade"

sp1 = pmpkg("pkg1", "1.0-2")
self.addpkg2db("sync", sp1)

sp2 = pmpkg("pkg2", "1.0-2")
self.addpkg2db("sync", sp2)

lp2 = pmpkg("pkg2")
self.addpkg2db("local", lp2)

self.args = "-S %s" % sp1.name

self.addrule("PACMAN_RETCODE=1")
self.addrule("!PKG_EXIST=pkg1")
