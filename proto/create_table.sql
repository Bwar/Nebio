CREATE DATABASE IF NOT EXISTS `db_analysis`;
USE db_analysis;

CREATE TABLE IF NOT EXISTS `tb_user`(
    stat_date date NOT NULL DEFAULT '2018-03-05' COMMENT '统计目标日期',
    app_id int(11) unsigned NOT NULL DEFAULT 0 COMMENT '应用ID',
    channel varchar(32) NOT NULL DEFAULT '' COMMENT '来源渠道',
    tag varchar(32) NOT NULL DEFAULT '' COMMENT '标签',
    user_type tinyint(4) unsigned NOT NULL DEFAULT 0 COMMENT '用户类型',
    uv int(11) unsigned NOT NULL DEFAULT 0 COMMENT '用户数',
    pv int(11) unsigned NOT NULL DEFAULT 0 COMMENT '次数',
    vv int(11) unsigned NOT NULL DEFAULT 0 COMMENT '会话数',
    iv int(11) unsigned NOT NULL DEFAULT 0 COMMENT '独立IP数',
    session_len bigint(20) NOT NULL DEFAULT 0 COMMENT '总会话时长(秒)',
    PRIMARY KEY stat_key(stat_date, app_id, channel, tag, user_type)
) ENGINE=InnoDB, CHARSET=utf8mb4, COMMENT='用户统计结果表';

CREATE TABLE IF NOT EXISTS `tb_event`(
    stat_date date NOT NULL DEFAULT '2018-03-05' COMMENT '统计目标日期',
    app_id int(11) unsigned NOT NULL DEFAULT 0 COMMENT '应用ID',
    channel varchar(32) NOT NULL DEFAULT '' COMMENT '来源渠道',
    tag varchar(32) NOT NULL DEFAULT '' COMMENT '标签',
    event_id varchar(64) NOT NULL DEFAULT '' COMMENT '标签',
    uv int(11) unsigned NOT NULL DEFAULT 0 COMMENT '用户数',
    pv int(11) unsigned NOT NULL DEFAULT 0 COMMENT '次数',
    vv int(11) unsigned NOT NULL DEFAULT 0 COMMENT '会话数',
    iv int(11) unsigned NOT NULL DEFAULT 0 COMMENT '独立IP数',
    event_length bigint(20) NOT NULL DEFAULT 0 COMMENT '总事件时长(秒)',
    PRIMARY KEY stat_key(stat_date, app_id, channel, tag, event_id)
) ENGINE=InnoDB, CHARSET=utf8mb4, COMMENT='事件统计结果表';

CREATE TABLE IF NOT EXISTS `tb_page`(
    stat_date date NOT NULL DEFAULT '2018-03-05' COMMENT '统计目标日期',
    app_id int(11) unsigned NOT NULL DEFAULT 0 COMMENT '应用ID',
    channel varchar(32) NOT NULL DEFAULT '' COMMENT '来源渠道',
    tag varchar(32) NOT NULL DEFAULT '' COMMENT '标签',
    page varchar(128) NOT NULL DEFAULT '' COMMENT '标签',
    uv int(11) unsigned NOT NULL DEFAULT 0 COMMENT '用户数',
    pv int(11) unsigned NOT NULL DEFAULT 0 COMMENT '次数',
    vv int(11) unsigned NOT NULL DEFAULT 0 COMMENT '会话数',
    iv int(11) unsigned NOT NULL DEFAULT 0 COMMENT '独立IP数',
    online_time bigint(20) NOT NULL DEFAULT 0 COMMENT '总页面时长(秒)',
    exit_vv int(11) unsigned NOT NULL DEFAULT 0 COMMENT '退出数',
    bounce_vv int(11) unsigned NOT NULL DEFAULT 0 COMMENT '跳出数',
    PRIMARY KEY stat_key(stat_date, app_id, channel, tag, page)
) ENGINE=InnoDB, CHARSET=utf8mb4, COMMENT='页面统计结果表';



