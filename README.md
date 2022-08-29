# 二叉树数据结构可视化(Visualized-two-three-tree)
数据结构课程设计，实现能够支持动态插入、查找、删除节点操作的可视化2-3树(Implement a visualized 2-3 tree which supports node insertion, search and deletion dynamically.)
## 题目要求（Problem description）
**试从空树出发构造一棵深度至少为 3（不包括失误结点）的 3 阶 B-树（又称 2-3 树），
并可以随时进行查找、插入、删除等操作。
要求：能够把构造和删除过程中的 B-树随时显示输出来，能给出查找是否成功的有关信
息。**
## 数据结构与算法（DS & Algorithms）
**实验中实现一棵2-3树的基本数据结构，和动态插入、删除与查找节点算法（2-3树满足二叉排序树的特性，并且对于为2节点的根节点，左子树的所有节点中的元素小于根节点元素，右子树的所有节点中的元素大于根节点元素，对于为3节点的根节点，根节点的左边元素小于根节点右边元素，左子树的所有节点中的元素小于根节点的左边元素和右边元素，中间子树所有节点中的元素大于根节点左边元素但是小于根节点右边元素，右子树所有节点中的元素大于根节点的右边元素）  
(This project implements a basic 2-3 tree structure and dynamic insertion, deletion and search algorithm for the tree. Pls search "What is a 2-3 tree?" with any search engine if u dont know what a 2-3 tree is.)**
## 操作说明（Manual）
**通过左上方控件添加、删除、查找节点，在右侧画布窗口可视化显示树。使用左键按住节点可以拖动节点，右键按住画布可以拖动画布，右键单击节点可以标记/取消标记节点，使用样式设置栏可以设置树绘制的样式。  
（Insert/Delete/Search for Node with respectives controls. Tree will be drawn in the canvas window. Hold mouse left key to drag a node, hold mouse right key to drag the canvas, press mouse right key on any node to mark/unmark it, use controls under Misc tab to set styles for visualization.）**
## 运行平台（Platform）
（1）操作系统（OS）：Windows11 Pro build 22000.795  
（2）开发语言（Language）：C++(ISO C++17 Standard)  
（3）集成开发环境（IDE）：Visual Studio 2022  
（4）Windows 软件开发包版本（Windows SDK Version）：10.0 (latest installed version)  
（6）图形框架（GUI）：Dear ImGui  
（5）渲染器（Renderer）：OpenGL2 + GLFW 平台  
（6）编译器（Compiler）：MSVC v143 32bit  
## 功能（Features）
+ 添加节点（Add Node）
+ 查找节点（Search For Node）
+ 删除节点（Modify Node）
+ 样式设置（Set Styles）
+ 其它功能（Miscellaneous features, find out urself）
## 截图（Screenshot）
![screenshot](https://raw.githubusercontent.com/leo4048111/Visualized-Two-Three-Tree/main/screenshot/screenshot.png?token=GHSAT0AAAAAABRWPC25ZJCEW4ISJCUYXDACYYMCEMA)
## License
All 3rd-party assets and libraries used in this project retain all rights under their respective licenses.
