#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <memory>
#include <chrono>

#include <future>  // std::future and std::promise


void test_mutex_cv()
{
	struct _data {
		bool ready; // 是否准备好
		int32_t value;
	};

	struct _data data = { false, 0 };
	std::mutex data_mutex;
	std::condition_variable data_con;

	// 生产者线程，用于产生数据
	std::thread prepare_data_pthread([&data_mutex, &data_con, &data]() {
		std::this_thread::sleep_for(std::chrono::seconds(2));

		std::unique_lock<std::mutex> ulock(data_mutex);
		data.ready = true;
		data.value = 1;

		// 唤醒消费者

		data_con.notify_one();
	});

	// 消费者线程，用于消费数据
	std::thread process_data_thread([&data_mutex, &data_con, &data]() {
		std::unique_lock<std::mutex> ulock(data_mutex);
		data_con.wait(ulock, [&data]() {
			return data.ready;
		});

		std::cout << data.value << std::endl;
	});

	process_data_thread.join();
	prepare_data_pthread.join();
}

void use_future_promise()
{
	struct _data {
		int32_t value;
	}data = {0};

	std::promise<_data> data_promise;	// 承诺回复数据
	std::future<_data> data_future = data_promise.get_future(); // 从承诺中得到数据

	// 要使用 future 和 promise 来同步线程，需要将 promise 和 future  以函数入参的方式，
	// 传入lambda中
	std::thread prepare_data_thread([](std::promise<_data>& data_promise) {
		std::cout << "enter prepare thread" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(5));
		std::cout << "exit prepare thread" << std::endl;
		data_promise.set_value({1});	//通过set_value()反馈结果
	}, std::ref(data_promise));

	std::thread process_data_thread([](std::future<_data>& data_future) {
		std::cout << "enter process thread"<< std::endl;
		std::cout << data_future.get().value << std::endl;	// 在获取时，阻塞在 get() 这里
		std::cout << "exit process thread" << std::endl;
	}, std::ref(data_future));

	prepare_data_thread.join();
	process_data_thread.join();
}

void packaged_task()
{
	struct _data {
		int32_t value;
	}data = {0};

	// 使用 packaged_task 在更高层次上封装 任务, 需要使用 尾缀标示符来表示返回类型
	// 在函数体内，使用return来返回对应的类型
	std::packaged_task<_data()> prepare_data_task([]() -> _data {
			std::cout << "enter prepare_data_task" << std::endl << std::flush;
			std::this_thread::sleep_for(std::chrono::seconds(3));
			std::cout << "exit prepare_data_task" << std::endl;
			return{ 1 };
		}
	);

	// 使用内部的 get_future 获得期望的结果
	auto data_future = prepare_data_task.get_future();

	std::thread do_task_thread([](std::packaged_task<_data()>& task) {
			std::cout << "enter do_task_thread" << std::endl;
			task();
			std::cout << "exit do_task_thread" << std::endl;
		}
	, ref(prepare_data_task));

	std::thread process_data_thread([](std::future<_data>& data_future) {
		std::cout << "enter process_data_thread" << std::endl;
		std::cout << data_future.get().value << std::endl;	// 只能被调用一次，多次调用会触发异常
		//std::cout << data_future.get().value << std::endl;	// 只能被调用一次，多次调用会触发异常
		std::cout << "exit process_data_thread" << std::endl;
	}, std::ref(data_future));

	// 使用 packagetask的时候，要先调用生产者，再调用消费者
	process_data_thread.join();
	do_task_thread.join();
}

// 想异步完成一个操作，在主线程中阻塞或者非阻塞获得该 异步操作的结果
// 通常的做法 创建子线程完成异步操作，更新共享数据，子线程完成后，通过消息通知主线程
// 主线程 使用 锁和条件变量 来获得该 共享数据
// c++11 提供 async 来封装上述功能
void test_async()
{
	struct _data {
		int32_t value;
	}data = {0};

	auto start = std::chrono::steady_clock::now();

	// async 返回 一个 与 lambda返回值类型相同的 future，后面通过 它获得异步结果
	std::future<_data> data_future = std::async(std::launch::async,	// 启动策略为同时启动：async  延迟启动策略：deferred（直到调用get时，子线程才会被创建执行）
											   []() -> _data {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			return { 2 };
		}
	);

	std::this_thread::sleep_for(std::chrono::seconds(2));
	std::cout << data_future.get().value << std::endl; // 使用 产出值
	
	auto end = std::chrono::steady_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

}

int main()
{
	//test_mutex_cv();
	//use_future_promise();
	//packaged_task();
	test_async();
	system("pause");
	return 0;
}
