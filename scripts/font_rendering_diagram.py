#!/usr/bin/env python3
"""
OpenGL 文本渲染示意图生成器
此脚本生成一个图形，展示 OpenGL 如何使用三角形和纹理渲染文本
"""

import matplotlib.pyplot as plt
import matplotlib.patches as patches
import numpy as np
from matplotlib.path import Path
import os
import matplotlib as mpl

# 设置 matplotlib 支持中文显示
# 使用系统中已安装的 Noto Sans CJK SC 字体
plt.rcParams['font.sans-serif'] = ['Noto Sans CJK SC', 'Noto Sans CJK TC', 'Noto Sans CJK JP', 'Noto Sans CJK KR', 'AR PL UKai CN', 'AR PL UMing CN']
plt.rcParams['axes.unicode_minus'] = False  # 解决负号显示问题
mpl.rcParams['font.family'] = 'sans-serif'

# 确保输出目录存在
os.makedirs("imgs", exist_ok=True)

# 创建图形和子图
fig, ax = plt.subplots(figsize=(8, 6))
fig.suptitle('OpenGL 文本渲染示意图', fontsize=16)

# 绘制字符纹理和三角形 (使用字母 "A")
ax.set_title('字符纹理和三角形 - 字母"A"')
ax.set_xlim(-0.1, 1.1)
ax.set_ylim(-0.1, 1.1)
ax.set_aspect('equal')
ax.grid(True, linestyle='--', alpha=0.7)

# 定义四边形的顶点（两个三角形）
quad_vertices = np.array([
    [0.0, 1.0],  # 左上 (v0)
    [0.0, 0.0],  # 左下 (v1)
    [1.0, 0.0],  # 右下 (v2)
    [1.0, 1.0]   # 右上 (v3)
])

# 定义三角形 - 修正红色三角形的顶点顺序
triangle1 = np.array([quad_vertices[0], quad_vertices[1], quad_vertices[2]])
triangle2 = np.array([quad_vertices[0], quad_vertices[2], quad_vertices[3]])  # 正确的顺序

# 绘制三角形
t1 = patches.Polygon(triangle1, alpha=0.3, color='blue', label='三角形 1')
t2 = patches.Polygon(triangle2, alpha=0.3, color='red', label='三角形 2')
ax.add_patch(t1)
ax.add_patch(t2)

# # 绘制顶点并添加标签
# for i, (x, y) in enumerate(quad_vertices):
#     ax.plot(x, y, 'ko', markersize=8)
#     ax.text(x, y, f'v{i} ({x},{y})', fontsize=10, 
#              ha='right' if x < 0.5 else 'left', 
#              va='bottom' if y < 0.5 else 'top')

# 添加顶点顺序箭头 - 修正红色三角形的箭头顺序
ax.annotate('', xy=(0.0, 0.0), xytext=(0.0, 1.0), 
             arrowprops=dict(arrowstyle='->', color='blue', lw=1.5))
ax.annotate('', xy=(1.0, 0.0), xytext=(0.0, 0.0), 
             arrowprops=dict(arrowstyle='->', color='blue', lw=1.5))
ax.annotate('', xy=(0.0, 1.0), xytext=(1.0, 0.0), 
             arrowprops=dict(arrowstyle='->', color='blue', lw=1.5))

# 修正红色三角形的箭头顺序
ax.annotate('', xy=(1.0, 0.0), xytext=(0.0, 1.0), 
             arrowprops=dict(arrowstyle='->', color='red', lw=1.5))
ax.annotate('', xy=(1.0, 1.0), xytext=(1.0, 0.0), 
             arrowprops=dict(arrowstyle='->', color='red', lw=1.5))
ax.annotate('', xy=(0.0, 1.0), xytext=(1.0, 1.0), 
             arrowprops=dict(arrowstyle='->', color='red', lw=1.5))

# 添加纹理坐标标签 - 更新为正确的纹理坐标
ax.text(0, 1, '(0,1)', fontsize=10, ha='right', va='top', color='blue')
ax.text(0, 0, '(0,0)', fontsize=10, ha='right', va='top', color='blue')
ax.text(1, 0, '(1,0)', fontsize=10, ha='right', va='top', color='blue')

# 红色三角形
ax.text(0, 1, '(0,1)', fontsize=10, ha='left', va='bottom', color='red')
ax.text(1, 0, '(1,0)', fontsize=10, ha='left', va='bottom', color='red')
ax.text(1, 1, '(1,1)', fontsize=10, ha='left', va='bottom', color='red')

# 添加基线标记
ax.axhline(y=0.2, color='green', linestyle='--', alpha=0.7)
ax.text(1.05, 0.2, '基线', fontsize=10, color='green', va='center')

# 在三角形中间绘制字母"A"
# 左斜线
ax.plot([0.2, 0.5], [0.2, 0.8], 'k-', linewidth=3)
# 右斜线
ax.plot([0.5, 0.8], [0.8, 0.2], 'k-', linewidth=3)
# 横线
ax.plot([0.3, 0.7], [0.5, 0.5], 'k-', linewidth=3)

ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.05))

# 调整布局并保存
plt.tight_layout(rect=[0, 0, 1, 0.95])
plt.savefig('imgs/font_rendering_diagram.png', dpi=300, bbox_inches='tight')
plt.savefig('imgs/font_rendering_diagram.svg', format='svg', bbox_inches='tight')

print("渲染示意图已保存到 imgs/font_rendering_diagram.png 和 .svg") 