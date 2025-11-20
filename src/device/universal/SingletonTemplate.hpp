#pragma once

#include <memory>
#include <mutex>
#include <utility>


template <typename T> class Singleton {
public:
  Singleton(const Singleton&)            = delete;
  Singleton& operator=(const Singleton&) = delete;

  template <typename... Args> static T* getInstance(Args&&... args) {
    std::call_once(_initInstanceFlag, [&] {
      Singleton::initSingleton(std::forward<Args>(args)...);
    });
    return _instance.get();
  }

  static T* getInstance() {
    std::call_once(_initInstanceFlag, [&] {
      Singleton::initSingleton();
    });
    return _instance.get();
  }

protected:
  Singleton()          = default;
  virtual ~Singleton() = default;

private:
  template <typename... Args> static void initSingleton(Args&&... args) {
    _instance = std::unique_ptr<T>(new T(std::forward<Args>(args)...)); // std::make_uniqe<T>() doesn't work because of private constructor of T
  }

  static std::unique_ptr<T> _instance;
  static std::once_flag _initInstanceFlag;
}; // Class Singleton

template <typename T> std::unique_ptr<T> Singleton<T>::_instance = nullptr;

template <typename T> std::once_flag Singleton<T>::_initInstanceFlag;