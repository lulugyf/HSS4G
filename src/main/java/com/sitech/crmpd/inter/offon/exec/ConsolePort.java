package com.sitech.crmpd.inter.offon.exec;

import com.sitech.crmpd.inter.offon.mgr.CmdDataAck;
import com.sitech.crmpd.inter.offon.mgr.CmdDataReq;
import org.apache.commons.lang3.StringUtils;
import org.slf4j.Logger;

import java.text.MessageFormat;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Map;
import java.util.Scanner;

/**
 * Created by guanyf on 7/31/2016.
 */
public class ConsolePort extends BasePort {
    private Scanner scanner;
    private int lineno = 0;
    private static final MessageFormat format = new MessageFormat("{0}|{1}|{2}|{3}|{4}|{5}");


    public ConsolePort(String basePath, String portname, Map conf, ConfigureManager cm){
        super(basePath, portname, conf, cm);
    }

    @Override
    protected boolean connect(Logger log) {
        scanner = new Scanner(System.in);
        maxExec = 1;
        return true;
    }
    private ArrayList<String> orders = new ArrayList<String>();
    private volatile Object[] args;

    @Override
    protected int getOrder(CmdDataReq req, CmdDataAck ack, Logger log) {
        String line;
        while (true) {
            if(orders.size() > 0){ //取保存下的order
                ack.ordercode = orders.remove(0);
                assign(args, ack);
                return 1;
            }
            System.out.println("Input(q for quit)>");
            line = scanner.nextLine();
            if (line.equalsIgnoreCase("quit") || line.equalsIgnoreCase("exit") || line.equalsIgnoreCase("q")) {
                return -1;
            }
            if (StringUtils.isBlank(line)) {
                continue;
            }
            Object[] p = null;
            try {
                p = format.parse(line);
            } catch (ParseException e) {
                e.printStackTrace();
            }
            if(p == null || p.length < 6)
                continue;

            String order = (String) p[0];
            if(order.indexOf(',') > 0){ //支持多个ordercode 以 , 分隔
                String[] _orders = order.split("\\,");
                ack.ordercode = _orders[0];
                for(int i=1; i<_orders.length; i++)
                    this.orders.add(_orders[i]);
                this.args = p;
            }else{
                ack.ordercode = order;
            }
            assign(p, ack);
            return 1;
        }
    }

    private void assign(Object[] p, CmdDataAck ack) {
        ack.phone_no =  (String)p[1];
        ack.imsi_no =   (String)p[2];
        ack.ss_info1 =  (String)p[3];
        ack.ss_info2 =  (String)p[4];
        ack.ss_info3 =  (String)p[5];
        ack.stream_id = String.valueOf(lineno++);
        ack.retn = 1;
    }

    @Override
    protected void close(Logger log) {
        scanner.close();
    }

    public static void main(String[] args) throws Exception {
        String line = "9101,9201|15700240214|460077296271684|||";

        Object[] p = format.parse(line); //line.split("\\|");
        System.out.println("====[" + p.length);
        System.out.println(p[4]);
    }
}
