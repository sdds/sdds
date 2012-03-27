extern "C"
{

#include <stdbool.h>
#include "linux_sdds_impl.h"
#include "ipc-ds.h"

}

#include <iostream>
#include <thread>
#include <mutex>

using std::cerr;
using std::cout;
using std::thread;
using std::mutex;
using std::unique_lock;

void do_receive()
{
	ipc_data_t data_used;
	ipc_data_t *data_used_ptr = &data_used;

	for (;;)
	{
		dds_return_t ret = ipc_data_reader_take_next_sample(g_ipc_reader, &data_used_ptr, NULL);

		if (ret == DDS_RETCODE_NO_DATA)
		{
			sleep(1);
		}
		else
		{
			unique_lock<mutex> lock;
			cout << "received: " << data_used.value << '\n';
		}
	}
}

int main()
{
	init_sdds(3);

	ipc_data_t ipc_data_used = { 0 };

	thread t(do_receive);

	for (;;)
	{
		sleep(2);

		dds_return_t ret = ipc_data_writer_write(g_ipc_writer, &ipc_data_used, NULL);

		if (ret != DDS_RETCODE_OK)
		{
			cerr << "sending ipc command failed\n";
		}
		else
		{
			cout << "sent: " << ipc_data_used.value << '\n';
		}

		ipc_data_used.value++;
	}

	t.join();
}
