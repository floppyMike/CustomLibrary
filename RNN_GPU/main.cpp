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








		// Get Platform and Device
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);
		auto platform = platforms.front();
		std::vector<cl::Device> devices;
		platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
		auto device = devices.front();

		//This context doesn't work. Causes CL_INVALID_CONTEXT (-34)
		cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform(), 0 };
		cl::Context myContext(device, properties);

		//If I stick with the default context, things work.
		cl::Context defaultContext = cl::Context::getDefault();

		//The choice of context here determines whether it works or not:
		// myContext -> Fails with CL_INVALID_CONTEXT (-34)
		// defaultContext -> works
		//auto chosenContext = myContext;

		std::ifstream helloWorldFile("hello_world.cl");
		std::string src(std::istreambuf_iterator<char>(helloWorldFile), (std::istreambuf_iterator<char>()));

		cl::Program program(myContext, src);
		program.build(devices, "");

		//Debugging code: Check to make sure that the contexts are similar
		//auto myContextDevices = myContext.getInfo<CL_CONTEXT_DEVICES>();
		//auto defaultContextDevices = defaultContext.getInfo<CL_CONTEXT_DEVICES>();
		//auto devicesMatch = myContextDevices == defaultContextDevices; //true

		//auto myContextProperties = myContext.getInfo<CL_CONTEXT_PROPERTIES>();
		//auto defaultContextProperties = defaultContext.getInfo<CL_CONTEXT_PROPERTIES>();
		//auto propertiesMatch = myContextProperties == defaultContextProperties; //true

		//auto myContextNumDevices = myContext.getInfo<CL_CONTEXT_NUM_DEVICES>();
		//auto defaultContextNumDevices = defaultContext.getInfo<CL_CONTEXT_NUM_DEVICES>();
		//auto numDevicesMatch = myContextNumDevices == defaultContextNumDevices; //true

		//auto myContextRefCount = myContext.getInfo<CL_CONTEXT_REFERENCE_COUNT>();           // 1 if defaultContext, 3 if myContext
		//auto defaultContextRefCount = defaultContext.getInfo<CL_CONTEXT_REFERENCE_COUNT>(); // 4 if defaultContext, 2 if myContext
		//auto refCountsMatch = myContextRefCount == defaultContextRefCount;                  // false

		//auto contextsMatch = myContext == defaultContext; //false
		//End of debugging code

		//Continuing with computation
		char buf[16];
		cl::Buffer outputBuffer = cl::Buffer(myContext, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(buf));

		cl::Kernel kernel(program, "HelloWorld");
		kernel.setArg(0, outputBuffer);

		cl::CommandQueue commandQueue(myContext, device);
		auto result = commandQueue.enqueueNDRangeKernel(kernel, 0, 1, 1);           //CL_SUCCESS
		commandQueue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, sizeof(buf), buf); // Execution fails here, raises cl::Error (-34)

		std::cout << buf;

		return EXIT_SUCCESS;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}

	return 0;
}