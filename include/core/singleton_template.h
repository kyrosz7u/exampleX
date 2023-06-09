//
// Created by kyros on 2023/6/4.
//

#ifndef XEXAMPLE_SINGLETON_TEMPLATE_H
#define XEXAMPLE_SINGLETON_TEMPLATE_H

// 更好的单例模式实现：需要单例化的类用这个装饰器包装下即可
template<class T>
class SingletonTemplate {
public:
    static T &Instance() {
        static T instance;
        return instance;
    }

    SingletonTemplate(const SingletonTemplate &) = delete;

    SingletonTemplate &operator=(const SingletonTemplate &) = delete;

    SingletonTemplate(SingletonTemplate &&) = delete;

protected:
    SingletonTemplate() {}
};

#endif //XEXAMPLE_SINGLETON_TEMPLATE_H
