self.description = "Partial upgrade from replace"

sp1 = pmpkg("pkg1", "1.0-2")
self.addpkg2db("sync", sp1)

sp2 = pmpkg("pkg2", "1.0-1")
self.addpkg2db("sync", sp2)

sp3 = pmpkg("replace")
sp3.replaces = ["pkg2"]
self.addpkg2db("sync", sp3)

lp2 = pmpkg("pkg2")
self.addpkg2db("local", lp2)

self.args = "-S %s" % sp1.name

self.addrule("!PACMAN_RETCODE=0")
self.addrule("!PKG_EXIST=pkg1")
self.addrule("PKG_EXIST=pkg2")
self.addrule("!PKG_EXIST=replace")

