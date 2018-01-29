### EVALUATING THE ONLINE CAPABILITIES OF ONSET DETECTION METHODS 论文实现
####Paper description
1. 主要实现音频的onset检测
2. 该篇论文总结了传统onset检测的多种方法，比如检测频谱的幅度值(SF)变化,相位变化（PD）等，实现方法是论文中提出的log SF filter方法
3. log sf filter是通过fft计算频谱,然后对频谱进行log操作，之后进行滤波filter

### project description
1. src 代码源文件
2. include 代码头文件
3. config 本工程的一些配置文件,比如降采样所需要的配置文件
4. pythontool:读取原始音频数据的python实现，主要目的是分帧
5. 本工程需要python环境支持


### project Compile
	mkdir build
	cd build
	cmake .. -G "Visual Studio 12 2013 Win64"

