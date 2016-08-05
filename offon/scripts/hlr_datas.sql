insert into cBossUserInfo values('${hlrstr}', 'onoffdb', 'offon', 'Crmpd!06', 'sndcmdday${hlrstr}', '${hlrstr}');

insert into cQueryParse(hlr_code, command_code, order_code, src_beg, data_len, dest_station, dest_beg, note)
    select '${hlrstr}', command_code, order_code, src_beg, data_len, dest_station, dest_beg, note from cQueryParse
  where hlr_code='${copyhlr}';

insert into cQueryRevise(hlr_code, command_code, order_code, reply_code, func_code,
                status_flag, info_flag, status_addr, status_null, info_beg, info_len, note)
        select '${hlrstr}', command_code, order_code, reply_code, func_code,
                status_flag, info_flag, status_addr, status_null, info_beg, info_len, note
  from cQueryRevise where hlr_code='${copyhlr}';

insert into cOrderResend
        select '${hlrstr}', order_code, func_code from cOrderResend where hlr_code='${copyhlr}';

insert into cOrderReplyParse
        select '${hlrstr}', command_code, src_order_code, src_reply_code, parse_result, revise, dest_order, serial, order_type
                 from cOrderReplyParse where hlr_code='${copyhlr}';

insert into cCmdToOrder select '${hlrstr}', command_code, command_priority, order_type, order_code, serial, note
        from cCmdToOrder where hlr_code='${copyhlr}';

insert into cMainCfgInfo(hlr_code, ip_addr, comm_port, usrdata_mgr, note)
  values('${hlrstr}', '127.0.0.1', (select max(comm_port)+1 from cMainCfgInfo), 'n', '${hlrstr}');

insert into cphonehlrcode
   select phone_head, '${hlrstr}', phone_type, region_code, note from cphonehlrcode where hlr_code='${copyhlr}';

insert into cLoginUser(user_name, user_pswd, hlr_code, hlr_port, term_type, encrypt, term_id)
  values('hlr${hlrstr}', 'hlr${hlrstr}', '${hlrstr}', '1', '1', '0', 'hlr${hlrstr}');

insert into cLoginUser(user_name, user_pswd, hlr_code, hlr_port, term_type, encrypt, term_id)
  values('hlr${hlrstr}', 'hlr${hlrstr}', '${hlrstr}', '8', '3', '0', 'hlr${hlrstr}');
