AddressInfo(sourceAddr 10.0.0.1 1A:7C:3E:90:78:41)
AddressInfo(responderAddr 10.0.0.2 1A:7C:3E:90:78:42)

source::IGMPQuery(MRC 126, QRV 6)



source
	-> IPEncap(2, sourceAddr, responderAddr)
	// -> EtherEncap(0x0800, sourceAddr, responderAddr)
	-> ToDump(output_query_right.dump)
	-> Print("test")
	-> client1_class :: Classifier(12/0806 20/0001, 12/0806 20/0002, -)
	-> Discard

client1_class[1] -> Discard
client1_class[2]
	-> Paint(2)
	-> Strip(14)
	-> CheckIPHeader
	-> Print("test2")
	-> Discard