self.description = "No partial upgrade from -U"

p1 = pmpkg("pkg1", "1.0-2")
self.addpkg(p1)

sp2 = pmpkg("pkg2", "1.0-2")
self.addpkg2db("sync", sp2)

lp2 = pmpkg("pkg2")
self.addpkg2db("local", lp2)

self.args = "-U %s" % p1.filename()

self.addrule("PACMAN_RETCODE=0")
self.addrule("PKG_EXIST=pkg1")
self.addrule("PKG_EXIST=pkg2")
self.addrule("PKG_VERSION=pkg2|1.0-1")

