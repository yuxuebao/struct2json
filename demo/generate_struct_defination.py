#!/usr/bin/python3
import os
import re
import sys

root = "./inc"
if not os.path.isdir(root):
    print("错误: 目录 %s 不存在，请先创建并放入头文件" % root)
    sys.exit(1)
# 仅匹配以 .h 结尾的文件并跳过隐藏文件（vim 交换文件 .xxx.h.swp 会被 rfind(".h") 误匹配导致解码崩溃）
# 排序保证多文件时提取顺序稳定，避免因目录顺序不同产生无意义的 diff
h_file_list = sorted(f for f in os.listdir(root)
                     if f.endswith(".h") and not f.startswith("."))
content = ""

def read_header(path):
    """优先按 UTF-8 读取，遇到非 UTF-8 编码（如 GBK 中文注释）回退，避免直接崩溃"""
    try:
        with open(path, 'r', encoding='UTF-8') as f:
            return f.read()
    except UnicodeDecodeError:
        print("警告: %s 非 UTF-8 编码，已按 GBK 回退读取" % path)
        with open(path, 'r', encoding='GBK', errors='replace') as f:
            return f.read()

for h_file in h_file_list:
    content_file = read_header(root + "/" + h_file)
    content_file = re.sub(r'((?<=\n)|^)[\t]*\/\*.*?\*\/\n?|\/\*.*?\*\/|((?<=\n)|^)[\t]*\/\/[^\n]*\n|\/\/[^\n]*','',content_file)
    num_struct = content_file.count('typedef struct')
    keyword_start = 'typedef struct'
    keyword_middle = '}'
    keyword_end = ';'
    position_start = 0
    position_end=0
    flag=0
    content_struct=""
    for i in range(num_struct):
        content_temp=""
        position_start = content_file.find(keyword_start,position_start)
        position_middle = content_file.find(keyword_middle, position_start)
        position_end = content_file.find(keyword_end, position_middle)
        #print(position_end)
        if position_start != -1 and position_middle != -1 and position_end != -1:
            content_temp = content_file[position_start:position_end+2]
            position_start += 1
        else:
            print("警告: %s 中第 %d 个 typedef struct 解析失败（可能包含嵌套花括号），已跳过" % (h_file, i+1))
        content_struct = content_struct + "\n" + content_temp
    content=content+content_struct
#print(content)
with open("struct_defination.txt", "w") as f:
    f.write(content)
