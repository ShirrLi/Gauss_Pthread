# Gauss_Pthread
高斯消去Pthread实验
**x86文件夹**：
- threads文件夹存放了高斯消去串行算法与不同线程数下并行优化算法的源码。
- active_statistic文件夹存放了高斯消去动态线程算法与静态线程算法的源码。
- division文件夹存放有高斯消去块划分算法与循环划分算法的源码。
- Synchronous文件夹存放有高斯消去信号量同步算法与barrier同步算法的源码。
- Pthread_SIMD文件夹存放有Pthread与SSE与AVX结合的两种算法的源码。

**arm文件夹**：
- threads文件夹存放了高斯消去串行算法与不同线程数下并行优化算法的源码ns.cpp。
- active_statistic文件夹存放了高斯消去动态线程算法与静态线程算法的源码as.cpp。
- division文件夹存放有高斯消去块划分算法与循环划分算法的源码division.cpp。
- Pthread_SIMD文件夹存放有Pthread与Neon结合的算法的源码neon.cpp。
