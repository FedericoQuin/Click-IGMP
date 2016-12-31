
clientState::ClientInfoBase()
reportSource::IGMPReportGenerator(clientState)
queryHandler::IGMPQueryHandler(clientState, reportSource)

IGMPQueryGenerator(10)
//	-> IPEncap(2, 192.168.1.20, 224.0.0.22, TTL 1)
//	-> IPEncapIGMPQuery(192.168.1.20)
//	-> EtherEncap(0x0800, 1A:7C:3E:90:78:41, 1A:7C:3E:90:78:42)
//	-> ToDump(queries.dump)
//	-> Discard


	-> IPEncapIGMPQuery(192.168.1.20)
	-> EtherEncap(0x0800, 1A:7C:3E:90:78:41, 1A:7C:3E:90:78:42)
	-> ToDump(queries.dump)
	-> Strip(14)
	-> rt :: StaticIPLookup(
			224.0.0.0/4 0,
			0.0.0.0/0 1)

	rt[0]
		-> ipclass::IPClassifier(ip proto IGMP,-)

	rt[1]
		-> ToDump(dumpedPacketsMulticast.dump)
		-> Discard;

	ipclass[0]
		-> queryHandler;

	ipclass[1]
		-> ToDump(dumpedPacketsIGMP.dump)
		-> Discard


reportSource
	-> IPEncap(2, 192.168.1.20, 224.0.0.22, TTL 1)
	-> EtherEncap(0x0800, 1A:7C:3E:90:78:41, 1A:7C:3E:90:78:42)
	-> ToDump(reports.dump)
	-> Discard;