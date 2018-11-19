# NumPy学习

Numpy是Python的科学计算基础库，它提供多维数组对象，矩阵对象以及一系列快速操作函数，包括数学类，逻辑类，形状操控类，排序，离散傅里叶变换，基本线性代数，统计操作，随机模拟等许多功能。

Numpy包的合行对象是`ndarray`对象，它和`Python list`对象有以下区别：

* `NumPy arrays`在创建时固定大小，而`Python lists`可以动态扩展。改变`ndarray`的大小会创建一个新的`ndarray`，删除旧的`ndarray`

* 在`Numpy arrays`中的元素类型要求是同一数据类型

* 相对比`Python lists`，`Numpy arrays`在大规模数据的数学计算上能够花费更少的代码，同时运行的更有效率。

* 许多科学计算包底层都是基于`NumPy`，这些包对外支持`Python lists`输入，内部优先将其转换为`NumPy arrays`，然后再进行后续处理，他们的输出经常是`NumPy arrays`格式。


## PyCharm项目设置 ##

Python项目设置的最佳实践是为每个项目创建`virtualenv`,
