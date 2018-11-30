#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import socket

sk = socket.socket()
sk.bind(("127.0.0.1", 80))
sk.listen()


def func(url):
	s = "这是 {} 页面".format(url)
	return bytes(s, encoding="gb2312")

def index(url):
	s = "这是 {} 页面 XX!".format(url)
	return bytes(s, encoding="gb2312")

def content(url):
	with open("a.html", "r", encoding="utf-8") as f:
		s = f.read()
	return bytes(s, encoding="utf-8")


def home(url):
	s = "这是 {} 页 OO!".format(url)
	return bytes(s, encoding="gb2312")


def time(url):
	import time
	with open("time.html", "r", encoding="utf-8") as f:
		s = f.read()
		s = s.replace("@@time@@", time.strftime("%y-%m-%d %H:%H:%M:%S"))
	return bytes(s, encoding="utf-8")

list1 = [
	("/index", index),
	("/home", home),
	("/content", content),
	("/time", time),
]

while True:
	conn, addr = sk.accept()
	data = conn.recv(8096)
	data = str(data, encoding="utf8")  # 接收到的字节类型转换为字符串
	data1 = data.split("\r\n")[0]      # 按 "/r/n" 分割
	url = data1.split()[1]				# 从路径中分离出访问路径

	conn.send(b"HTTP/1.1 200 OK \r\n\r\n")

	# 按路径匹配
	# if url == "/index":
	# 	response = func(url)
	# elif url == "/home":
	# 	response = func(url)
	# else:
	# 	response = b"404 not found"

	func = None
	for item in list1:
		if item[0] == url:
			func = item[1]
			break

	if func:
		response = func(url)
	else:
		response = b"404 Not Found!!"

	conn.send(response)
	conn.close()

