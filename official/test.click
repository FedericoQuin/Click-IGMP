
clientState::RouterInfoBase()
reportSource::IGMPReport(clientState)

IGMPQuery(0)
	-> IPEncap(2, 192.168.1.20, 224.0.0.1, TTL 1)
	-> EtherEncap(0x0800, 1A:7C:3E:90:78:41, 1A:7C:3E:90:78:42)
	-> ToDump(queries.dump)
	-> Strip(14)
	-> IGMPQueryHandler(clientState, reportSource)

reportSource
	-> IPEncap(2, 192.168.1.20, 224.0.0.22, TTL 1)
	-> EtherEncap(0x0800, 1A:7C:3E:90:78:41, 1A:7C:3E:90:78:42)
	-> ToDump(test.dump)
	-> Discard;