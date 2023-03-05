## Computer Vision Assignment1

### 题目要求

利用空间滤波方法对图像进行处理。

具体要求包括:

1. 输入一张校园图像。
2. 分别添加高斯噪声和椒盐噪声。
3. 分别选择高斯滤波或中值滤波对图像进行处理。

提交内容:

+ a. 源码。

  + 完成(2)(40分)
  + 使用OpenCV函数完成(3)(20分)
  + 自行写(3)中的两种(30分，其中一种给15分)，使用c++，提交cpp文件。
+ b. 报告。
  包含实验结果分析及运行结果截图。原图+(2）中任意一种效果图+(3）效果图Pdf文件，10分

### 完成情况

1. 输入校园图像并显示
2. 分别添加了高斯噪声和椒盐噪声
3. 分别使用了高斯滤波和中值滤波对两种噪声进行处理
   + 分别使用OpenCV内置函数和自行编写函数实现两种滤波算法
   + 对高斯噪声进行高斯滤波，对椒盐噪声进行中值滤波

### Usage

使用`CMake`完成编译，推荐在Linux系统下运行，在Windows下运行需要提前配置好相应环境变量，如CMake、Make、编译好的OpenCV、OpenCV_DIR等。

本项目使用的OpenCV版本为 OpenCV 4.5.3

+ 编译

    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

    如果是在Windows系统下，则需使用如下指令：
    ```bash
    mkdir build
    cd build
    cmake .. -G "Unix Makefiles"
    make
    ```

+ 展示添加椒盐噪声结果：

```bash
./main.exe do_salt
```

+ 使用OpenCV内置函数对椒盐噪声进行中值滤波：

```bash
./main.exe do_salt opencv
```

+ 使用自己编写的函数对椒盐噪声进行中值滤波：

```
./main.exe do_salt our
```

+ 同时运行OpenCV内置函数和自行编写的中值滤波函数：

```
./main.exe do_salt both
```

+ 展示添加高斯噪声结果：

```bash
./main.exe do_gaussian
```

+ 使用OpenCV内置函数对高斯噪声进行高斯滤波：

```bash
./main.exe do_gaussian opencv
```

+ 使用自己编写的函数对高斯噪声进行高斯滤波：

```bash
./main.exe do_gaussian our
```

+ 同时运行OpenCV内置函数和自行编写的高斯滤波函数：

```
./main.exe do_gaussian both
```

### 效果展示

#### 原图

+ <img src="\assets\original.png" alt="image-20230305201053547" style="zoom:40%;" />

#### 椒盐噪声及其中值滤波结果

+ 添加椒盐噪声结果：

​				<img src="\assets\salt_pepper_noise.png" alt="image-20230305201053547" style="zoom:40%;" />

+ OpenCV内置函数进行中值滤波：

  ​		<img src="\assets\salt_opencv_median_result.png" alt="image-20230305201053547" style="zoom:40%;" />

+ 自行编写的滤波函数进行中值滤波：

​				<img src="\assets\salt_our_median_result.png" alt="image-20230305201053547" style="zoom:40%;" />

#### 高斯噪声及其高斯滤波结果

+ 添加高斯噪声结果：
​				<img src="\assets\gaussian_noise.png" alt="image-20230305201053547" style="zoom:40%;" />
+ OpenCV内置函数进行高斯滤波：
​				<img src="\assets\gaussian_opencv_result.png" alt="image-20230305201053547" style="zoom:40%;" />
+ 自行编写的滤波函数进行高斯滤波：
​				<img src="\assets\gaussian_our_result.png" alt="image-20230305201053547" style="zoom:40%;" />

仔细看还是可以看出高斯噪声滤波结果的。