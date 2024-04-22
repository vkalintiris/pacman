self.description = "No partial upgrade all needed packages are instaled"

sp1 = pmpkg("pkg1", "1.0-2")
self.addpkg2db("sync", sp1)

sp2 = pmpkg("pkg2", "1.0-2")
self.addpkg2db("sync", sp2)

lp2 = pmpkg("pkg2")
self.addpkg2db("local", lp2)

self.args = "-S %s %s" % (sp1.name, sp2.name)

self.addrule("PACMAN_RETCODE=0")
self.addrule("PKG_EXIST=pkg1")
self.addrule("PKG_EXIST=pkg2")
