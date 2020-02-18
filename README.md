# TinyRasterizer

迷你3D光栅软渲染器，遵照OpenGL的一些惯例设计。

### 使用的第三方库:
- glm(数学库，没必要重复造轮子)
- assimp(导入obj需要，没必要手写parser)

### 目标：
- [x] 三角形光栅化
- [x] 变换矩阵(model,view,projection) 
- [x] 深度测试
- [x] 摄像机
- [x] 模型导入
- [x] 纹理插值
- [ ] shader
- [ ] Phong/ Blinn Phong光照
- [ ] normal mapping
- [ ] 背面剔除
- [ ] 窗口裁剪
- [ ] shadow mapping

### Preview
**颜色插值**

![颜色插值](images/image1.jpg)

**深度测试**

![深度测试](images/image3.jpg)

**模型加载**

![模型加载](images/image2.jpg)

### reference
[ssloy/tinyrenderer](https://github.com/ssloy/tinyrenderer/wiki)
[learnOpenGL](https://learnopengl.com/)