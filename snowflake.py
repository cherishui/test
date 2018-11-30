#!/usr/bin/env python
# -*- coding: utf-8 -*-

# @Create on
# @Author:
# @File: snowflake
# @Description: python版本的 snowflake 算法实现，用于 分布式环境下的全局唯一ID生产

import sys
import random
import threading
import time

# 提供 ThreadPoolExecutor 和 ProcessPollExecutor 两个类，是对 线程和进程的进一步封装
# 为编写线程池和进程池提供直接支持
from concurrent import futures

"""
// futures 模块使用
https://www.cnblogs.com/dylan-wu/p/7163823.html

// snow flake 算法
https://www.jianshu.com/p/61817cf48cc3

Snow flake 算法
41 bits timestamp（毫秒） | 3 bits 区域(机房) | 10 bits 机器编号 | 10 bits 序列号
"""

class Snowflake(object):
    region_id_bits = 2 # 区域标志位数
    worker_id_bits = 10 # 机器编号
    sequence_bits = 11
    MAX_REGION_ID = -1 ^ (-1 << region_id_bits) # 区域标志ID最大值
    MAX_WORKER_ID = -1 ^ (-1 << worker_id_bits) # 机器ID最大值
    SEQUENCE_MASK = -1 ^ (-1 << sequence_bits) # 序列号最大值

    WOEKER_ID_SHIFT = sequence_bits
    REGION_ID_SHIFT = sequence_bits + worker_id_bits
    TIMESTAMP_LEFT_SHIFT = (sequence_bits + worker_id_bits + region_id_bits)

    def __init__(self, worker_id, region_id=0):
        self.twepoch = 1288834974657    # 基准时间
        self.last_timestamp = -1
        self.sequence = 0

        # 超出范围，触发断言
        assert 0 <= worker_id <= Snowflake.MAX_WORKER_ID
        assert 0 <= region_id <= Snowflake.MAX_REGION_ID

        self.worker_id = worker_id
        self.region_id = region_id

        self.lock = threading.Lock()

    def generate(self, bus_id=None):
        return self.next_id(
            True if bus_id is not None else False,
            bus_id if bus_id is not None else 0
        )

    def next_id(self, is_padding, bus_id):
        with self.lock:
            timestamp = self.get_time()
            padding_num = self.region_id

            if is_padding:
                padding_num = bus_id

            if timestamp < self.last_timestamp:
                try:
                    raise ValueError(
                        'Clock moved backwards. Refusing to'
                        'generate id for {0} milliseconds'.format(
                            self.last_timestamp - timestamp
                        )
                    )
                except ValueError:
                    print(sys.exc_info()[2])

            if timestamp == self.last_timestamp:
                # 如果上次生成时间和当前时间相同，表示在 同一毫秒内，序号自增。因为序号只有10bits，会回转
                self.sequence = (self.sequence + 1) & Snowflake.SEQUENCE_MASK
                if self.sequence == 0:
                    # 自旋等待到下一毫秒
                    timestamp = self.tail_next_millis(self.last_timestamp)
            else:
                # 如果和上次生成的时间不同，重置 sequence 到[0,9] 之间的随机数
                self.sequence = random.randint(0, 9)

            self.last_timestamp = timestamp

            return (
                (timestamp - self.twepoch) << Snowflake.TIMESTAMP_LEFT_SHIFT |
                (padding_num << Snowflake.REGION_ID_SHIFT) |
                (self.worker_id << Snowflake.WOEKER_ID_SHIFT) |
                self.sequence
            )

    def tail_next_millis(self, last_timestamp):
        timestamp = self.get_time()
        while timestamp <= last_timestamp:
            timestamp = self.get_time()
        return timestamp

    def get_time(self):
        return int(time.time() * 1000)


def main():
    id_set = set()
    snowflake = Snowflake(1)

    def gen_id():
        try:
            _id = snowflake.generate()
        except Exception as e:
            print(e)
        else:
            assert _id not in id_set
            id_set.add(_id)

    f_list = []
    with futures.ThreadPoolExecutor(max_workers=16) as executor:
        f_list.append([executor.submit(gen_id) for _ in range(1000)])

    print(futures.wait(f_list))
    print('%d IDs in the set' % (len(id_set)))


if __name__ == '__main__':
    main()
