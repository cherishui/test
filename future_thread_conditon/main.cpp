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
		bool ready; // �Ƿ�׼����
		int32_t value;
	};

	struct _data data = { false, 0 };
	std::mutex data_mutex;
	std::condition_variable data_con;

	// �������̣߳����ڲ�������
	std::thread prepare_data_pthread([&data_mutex, &data_con, &data]() {
		std::this_thread::sleep_for(std::chrono::seconds(2));

		std::unique_lock<std::mutex> ulock(data_mutex);
		data.ready = true;
		data.value = 1;

		// ����������

		data_con.notify_one();
	});

	// �������̣߳�������������
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

	std::promise<_data> data_promise;	// ��ŵ�ظ�����
	std::future<_data> data_future = data_promise.get_future(); // �ӳ�ŵ�еõ�����

	// Ҫʹ�� future �� promise ��ͬ���̣߳���Ҫ�� promise �� future  �Ժ�����εķ�ʽ��
	// ����lambda��
	std::thread prepare_data_thread([](std::promise<_data>& data_promise) {
		std::cout << "enter prepare thread" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(5));
		std::cout << "exit prepare thread" << std::endl;
		data_promise.set_value({1});	//ͨ��set_value()�������
	}, std::ref(data_promise));

	std::thread process_data_thread([](std::future<_data>& data_future) {
		std::cout << "enter process thread"<< std::endl;
		std::cout << data_future.get().value << std::endl;	// �ڻ�ȡʱ�������� get() ����
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

	// ʹ�� packaged_task �ڸ��߲���Ϸ�װ ����, ��Ҫʹ�� β׺��ʾ������ʾ��������
	// �ں������ڣ�ʹ��return�����ض�Ӧ������
	std::packaged_task<_data()> prepare_data_task([]() -> _data {
			std::cout << "enter prepare_data_task" << std::endl << std::flush;
			std::this_thread::sleep_for(std::chrono::seconds(3));
			std::cout << "exit prepare_data_task" << std::endl;
			return{ 1 };
		}
	);

	// ʹ���ڲ��� get_future ��������Ľ��
	auto data_future = prepare_data_task.get_future();

	std::thread do_task_thread([](std::packaged_task<_data()>& task) {
			std::cout << "enter do_task_thread" << std::endl;
			task();
			std::cout << "exit do_task_thread" << std::endl;
		}
	, ref(prepare_data_task));

	std::thread process_data_thread([](std::future<_data>& data_future) {
		std::cout << "enter process_data_thread" << std::endl;
		std::cout << data_future.get().value << std::endl;	// ֻ�ܱ�����һ�Σ���ε��ûᴥ���쳣
		//std::cout << data_future.get().value << std::endl;	// ֻ�ܱ�����һ�Σ���ε��ûᴥ���쳣
		std::cout << "exit process_data_thread" << std::endl;
	}, std::ref(data_future));

	// ʹ�� packagetask��ʱ��Ҫ�ȵ��������ߣ��ٵ���������
	process_data_thread.join();
	do_task_thread.join();
}

// ���첽���һ�������������߳����������߷�������ø� �첽�����Ľ��
// ͨ�������� �������߳�����첽���������¹������ݣ����߳���ɺ�ͨ����Ϣ֪ͨ���߳�
// ���߳� ʹ�� ������������ ����ø� ��������
// c++11 �ṩ async ����װ��������
void test_async()
{
	struct _data {
		int32_t value;
	}data = {0};

	auto start = std::chrono::steady_clock::now();

	// async ���� һ�� �� lambda����ֵ������ͬ�� future������ͨ�� ������첽���
	std::future<_data> data_future = std::async(std::launch::async,	// ��������Ϊͬʱ������async  �ӳ��������ԣ�deferred��ֱ������getʱ�����̲߳Żᱻ����ִ�У�
											   []() -> _data {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			return { 2 };
		}
	);

	std::this_thread::sleep_for(std::chrono::seconds(2));
	std::cout << data_future.get().value << std::endl; // ʹ�� ����ֵ
	
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
