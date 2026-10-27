/*
 * 阿里云 IoT 设备凭据模板
 * ------------------------------------------------------------------
 * 用法：
 *   1. 把本文件复制为 Air724_config_local.h（同目录）
 *   2. 填入自己的 ProductKey / DeviceName / DeviceSecret / 时间戳
 *   3. Air724.h 会自动 include Air724_config_local.h，无需改源码
 *
 * Air724_config_local.h 已加入 .gitignore，不会被提交，也不会打进素材包。
 * 未提供该文件时，Air724.h 使用占位值，工程仍可编译，但无法连上阿里云。
 * ------------------------------------------------------------------
 */
#ifndef AIR724_CONFIG_LOCAL_H
#define AIR724_CONFIG_LOCAL_H

/* 阿里云 IoT 三元组 */
#define ALIYUN_PRODUCT_KEY   "YOUR_PRODUCT_KEY"
#define ALIYUN_DEVICE_NAME   "YOUR_DEVICE_NAME"
#define ALIYUN_DEVICE_SECRET "YOUR_DEVICE_SECRET"

/* 客户端 ID 中的 13 位毫秒时间戳，签名时使用 */
#define ALIYUN_TIMESTAMP     "1700000000000"

/* 监护人手机号（用于短信告警与拨号）；留空则不发送 */
#define TELL_PHONE_NUMBER    "13800000000"

#endif /* AIR724_CONFIG_LOCAL_H */