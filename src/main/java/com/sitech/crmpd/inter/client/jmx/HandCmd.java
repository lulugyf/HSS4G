package com.sitech.crmpd.inter.client.jmx;

public class HandCmd {

	private static void usage(String info) {
		System.out.println(info);
		System.out.println("Usage: $0 hss");
		System.exit(0);
	}
	
	public static void main(String[] args) throws Exception {

		if (args.length < 1) {
			usage("invalid arguments");
			return;
		}

		String[] hlr = args[0].split("\\.");
		
		BaseComm com = null;
		if(args.length == 1){
			com = new ConsoleComm();
		}else if(args.length > 1) {
			com = new FileComm(args[1]);
		}
		
		HLRPort h = new HLRPort(hlr[0], hlr[1], com);
		h.run();
	}
}
