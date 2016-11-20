AddressInfo(sourceAddr 10.0.0.1 1A:7C:3E:90:78:41)
AddressInfo(responderAddr 10.0.0.2 1A:7C:3E:90:78:42)

source::IGMPQuery(MRC 126, QRV 6)



source
	-> IPEncap(2, sourceAddr, responderAddr)
	-> EtherEncap(0x0800, sourceAddr, responderAddr)
	-> ToDump(output_query.dump)
	-> Discard