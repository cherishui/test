#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from wsgiref.simple_server import make_server


# 将返回不同的内容部分封装成函数
def index(url):
	# 读取index.html页面的内容
	with open("index.html", "r", encoding="utf8") as f:
		s = f.read()
	# 返回字节数据
	return bytes(s, encoding="utf8")


def home(url):
	with open("home.html", "r", encoding="utf8") as f:
		s = f.read()
	return bytes(s, encoding="utf8")


# 定义一个url和实际要执行的函数的对应关系
list1 = [
	("/index", index),
	("/home", home)
]


def run_server(environ, start_response):
	start_response("200 OK", [("Content-Type", "text/html;charset=utf8"),])  # 设置HTTP响应的状态码和头部信息
	url = environ["PATH_INFO"]
	func = None
	for i in list1:
		if i[0] == url:
			func = i[1]
			break

	if func:
		response = func(url)
	else:
		response = b"404 Not Found"

	return [response, ]


if __name__ == "__main__":
	httpd = make_server("127.0.0.1", 8090, run_server)
	print("我在8090等你")
	httpd.serve_forever()
