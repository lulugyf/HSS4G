package com.sitech.crmpd.inter.offon.exec;

import com.sitech.crmpd.inter.offon.mgr.CmdDataAck;
import com.sitech.crmpd.inter.offon.mgr.CmdDataReq;
import org.apache.commons.lang3.StringUtils;
import org.slf4j.Logger;

import java.text.MessageFormat;
import java.text.ParseException;
import java.util.Map;
import java.util.Scanner;

/**
 * Created by guanyf on 7/31/2016.
 */
public class ConsolePort extends BasePort {
    private Scanner scanner;
    private int lineno = 0;
    private final MessageFormat format = new MessageFormat("{0}|{1}|{2}|{3}|{4}|{5}");


    public ConsolePort(String basePath, String portname, Map conf, ConfigureManager cm){
        super(basePath, portname, conf, cm);
    }

    @Override
    protected boolean connect(Logger log) {
        scanner = new Scanner(System.in);
        maxExec = 1;
        return true;
    }

    @Override
    protected int getOrder(CmdDataReq req, CmdDataAck ack, Logger log) {
        String line;
        while (true) {
            System.out.println("请输入请求参数");
            line = scanner.nextLine();
            if (line.equalsIgnoreCase("quit") || line.equalsIgnoreCase("exit") || line.equalsIgnoreCase("q")) {
                return -1;
            }
            if (StringUtils.isBlank(line)) {
                continue;
            }
            Object[] p = null;
            try {
                p = format.parse(line.trim());
            } catch (ParseException e) {
                e.printStackTrace();
            }
            ack.ordercode = (String)p[0];
            ack.phone_no =  (String)p[1];
            ack.imsi_no =  (String)p[2];
            ack.ss_info1 = (String)p[3];
            ack.ss_info2 = (String)p[4];
            ack.ss_info3 = (String)p[5];
            ack.stream_id = String.valueOf(lineno++);
            ack.retn = 2001;
            return 1;
        }
    }

    @Override
    protected void close(Logger log) {
        scanner.close();
    }
}
