#pragma once

#include <memory>
#include <vector>
#include <string>
#include <cstring>
#include "common.h"

static const int kPrePendIndex = 8; // prependindex长度
static const int kInitalSize = 1024; // 初始化开辟空间长度

class Buffer{
    public:
        DISALLOW_COPY_AND_MOVE(Buffer);

        Buffer();
        ~Buffer();

        // buffer的起始位置
        char *begin();
        // const对象的begin函数，使得const对象调用begin函数时
        // 所得到的迭代器只能对数据进行读操作，而不能进行修改
        const char *begin() const;

        char *beginread();
        const char *beginread() const;

        char *beginwrite();
        const char *beginwrite() const;


        // 添加数据
        void Append(const char *message);
        void Append(const char *message, int len);
        void Append(const std::string &message);


        // 获得可读大小等
        int readablebytes() const;
        int writablebytes() const;
        int prependablebytes() const;

        // 查看数据，但是不更新`read_index_`位置
        char *Peek();
        const char *Peek() const;
        std::string PeekAsString(int len);
        std::string PeekAllAsString();

        // 取数据，取出后更新read_index,相当于不可重复取
        // 定长
        void Retrieve(int len);
        std::string RetrieveAsString(int len);

        // 全部
        void RetrieveAll();
        std::string RetrieveAllAsString();

        // 某个索引之前
        void RetrieveUtil(const char *end);
        std::string RetrieveUtilAsString(const char *end);

        

        //查看空间
        void EnsureWritableBytes(int len);

    private:
        std::vector<char> buffer_;
        int read_index_;
        int write_index_;
};