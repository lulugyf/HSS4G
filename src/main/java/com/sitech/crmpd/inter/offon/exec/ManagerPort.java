package com.sitech.crmpd.inter.offon.exec;

import com.sitech.crmpd.inter.offon.mgr.*;
import org.slf4j.Logger;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.Map;

/**
 * Created by guanyf on 7/31/2016.
 */
public class ManagerPort extends BasePort {
    public ManagerPort(String basePath, String portname, Map conf, ConfigureManager cm){
        super(basePath, portname, conf, cm);
    }

    private String manager_ip;
    private int manager_port;
    private String manager_user;
    private String manager_password;
    private String term_type;

    // 与 offon_manager 建立连接, 并登录
    private int transid = 0;
    private Socket sock = null;
    private InputStream iStream = null;
    private OutputStream oStream = null;
    private byte[] buf = new byte[2048];

    protected boolean connect(Logger log) {

        manager_ip = String.valueOf(conf.get("manager.ip"));
        manager_port = Integer.parseInt(String.valueOf(conf.get("manager.port")));
        manager_user = String.valueOf(conf.get("manager.user"));
        manager_password = String.valueOf(conf.get("manager.password"));
        term_type = String.valueOf(conf.get("term.type"));
        final LoginReq req = new LoginReq();
        final LoginAck ack = new LoginAck();

        // haa.1
        req.hlrcode = portname.substring(0, 3);
        req.hlrport = portname.charAt(4);
        req.termtype = term_type.charAt(0);
        req.username = manager_user;
        req.userpswd = manager_password;
        final byte[] buf = new byte[100];
        req.toByteArray(buf, transid++);
        try {
            sock = new Socket(manager_ip, manager_port);
            iStream = sock.getInputStream();
            oStream = sock.getOutputStream();
            BasePkt.write_pkt(oStream, buf);
            BasePkt.read_pkt(iStream, buf);
            ack.fromByteArray(buf);
            log.info("login return [{}]", ack.retn);
            if (ack.retn == 0) {
                return true;
            }
        } catch (final Exception ex) {
            log.error("connect(" + manager_ip + ":" + manager_port + ") failed", ex);
        }
        return false;
    }


    protected int getOrder(CmdDataReq req, CmdDataAck ack, Logger log) {
        int len = -1;
        try {
            len = req.toByteArray(buf, transid++);
            BasePkt.write_pkt(oStream, buf);
            len = BasePkt.read_pkt(iStream, buf);
            if (len > 0) {
                len = ack.fromByteArray(buf);
            }
        } catch (final IOException ex) {
            log.error("", ex);
            len = -1;
        }
        return len;
    }


    protected void close(Logger log) {
        try {
            if (iStream != null) {
                iStream.close();
            }
            if (oStream != null) {
                oStream.close();
            }
            if (sock != null) {
                sock.close();
            }
        } catch (final Exception ex) {
            log.error("", ex);
        }
    }
}
