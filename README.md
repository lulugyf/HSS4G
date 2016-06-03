# HSS4G

开关机SOAP指令客户端代码

1. hss.cfg 改为 hss.yaml
2. order 目录改为 .yaml
3. 爱立信ip hex->oct 转换
4. gprs apn 的 域名到templdateid的转换
5. 查询指令结果中解析数据作为后一条指令的参数
6. 进程合并， 使用 jolokia 做jmx代理， 在Control 进程中起停工作线程
7. 各工作线程分别记录日志文件

详情查看readme.txt


## change.log:

## 0.1.5
1. 对于网元URL连接， 采用连接池模式
2. 网元连接数量限制设置， 在启动参数中增加 -DMAXURLCONN=?? -DMAXURLCONNTOTAL=??
    来设置连接数限制， 前者是对单个服务器地址的连接数限制， 后者为总数限制
3. 返回内容映射错误代码的处理， 增加配置 result.map 指向一个文件， 文件格式为每行一个代码, 如下， 匹配方式为 indexOf
  > nnnn content
4. -DCONF=??? 可以指定 hss.yaml 文件的路径， 同时兼容以前不指定的情况： 手工执行命令取当前路径下的hss.yaml, 运行环境取env("ETCDIR")/hss.yaml
