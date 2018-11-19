# MySql学习文档

## MySql安装


在[MySql](https://dev.mysql.com/downloads/file/?id=479861 "MySql")网站上下载对应在线安装包(`mysql-installer-web-community`)，或者离线安装包（`mysql-installer-community`）,下载后一路next安装即可。


MySql的Root用户密码设置为：123456@163.com

新建一个用户：hao，登陆密码为：123456

## MySql整体架构介绍


![MySql_Architecture](http://pd17tlo0a.bkt.clouddn.com/MySql_Architecture.png)

MySql的核心组件：

1. 连接池：用于用户认证、线程重用、并发连接限制、 并发连接数限制、内存检测以及连接缓存。

2. SQL接口：适用SQL语言对数据库进行操作

3. Parser：SQL语句解析器，用来进行查询转换、权限分析获取

4. Optimizer：优化器，生成并选择最佳的访问路径。

5. Cache&Buffers：与存储引擎相关的I/O性能提升工具

6. 存储引擎：真正执行数据存储和索引管理的工具，常见的有`InnoDB`,`MyISAM`,`NDB`等。逻辑上的数据表体现为在磁盘上的文件，该文件的内部组织结构、存储机制和索引方案等，均由存储引擎决定。

## MySql Router介绍

`MySQL Router`是`InnoDB cluster`的一部分，它是一种轻量级的、在应用程序和后端MySql服务器之间提供透明路由的中间件。为了最佳性能，`MySQL Router`安装在同应用程序同一主机上，原因如下：

1. 允许应用以UNIX域套接字取代TCP/IP，来连接`MySQL Router`。

2. 减少网络延迟

3. 方便应用服务器水平扩展

4. 允许`MySQL Router`无需额外的路由器主机账户连接`MySQL`

    ![MySql_Router_Positioning](http://pd17tlo0a.bkt.clouddn.com/mysql-router-positioning.png)


## MySql存储引擎 ##

### Innodb ###
Innodb自从5.7版本后就成为MySql默认存储引擎，**支持事务和外键**，占用磁盘空间保存索引，写的效率比MyiSam低，**锁机制为行锁**，默认索引为BTREE索引。frm表定义，表空间数据和日志文件。

Innodb适用场合：

* 更新密集的表。Innodb适合处理**高并发的更新请求**
* **事务**。Innodb支持事务
* 自动灾难恢复。能够自动从在灾难中恢复。
* 外键约束。
* 支持自增属性AutoIncrement 

### MyISAM ###
MyISAM引擎独立于操作系统，不支持事务和外键，**锁机制为表锁**，访问速度快,默认索引为BTREE索引，例如，建立一个MyISAM引擎的tb_Demo表，那么会生成以下三个文件：

* tb_demo.frm 存储表定义
* tb_demo.myd 存储表数据
* tb_demo.myi 存储表索引

MyISAM适用场合：

* 选择密集型表，MyISAM在筛选大量数据时非常迅速
* 插入密集型表，MyISAM在并发插入特性允许同时选择和插入数据，适合管理邮件和Web服务器日志数据。 

### Memory ###
Memory引擎的数据存放在内存中，访问速度很快，当mysqld守护进程崩溃时，所有Memory数据都会丢失，它要求存储数据为长度不变的格式，因此无法适用BLOB和TEXT这样长度可变的数据类型，VarChar在MySql内部当作长度固定不变的CHAR类型，可以用在此处。默认为hash索引。

Memory适用常见：

* 数据量较小，而且被频繁的访问。数据全存储在内存中，可通过参数`max_heap_table_size`来调整Memory表的大小。
* 对于一些临时的、要求必须立即可用的数据
* 存储在表中的数据突然丢失，不会对应用服务产生实质的负面影响
* Memory同时支持hash索引和B树索引。

   hash索引：进行相等比较很快，对范围比较较慢，适用操作 = 和 <> 操作符中，不适合 <,>,order by 子句。
   
   B树索引：可使用部分查询和通配查询，也可使用 <,>和>=等操作符。


查看支持的所有存储引擎：`show engines;`

查看当前存储引擎：`show variables like "%storage_engine%";`

设置存储引擎：
1. 可在数据库配置文件中设置默认引擎，`default_storage_engine`
2. 可在创建表的时候选择引擎，create table(...)engine=InnoDB;

选择引擎考虑的着力点：

1. 是否需要支持事务
2. 是否需要使用热备
3. 崩溃恢复，能否接收崩溃
4. 是否需要外键支持
5. 存储的限制
6. 对索引和缓存的支持


## Navicat使用技巧

Ctrl+F只能搜索本页数据，默认一页展示1000条数据，超过1000条数据会分页展示。

Navicat中按字段排序不会影响数据库里面对数据排序，只是显示的顺序发生变化而已。

Ctrl+D快捷键，显示表结构。

Ctrl+O快捷键，显示表数据。

Ctrl+Q快捷键，切换到表查询