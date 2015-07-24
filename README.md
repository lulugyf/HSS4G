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
