#!/usr/bin/env python
# -*- coding: utf-8 -*-

# @Create on  2018-11-30 13:59
# @Author: huhao
# @File: snowflake
# @Description: python版本的 snowflake 算法实现，用于 分布式环境下的全局唯一ID生产



import sys
import random
import threading
import time

from concurrent import futures



class Snowflake(object):

    region_id_bits = 2
