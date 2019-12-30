#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200

#include <CustomLibrary/Error.h>
#include <CL/cl.hpp>
#include <stdio.h>

using namespace ctl;

void check_err(cl_int err, const char* name)
{
	if (err != CL_SUCCESS)
		throw err::Log(name);
}

int main(int argc, char** argv)
{
	//constexpr std::string_view hw = "Hello World\n";

	try
	{
		//std::vector<cl::Platform> platforms;
		//cl::Platform::get(&platforms);
		//if (platforms.size() == 0)
		//	throw err::Log("No platform devices.");
		//err::g_log.write("Platforms: " + std::to_string(platforms.size()) + '\n');

		//std::string platform_vendor;
		//platforms.front().getInfo(CL_PLATFORM_VENDOR, &platform_vendor);
		//err::g_log.write("Platform is by: " + platform_vendor + '\n');
		//
		//cl_int err;
		//cl_context_properties prop[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms.front())(), 0 };
		//cl::Context context(CL_DEVICE_TYPE_CPU, prop, nullptr, nullptr, &err);

		//char* out_h = new char[hw.size() + 1];
		//cl::Buffer out_cl(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, hw.size() + 1, out_h, &err);
		//if (err == 0)
		//	throw err::Log("Buffer initialization failed.");

		//std::vector<cl::Device> devices;
		//devices = context.getInfo<CL_CONTEXT_DEVICES>(&err);
		//std::cerr << err << '\n';
		////if (devices.size() == 0)
		////	throw err::Log("No devices found.");

		//std::ifstream file_in("lesson1_kernels.cl");
		//if (!file_in.is_open())
		//	throw err::Log("kernel file not found.");

		//std::string prog(std::istreambuf_iterator<char>(file_in), (std::istreambuf_iterator<char>()));

		//cl::Program::Sources source(1, std::pair(prog.c_str(), prog.size() + 1));
		//cl::Program program(context, source);
		//err = program.build(devices, "");
		//if (!file_in.is_open())
		//	throw err::Log("File isn't open.");

		//cl::Kernel kernel(program, "hello", &err);
		////if (err)
		////	throw err::Log("Kernel returned a error.");

		//if (kernel.setArg(0, out_cl));
		//	//throw err::Log("Argument setting error.");

		//cl::CommandQueue queue(context, devices.front(), 0, &err);
		////if (err)
		////	throw err::Log("Commandqueue returned an error.");

		//cl::Event event;
		//if (queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(hw.size() + 1), cl::NDRange(1, 1), nullptr, &event));
		//	//throw err::Log("Command queue error.");

		//event.wait();
		//if (queue.enqueueReadBuffer(out_cl, CL_TRUE, 0, hw.size() + 1, out_h));
		//	//throw err::Log("Command queue second error.");

		//std::cout << out_h;
		//delete[] out_h;








		//std::vector<cl::Platform> platforms;
		//cl::Platform::get(&platforms);
		//auto& platform = platforms.front();
		//std::vector<cl::Device> devices;
		//platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
		//auto& device = devices.front();

		//cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform(), 0 };
		//cl::Context myContext(device, properties);

		//std::ifstream helloWorldFile("hello_world.cl");
		//std::string src(std::istreambuf_iterator<char>(helloWorldFile), (std::istreambuf_iterator<char>()));

		//cl::Program program(myContext, src);
		//program.build(devices, "");

		//char buf[16];
		//cl::Buffer outputBuffer(myContext, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(buf));

		//cl::Kernel kernel(program, "HelloWorld");
		//kernel.setArg(0, outputBuffer);

		//cl::CommandQueue commandQueue(myContext, device);
		//commandQueue.enqueueNDRangeKernel(kernel, 0, 1, 1);
		//commandQueue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, sizeof(buf), buf);

		//std::cout << buf;

		//return EXIT_SUCCESS;



















			//get all platforms (drivers)
		std::vector<cl::Platform> all_platforms;
		cl::Platform::get(&all_platforms);
		if (all_platforms.size() == 0) {
			std::cout << " No platforms found. Check OpenCL installation!\n";
			exit(1);
		}
		cl::Platform default_platform = all_platforms[0];
		std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

		//get default device of the default platform
		std::vector<cl::Device> all_devices;
		default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
		if (all_devices.size() == 0) {
			std::cout << " No devices found. Check OpenCL installation!\n";
			exit(1);
		}
		cl::Device default_device = all_devices[0];
		std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";


		cl::Context context({ default_device });

		cl::Program::Sources sources;

		// kernel calculates for each element C=A+B
		std::string kernel_code =
		"   void kernel simple_add(global const int* A, global const int* B, global int* C) {       "
		"       C[get_global_id(0)] = A[get_global_id(0)] + B[get_global_id(0)];                 "
		"   }                                                                               ";
		sources.push_back({ kernel_code.c_str(),kernel_code.length() });

		cl::Program program(context, sources);
		if (program.build({ default_device }) != CL_SUCCESS) {
			std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
			exit(1);
		}


		// create buffers on the device
		cl::Buffer buffer_A(context, CL_MEM_READ_WRITE, sizeof(int) * 10);
		cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, sizeof(int) * 10);
		cl::Buffer buffer_C(context, CL_MEM_READ_WRITE, sizeof(int) * 10);

		int A[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		int B[] = { 0, 1, 2, 0, 1, 2, 0, 1, 2, 0 };

		//create queue to which we will push commands for the device.
		cl::CommandQueue queue(context, default_device);

		//write arrays A and B to the device
		queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(int) * 10, A);
		queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, sizeof(int) * 10, B);


		//alternative way to run the kernel
		cl::Kernel kernel_add=cl::Kernel(program,"simple_add");
		kernel_add.setArg(0,buffer_A);
		kernel_add.setArg(1,buffer_B);
		kernel_add.setArg(2,buffer_C);
		queue.enqueueNDRangeKernel(kernel_add,cl::NullRange,cl::NDRange(10),cl::NullRange);
		queue.finish();

		int C[10];
		//read result C from the device to array C
		queue.enqueueReadBuffer(buffer_C, CL_TRUE, 0, sizeof(int) * 10, C);

		std::cout << " result: \n";
		for (int i = 0; i < 10; i++) {
			std::cout << C[i] << " ";
		}

		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}

	return 0;
}